#ifndef _BUTTERWORTH_FILTER_H_INCLUDED
#define _BUTTERWORTH_FILTER_H_INCLUDED

/** @file outputFiltering.hpp
*   @brief From Wikipedia :  The Butterworth filter is a type of signal processing filter designed to have a frequency response as flat as possible in the passband. 
*   It is also referred to as a maximally flat magnitude filter.
*   It was first described in 1930 by the British engineer and physicist Stephen Butterworth in his paper entitled "On the Theory of Filter Amplifiers"
*   https://en.wikipedia.org/wiki/Butterworth_filter
* 
*   The frequency response of the Butterworth filter is maximally flat (i.e. has no ripples) in the passband and rolls off towards zero in the stopband.
*   That's why it is used as a post-processing step if you don't disable it from the GUI. It should be noted that this is a relatively new addition to the codebase ( 30 -10-2019 ) 
*   the original BMVC 2019 paper  ( https://www.youtube.com/watch?v=fH5e-KMBvM0  ) did not have any post processing done..!
*
*   However some sort of filtering had to be added after numerous comments regarding signal noise. And here it is, in a header-only vanilla C compatible version. 
*   Thanks to Stelios Piperakis (  https://github.com/mrsp ) for giving me the initial code implementation  that this filter is based on 
*   
*   @author Ammar Qammaz (AmmarkoV)
*/


#include <math.h>

/**
 * @brief The complete state of a Butterworth filter instance  
 */
struct ButterWorth
{
  //https://en.wikipedia.org/wiki/Butterworth_filter
  //https://github.com/mrsp/serow/blob/master/src/butterworthLPF.cpp
  float unfilteredValue;
  float filteredValue;
  //-----------
  char initialized;
  //-----------
  float  a;
  float  fx;
  float  fs;
  float  a1;
  float  a2;
  float  b0;
  float  b1;
  float  b2;
  float  ff;
  float  ita;
  float  q;
  int  i;
  float  y_p;
  float  y_pp;
  float  x_p;
  float  x_pp;
};

/**
 * @brief Initialize a "sensor" using  fsampling/fcutoff values
 * @param Butterworth filter instance
 * @param frequency of sampling
 * @param frequency of cutoff
 */
static void initButterWorth(struct ButterWorth * sensor,float fsampling,float fcutoff)
{
    sensor->fs = fsampling;
    sensor->fx = fcutoff;

    sensor->i   = 0;
    sensor->ff  = (float) sensor->fx/sensor->fs;
    sensor->ita = (float) 1.0/tan((float) 3.14159265359 * sensor->ff);
    sensor->q   = 1.41421356237;
    sensor->b0  = (float) 1.0 / (1.0 + sensor->q*sensor->ita + sensor->ita*sensor->ita);
    sensor->b1  = 2*sensor->b0;
    sensor->b2  = sensor->b0;
    sensor->a1  = 2.0 * (sensor->ita*sensor->ita - 1.0) * sensor->b0;
    sensor->a2  = -(1.0 - sensor->q*sensor->ita + sensor->ita*sensor->ita) * sensor->b0;
    sensor->a   =(float) (2.0*3.14159265359*sensor->ff)/(2.0*3.14159265359*sensor->ff+1.0); 
}


/**
 * @brief Filter a new incoming value and get the result
 * @param Butterworth filter instance
 * @param Unfiltered input value
 * @retval Filtered output value
 */
static float filter(struct ButterWorth * sensor,float unfilteredValue)
{
 sensor->unfilteredValue = unfilteredValue;   
 
 float y = sensor->unfilteredValue; 
 float out;
 if ((sensor->i>2)&&(1))
       {
        out = sensor->b0 * y + sensor->b1 * sensor->y_p + sensor->b2* sensor->y_pp + sensor->a1 * sensor->x_p + sensor->a2 * sensor->x_pp;
       }
    else
       {
        out = sensor->x_p + sensor->a * (y - sensor->x_p);
        sensor->i=sensor->i+1;
       }
     
    sensor->y_pp = sensor->y_p;
    sensor->y_p = y;
    sensor->x_pp = sensor->x_p;
    sensor->x_p = out;
    
    sensor->filteredValue = out;
    
    if (!sensor->initialized)
    {
        //Do a warmup..
        //Make sure we dont start from 0
        
        sensor->initialized=1;
        for (unsigned int i=0; i<5; i++)
        {
          filter(sensor,unfilteredValue);            
        }

         return filter(sensor,unfilteredValue);
    }
    
    return out; 
}

#endif
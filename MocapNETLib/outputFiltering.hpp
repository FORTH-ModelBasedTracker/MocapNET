#pragma once 
/** @file outputFiltering.hpp
 *  @brief From Wikipedia :  The Butterworth filter is a type of signal processing filter designed to have a frequency response as flat as possible in the passband. It is also referred to as a maximally flat magnitude filter.
*  It was first described in 1930 by the British engineer and physicist Stephen Butterworth in his paper entitled "On the Theory of Filter Amplifiers"
*  https://en.wikipedia.org/wiki/Butterworth_filter
* 
*  The frequency response of the Butterworth filter is maximally flat (i.e. has no ripples) in the passband and rolls off towards zero in the stopband.
*   That's why it is used as a post-processing step if you don't disable it from the GUI. It should be noted that this is a relatively new addition to the codebase ( 30 -10-2019 ) 
*   the original BMVC 2019 paper  ( https://www.youtube.com/watch?v=fH5e-KMBvM0  ) did not have any post processing done..!
*
*   However some sort of filtering had to be added after numerous comments regarding signal noise. And here it is. 
*   Thanks to Stelios Piperakis (  https://github.com/mrsp ) for giving me the initial code implementation  that this filter is based on 
*   
 *  @author Ammar Qammaz (AmmarkoV)
 */


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
 */
void initButterWorth(struct ButterWorth * sensor,float fsampling,float fcutoff);



/**
 * @brief Filter a new incoming value and get the result
 */
float filter(struct ButterWorth * sensor,float unfilteredValue);
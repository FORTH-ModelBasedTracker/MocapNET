#include <stdio.h>
#include <stdlib.h>
#include "c.h"
 
void connect()
{
    printf("Connected to C extension...\n");
}
 
//return random value in range of 0-50
int randNum()
{
    int nRand = rand() % 50; 
    return nRand;
}
 
//add two number and return value
int addNum(int a, int b)
{
    int nAdd = a + b;
    return nAdd;
}

int printFloatList(float * l,int lSize)
{
  for (int i=0; i<lSize; i++)
  {
   printf("%u = %f\n",i,l[i]);
  }
}

#include "cameraControl.hpp"

#include <cstdlib>


/**
 * @brief In order to have the best possible quality we can crop the input frame to only perform detection around the area of the previous skeleton
 * This code performs this crop and tries to get the best detection window
 * @ingroup demo
 * @bug This code is oriented to a single 2D skeleton detected, Multiple skeletons will confuse it and there is no logic to handle them
 * @retval 1=Success/0=Failure
 */
int getBestCropWindow(
                       unsigned int * x,
                       unsigned int * y,
                       unsigned int * width,
                       unsigned int * height,
                       struct boundingBox * bbox,
                       unsigned int inputWidth2DJointDetector,
                       unsigned int inputHeight2DJointDetector,
                       unsigned int fullFrameWidth,
                       unsigned int fullFrameHeight
                     )
{
   //fprintf(stderr,"Previous crop started at %u,%u\n", *x,*y);
   if ( (bbox!=0) && (bbox->populated) )
    {
     //fprintf(stderr,"This means that the bounding box (%0.2f,%0.2f) -> (%0.2f,%0.2f)\n",bbox->minimumX,bbox->minimumY,bbox->maximumX,bbox->maximumY);
     bbox->minimumX+=(float) *x;
     bbox->maximumX+=(float) *x;
     bbox->minimumY+=(float) *y;
     bbox->maximumY+=(float) *y;
     //fprintf(stderr,"is actually (%0.2f,%0.2f) -> (%0.2f,%0.2f)\n",bbox->minimumX,bbox->minimumY,bbox->maximumX,bbox->maximumY);
    }

 
   unsigned int dimension = fullFrameHeight;
   float bodyWidth  = bbox->maximumX - bbox->minimumX;
   float bodyHeight = bbox->maximumY - bbox->minimumY;
   //fprintf(stderr,"Body starts at %0.2f and ends at %0.2f for a total of %0.2f pixels\n", bbox->minimumX , bbox->maximumX , bbox->maximumX-bbox->minimumX);
   if (fullFrameWidth>=fullFrameHeight) {
                                         //fprintf(stderr,"Since the whole frame has %u pixels\n",fullFrameWidth);
                                         //fprintf(stderr,"And we will use the dimension of the Y axis aka %u pixels to crop a rectangle\n",fullFrameHeight);

                                         if  ( (bbox!=0) && (bbox->populated) )
                                         {
                                           dimension = fullFrameHeight;  
                                           //TODO:
                                           //if (bodyHeight<dimension) { dimension = bodyHeight; }
                                           //if (dimension<inputHeight2DJointDetector) { dimension = inputHeight2DJointDetector; }

                                           *width=dimension;
                                           *height=dimension;

                                           //-------------------------------------------------------------------------------------------------------
                                           //-------------------------------------------------------------------------------------------------------
                                           //                                         Center on X axis..
                                           //-------------------------------------------------------------------------------------------------------
                                           //-------------------------------------------------------------------------------------------------------
                                           float bodyCenterX =  bbox->minimumX+ (float) bodyWidth/2;
                                           //fprintf(stderr,"The center X of the body lies at %0.2f\n",bodyCenterX);

                                           //fprintf(stderr,"We can afford %u pixels left and right of the body center \n",(unsigned int) *width/2);
                                           float cropStartX = bodyCenterX - *width/2;

                                           if (cropStartX<0) { cropStartX=0; } //Overflow..
                                           if (cropStartX>fullFrameWidth-fullFrameHeight) { cropStartX=fullFrameWidth-fullFrameHeight; }

                                           //Neural Networks cause flicker, if we don't have exactly the same
                                           //bounding box it's ok just keep the previous, it will help with flicker a lot..
                                           if ( abs(*x - (unsigned int) cropStartX) > 4 )
                                             {
                                              *x = (unsigned int) cropStartX;
                                             }
                                           //fprintf(stderr,"This means starting at %u and ending at %u\n",(unsigned int) *x, *x+*width);

/*
                                           //-------------------------------------------------------------------------------------------------------
                                           //-------------------------------------------------------------------------------------------------------
                                           //                                         Center on Y axis..
                                           //-------------------------------------------------------------------------------------------------------
                                           //-------------------------------------------------------------------------------------------------------
                                           float bodyCenterY =  bbox->minimumY+ (float) bodyHeight/2;
                                           //fprintf(stderr,"The center X of the body lies at %0.2f\n",bodyCenterX);

                                           //fprintf(stderr,"We can afford %u pixels up and down of the body center \n",(unsigned int) *height/2);
                                           float cropStartY = bodyCenterY - *height/2;

                                           if (cropStartY<0) { cropStartY=0; } //Overflow..
                                           if (cropStartY>fullFrameHeight) { cropStartY=fullFrameHeight; }

                                           //Neural Networks cause flicker, if we don't have exactly the same
                                           //bounding box it's ok just keep the previous, it will help with flicker a lot..
                                           if ( abs(*y - (unsigned int) cropStartY) > 4 )
                                             {
                                              *y = (unsigned int) cropStartY;
                                             } 
*/


                                         } else
                                         {
                                           //No skeleton? just crop in the center..
                                           *x=(fullFrameWidth-fullFrameHeight)/2;
                                           *y=0;
                                           *width=fullFrameHeight;
                                           *height=fullFrameHeight;
                                         }
                                       } else
   if (fullFrameWidth<fullFrameHeight) {
                                         if  ( (bbox!=0) && (bbox->populated) )
                                         {
                                            //TODO : Implelemnt rotated stream logic  
                                         } else
                                         {
                                           //No skeleton? just crop in the center..
                                           *x=0;
                                           *y=(fullFrameHeight-fullFrameWidth)/2;
                                           *width=fullFrameWidth;
                                           *height=fullFrameWidth;    
                                         }
                                       }
                                          

  return 1;
}


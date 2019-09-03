#include "cameraControl.hpp"

#include <cstdlib>


int populateBoundingBox(
                        struct boundingBox *bbox,
                        std::vector<cv::Point_<float> > points
                       )
{
 if (bbox!=0)
     { //----------------------------------------
      bbox->populated=0;

      if (points.size()>0)
      {
       bbox->minimumX=100000.0;
       bbox->maximumX=0.0;
       bbox->minimumY=100000.0;
       bbox->maximumY=0.0;
       int i;
       
       for (i=0; i<points.size()-1; i++)
       {
        if ( (points[i].x!=0) && (points[i].y!=0) )
        {
         if (bbox->minimumX > points[i].x) { bbox->minimumX=points[i].x; bbox->populated=1; }
         if (bbox->maximumX < points[i].x) { bbox->maximumX=points[i].x; bbox->populated=1; }
         if (bbox->minimumY > points[i].y) { bbox->minimumY=points[i].y; bbox->populated=1; }
         if (bbox->maximumY < points[i].y) { bbox->maximumY=points[i].y; bbox->populated=1; }
        }
       }
        return 1;
      }
     } //----------------------------------------
  return 0;   
}






/**
 * @brief In order to have the best possible quality we can crop the input frame to only perform detection around the area of the previous skeleton
 * This code performs this crop and tries to get the best detection window
 * @ingroup demo
 * @bug This code is oriented to a single 2D skeleton detected, Multiple skeletons will confuse it and there is no logic to handle them
 * @retval 1=Success/0=Failure
 */
int getMaximumCropWindow(
                         unsigned int * x,
                         unsigned int * y,
                         unsigned int * width,
                         unsigned int * height,
                         struct boundingBox * bboxExternal,
                         unsigned int inputWidth2DJointDetector,
                         unsigned int inputHeight2DJointDetector,
                         unsigned int fullFrameWidth,
                         unsigned int fullFrameHeight
                        )
{
     //Make a local copy in case of failure
     struct boundingBox bboxLocal = *bboxExternal;
     struct boundingBox * bbox = &bboxLocal;
     
     float extraSizeWidth=100;
     float extraSizeHeight=150;
     
     if ( (bbox!=0) && (bbox->populated) )
      {
       std::cerr<<"Previous Bounding Box was "<<*x<<","<<*y<<" to "<<*x+*width<<","<<*y+*height<<" \n";
 
       bbox->minimumX+=(float) *x - (float) extraSizeWidth;
       bbox->maximumX+=(float) *x + (float) extraSizeWidth; 
       bbox->minimumY+=(float) *y - (float) extraSizeHeight;
       bbox->maximumY+=(float) *y + (float) extraSizeHeight;
       
       if (bbox->minimumX < 0 ) { bbox->minimumX = 0; }
       if (bbox->minimumY < 0 ) { bbox->minimumY = 0; }
       if (bbox->maximumX > fullFrameWidth )  { bbox->maximumX = fullFrameWidth; }
       if (bbox->maximumY > fullFrameHeight ) { bbox->maximumY = fullFrameHeight; }
       
       float bodyWidth  = bbox->maximumX - bbox->minimumX;
       float bodyHeight = bbox->maximumY - bbox->minimumY;
       float bodyCenterX =  bbox->minimumX + (float) bodyWidth/2;   
       float bodyCenterY =  bbox->minimumY + (float) bodyHeight/2;   
       
       *x=(unsigned int) bbox->minimumX ;
       *y=(unsigned int) bbox->minimumY ;
       *width = (unsigned int) bodyWidth;
       *height= (unsigned int) bodyHeight;
       std::cerr<<"New MAX Bounding Box is "<<*x<<","<<*y<<" to "<<*x+*width<<","<<*y+*height<<" \n";   
       
       int tooSmall=0;
       if (*width<inputWidth2DJointDetector)   { *width=inputWidth2DJointDetector;   tooSmall=1; }  
       if (*height<inputHeight2DJointDetector) { *height=inputHeight2DJointDetector; tooSmall=1; }  
       
       if (tooSmall)
       {
         std::cerr<<"It was too small, new MAX Bounding Box is "<<*x<<","<<*y<<" to "<<*x+*width<<","<<*y+*height<<" \n";     
       }
        
       if  ( (*width!=0) && (*height!=0) ) 
       {
         *bboxExternal=bboxLocal;
         return 1;
       }
      }
     return 0; 
}


/**
 * @brief In order to have the best possible quality we can crop the input frame to only perform detection around the area of the previous skeleton
 * This code performs this crop and tries to get the best detection window
 * @ingroup demo
 * @bug This code is oriented to a single 2D skeleton detected, Multiple skeletons will confuse it and there is no logic to handle them
 * @retval 1=Success/0=Failure
 */
int getBestCropWindow(
                      int maximumCrop,
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
   if (maximumCrop)
   {
       if (  getMaximumCropWindow(
                                                                                   x,
                                                                                   y,
                                                                                   width,
                                                                                   height,
                                                                                   bbox,
                                                                                   inputWidth2DJointDetector,
                                                                                   inputHeight2DJointDetector,
                                                                                   fullFrameWidth,
                                                                                   fullFrameHeight
                                                                      )
            ) { return 1; }
            //If we fail to get Maximum Crop Window we will settle with normal crop window
   }
   
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
                                           float bodyCenterX =  bbox->minimumX + (float) bodyWidth/2;
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

                                             
                                             std::cerr<<"Normal Bounding Box derived "<<*x<<","<<*y<<" to "<<*x+*width<<","<<*y+*height<<" \n";   
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
                                            //TODO : Implement rotated stream logic  
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


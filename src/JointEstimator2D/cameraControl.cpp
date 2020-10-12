#include "cameraControl.hpp"

#include "../MocapNET2/MocapNETLib2/IO/commonSkeleton.hpp"

#include <cstdlib>


const unsigned int noiseThreshold = 30; //pixels


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


    float paddingPercent=0.2; // 0.1 = 10% etc

    if ( (bbox!=0) && (bbox->populated) )
        {
            unsigned int previousX = *x;
            unsigned int previousY = *y;
            unsigned int previousWidth = *width;
            unsigned int previousHeight = *height;

            //std::cerr<<"Previous Bounding Box was "<<*x<<","<<*y<<" to "<<*x+*width<<","<<*y+*height<<" \n";

            // Bring bounding box to normal coordinates
            //------------------------------------------------
            bbox->minimumX+= (float) *x;
            bbox->maximumX+= (float) *x;
            bbox->minimumY+= (float) *y;
            bbox->maximumY+= (float) *y;

            //Calculate the dimensions of the bounding box body and its center
            //----------------------------------------------------------------------------------------------------------
            float bodyWidth  = bbox->maximumX - bbox->minimumX;
            float bodyHeight = bbox->maximumY - bbox->minimumY;
            float bodyCenterX =  bbox->minimumX + (float) bodyWidth/2;
            float bodyCenterY =  bbox->minimumY + (float) bodyHeight/2;


            //If the body is smaller than our input size lets get some more detail in  so we avoid scaling up
            //----------------------------------------------------------------------------------------------------------------------------------------------------
            if (inputWidth2DJointDetector >bodyWidth )
                {
                    bodyWidth = inputWidth2DJointDetector;
                }
            if (inputHeight2DJointDetector> bodyHeight )
                {
                    bodyHeight = inputHeight2DJointDetector;
                }


            //Given that we have our new size and center we can now calculate the new offset from center
            //--------------------------------------------------------------------------------------------------------------------------------------------------------
            float bodyOffsetX =  (float) bodyWidth/2;
            float bodyOffsetY =  (float) bodyHeight/2;

            //And using the new offset derive the new bounding box
            //--------------------------------------------------------------------------------------------------------------------------------------------------------
            float pixelsPaddedX = paddingPercent * bodyWidth;
            float pixelsPaddedY = paddingPercent * bodyHeight;

            //We now have a preliminary bounding box BUT the final bounding box we want is a rectangle so we need to keep the biggest dimension
            //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            if ( bodyOffsetX+pixelsPaddedX> bodyOffsetY+pixelsPaddedY)
                {
                    bodyOffsetX =  bodyOffsetX + pixelsPaddedX;
                    bodyOffsetY =  bodyOffsetX + pixelsPaddedX;
                }
            else
                {
                    bodyOffsetX =  bodyOffsetY + pixelsPaddedY;
                    bodyOffsetY =  bodyOffsetY + pixelsPaddedY;
                }

            //We now have a rectangle bounding box
            //----------------------------------------------------------------
            bbox->minimumX  = bodyCenterX - bodyOffsetX;
            bbox->maximumX  = bodyCenterX + bodyOffsetX;
            bbox->minimumY  = bodyCenterY - bodyOffsetY;
            bbox->maximumY  = bodyCenterY + bodyOffsetY;

            //We do a last  bounds check
            //--------------------------------------------------------------------------------------------------------------------------------------------------------
            signed int limitLX=0,limitLY=0,limitRX=0,limitRY=0;
            if (bbox->minimumX < 0 )
                {
                    limitLX=-1*bbox->minimumX;
                    bbox->minimumX = 0;
                }
            if (bbox->minimumY < 0 )
                {
                    limitLY=-1*bbox->minimumY;
                    bbox->minimumY = 0;
                }
            if (bbox->maximumX > fullFrameWidth )
                {
                    limitRX=bbox->maximumX-fullFrameWidth;
                    bbox->maximumX = fullFrameWidth;
                }
            if (bbox->maximumY > fullFrameHeight )
                {
                    limitRY=bbox->maximumY-fullFrameHeight;
                    bbox->maximumY = fullFrameHeight;
                }


            //We now have a preliminary bounding box the rectangle might be in a corner and fall out of bounds
            //--------------------------------------------------------------------------------------------------------------------------------------------------------

            //We are maybe constrained left/up add what we lost right/down..!
            if (limitLX>0)
                {
                    bbox->maximumX += limitLX;
                }
            if (limitLY>0)
                {
                    bbox->maximumY += limitLY;
                }

            //We are maybe constrained right/down we subtract what we lost left/up ..!
            if (limitRX>0)
                {
                    bbox->minimumX -= limitRX;
                }
            if (limitRY>0)
                {
                    bbox->minimumY -= limitRY;
                }

            if ( (limitLX!=0) || (limitLY!=0) || (limitRX!=0) ||  (limitRY!=0) )
                {
                    //We have failed to initially resolve the bounding box..!
                    //std::cerr<<"Initial resolve the bounding box X1:"<<limitLX<<",Y1:"<<limitLY<<",X2:"<<limitRX<<",Y2:"<<limitRY<<"..\n";
                    //return 0;
                }



            //We one  last time check for bound limits
            limitLX=0,limitLY=0,limitRX=0,limitRY=0;
            if (bbox->minimumX < 0 )
                {
                    limitLX=1;
                    bbox->minimumX = 0;
                }
            if (bbox->minimumY < 0 )
                {
                    limitLY=1;
                    bbox->minimumY = 0;
                }
            if (bbox->maximumX > fullFrameWidth )
                {
                    limitRX=1;
                    bbox->maximumX = fullFrameWidth;
                }
            if (bbox->maximumY > fullFrameHeight )
                {
                    limitRY=1;
                    bbox->maximumY = fullFrameHeight;
                }

            if ( (limitLX!=0) || (limitLY!=0) || (limitRX!=0) ||  (limitRY!=0) )
                {
                    //We have failed to resolve the bounding box..!
                    //std::cerr<<"We have failed to resolve the bounding box X1:"<<limitLX<<",Y1:"<<limitLY<<",X2:"<<limitRX<<",Y2:"<<limitRY<<"..\n";
                    //std::cerr<<"bounding box was X1:"<<bbox->minimumX<<",Y1:"<<bbox->minimumY<<",X2:"<<bbox->maximumX<<",Y2:"<<bbox->maximumY<<"..\n";
                    return 0;
                }


            //And our output is ready for use..!
            //--------------------------------------------------------------------------------------------------------------------------------------------------------
            if (
                ( abs(previousX -  (unsigned int) bbox->minimumX  ) > noiseThreshold )  ||
                ( abs(previousY -  (unsigned int) bbox->minimumY ) > noiseThreshold )   ||
                ( abs(previousWidth  - (unsigned int)  ( bbox->maximumX - bbox->minimumX ) ) > 2*noiseThreshold ) ||
                ( abs(previousHeight - (unsigned int)  ( bbox->maximumY - bbox->minimumY ) ) > 2*noiseThreshold )
            )
                {
                    *x=(unsigned int) bbox->minimumX ;
                    *y=(unsigned int) bbox->minimumY ;
                    *width = (unsigned int) bbox->maximumX - bbox->minimumX;
                    *height= (unsigned int) bbox->maximumY - bbox->minimumY;

                    if (*width == *height +1 )
                        {
                            *width = *height;
                        }

                    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
                    //std::cerr<<"New MAX Bounding Box is "<<*x<<","<<*y<<" to "<<*x+*width<<","<<*y+*height<<"  Image("<<fullFrameWidth<<","<<fullFrameHeight<<")\n";

                    if  ( (*width!=0) && (*height!=0) )
                        {
                            //Success..!
                            *bboxExternal=bboxLocal;
                            return 1;
                        }
                }
            else
                {
                    fprintf(stderr,"Bounding box is too similar with previous so keeping previous..\n");
                    //std::cerr<<"New MAX Bounding Box would have been "<< bbox->minimumX<<","<< bbox->minimumY<<" to "<< bbox->maximumX<<","<< bbox->maximumY<<"  Image("<<fullFrameWidth<<","<<fullFrameHeight<<")\n";
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
    //fprintf(stderr,"getBestCropWindow(%u,%u,%u,%u)\n",*x,*y,*width,*height);
    //fprintf(stderr,"input image size is %u,%u\n",fullFrameWidth,fullFrameHeight);
   if (bbox==0) { return 0; }

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
               )
                {
                    return 1;
                }
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
    if (fullFrameWidth>=fullFrameHeight)
        {
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

                    if (cropStartX<0)
                        {
                            cropStartX=0;    //Overflow..
                        }
                    if (cropStartX>fullFrameWidth-fullFrameHeight)
                        {
                            cropStartX=fullFrameWidth-fullFrameHeight;
                        }

                    //Neural Networks cause flicker, if we don't have exactly the same
                    //bounding box it's ok just keep the previous, it will help with flicker a lot..
                    if ( abs(*x - (unsigned int) cropStartX) > noiseThreshold )
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


                    //std::cerr<<"Normal Bounding Box derived "<<*x<<","<<*y<<" to "<<*x+*width<<","<<*y+*height<<" \n";
                }
            else
                {
                    //No skeleton? just crop in the center..
                    *x=(fullFrameWidth-fullFrameHeight)/2;
                    *y=0;
                    *width=fullFrameHeight;
                    *height=fullFrameHeight;
                }
        }
    else if (fullFrameWidth<fullFrameHeight)
        {
            if  ( (bbox!=0) && (bbox->populated) )
                {
                    dimension = fullFrameWidth;
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
                    float bodyCenterY =  bbox->minimumY + (float) bodyHeight/2;
                    //fprintf(stderr,"The center X of the body lies at %0.2f\n",bodyCenterX);

                    //fprintf(stderr,"We can afford %u pixels left and right of the body center \n",(unsigned int) *width/2);
                    float cropStartY = bodyCenterY - *width/2;

                    if (cropStartY<0)
                        {
                            cropStartY=0;    //Overflow..
                        }
                    if (cropStartY>fullFrameHeight-fullFrameWidth)
                        {
                            cropStartY=fullFrameHeight-fullFrameWidth;
                        }

                    //Neural Networks cause flicker, if we don't have exactly the same
                    //bounding box it's ok just keep the previous, it will help with flicker a lot..
                    if ( abs(*y - (unsigned int) cropStartY) > noiseThreshold )
                        {
                            *y = (unsigned int) cropStartY;
                        }
                }
            else
                {
                    //No skeleton? just crop in the center..
                    *x=0;
                    *y=(fullFrameHeight-fullFrameWidth)/2;
                    *width=fullFrameWidth;
                    *height=fullFrameWidth;
                }
        }

    if  (
        ( *x + *width > fullFrameWidth )  ||
        ( *y + *height > fullFrameHeight )
    )
        {
            //We failed..!
            *x=0;
            *y=0;
            *width=fullFrameWidth;
            *height=fullFrameWidth;
            return 0;
        }

    return 1;
}

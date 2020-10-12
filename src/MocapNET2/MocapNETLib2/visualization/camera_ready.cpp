#include "camera_ready.hpp"
#include "visualization.hpp"
#include "drawSkeleton.hpp"
#include "widgets.hpp"

#include "../IO/jsonRead.hpp"
#include "../IO/bvh.hpp"
#include "../tools.hpp"
#include "../mocapnet2.hpp"


Mat getPaddedROI(const Mat &input, int top_left_x, int top_left_y, int width, int height, Scalar paddingColor)
{
    int bottom_right_x = top_left_x + width;
    int bottom_right_y = top_left_y + height;

    Mat output;
    if (top_left_x < 0 || top_left_y < 0 || bottom_right_x > input.cols || bottom_right_y > input.rows)
        {
            // border padding will be required
            int border_left = 0, border_right = 0, border_top = 0, border_bottom = 0;

            if (top_left_x < 0)
                {
                    width = width + top_left_x;
                    border_left = -1 * top_left_x;
                    top_left_x = 0;
                }
            if (top_left_y < 0)
                {
                    height = height + top_left_y;
                    border_top = -1 * top_left_y;
                    top_left_y = 0;
                }
            if (bottom_right_x > input.cols)
                {
                    width = width - (bottom_right_x - input.cols);
                    border_right = bottom_right_x - input.cols;
                }
            if (bottom_right_y > input.rows)
                {
                    height = height - (bottom_right_y - input.rows);
                    border_bottom = bottom_right_y - input.rows;
                }

            Rect R(top_left_x, top_left_y, width, height);
            copyMakeBorder(input(R), output, border_top, border_bottom, border_left, border_right, BORDER_CONSTANT, paddingColor);
        }
    else
        {
            // no border padding required
            Rect R(top_left_x, top_left_y, width, height);
            output = input(R);
        }
    return output;
}

int getBoundingBox(struct skeletonSerialized * skeleton,cv::Point & pointMinimum,cv::Point & pointMaximum,unsigned int x,unsigned int y,unsigned int width,unsigned int height)
{
    if (skeleton==0)
        {
            return 0;
        }
    if (skeleton->skeletonBodyElements==0)
        {
            return 0;
        }

    unsigned int jID = 3*3 ;

    float xNormalized =  (float) skeleton->skeletonBody[jID+0].value / skeleton->width;
    float yNormalized =  (float) skeleton->skeletonBody[jID+1].value / skeleton->height;

    int performedUpdate=0;
    pointMinimum.x = width;//x+xNormalized*width;
    pointMinimum.y = height;//y+yNormalized*height;
    pointMaximum.x = 0;//x+xNormalized*width;
    pointMaximum.y = 0;//y+yNormalized*height;

    for (int jointID=3; jointID<skeleton->skeletonBodyElements/3; jointID++)
        {
            jID = jointID*3 ;

            xNormalized =  (float) skeleton->skeletonBody[jID+0].value / skeleton->width;
            yNormalized =  (float) skeleton->skeletonBody[jID+1].value / skeleton->height;

            if ( (xNormalized>0.0) && (yNormalized>0.0) )
                {
                    unsigned int fX = x+xNormalized*width;
                    unsigned int fY = y+yNormalized*height;

                    if (fX<pointMinimum.x)
                        {
                            pointMinimum.x = fX;
                            performedUpdate=1;
                        }
                    if (fY<pointMinimum.y)
                        {
                            pointMinimum.y = fY;
                            performedUpdate=1;
                        }

                    if (fX>pointMaximum.x)
                        {
                            pointMaximum.x = fX;
                            performedUpdate=1;
                        }
                    if (fY>pointMaximum.y)
                        {
                            pointMaximum.y = fY;
                            performedUpdate=1;
                        }
                }
        }


    return performedUpdate;
}




int visualizeCameraReady(
    const char* windowName,
    unsigned int frameNumber,
    unsigned int saveVisualization,
    cv::Mat * alreadyLoadedImage,
    const char * path,
    const char * label,
    unsigned int serialLength,
    unsigned int width,
    unsigned int height,
    struct skeletonSerialized * skeleton,
    struct MocapNET2 * mnet,
    std::vector<std::vector<float> > points2DOutputGUIRealView,
    std::vector<std::vector<float> > points2DOutputGUIForcedView,
    std::vector<std::vector<float> > points2DOutputGUIForcedViewSide,
    unsigned int numberOfMissingJoints
)
{
    if (points2DOutputGUIRealView.size()==0)
        {
            return 0;
        }
    if (points2DOutputGUIForcedView.size()==0)
        {
            return 0;
        }
    if (points2DOutputGUIForcedViewSide.size()==0)
        {
            return 0;
        }
    if (path==0)
        {
            fprintf(stderr,"Can't visualize input without path to RGB images\n");
            return 0;
        }

#if USE_OPENCV
    int success=0;
    char finalFilename[2048]= {0};
    cv::Mat raytraced;


    snprintf(finalFilename,256,"%s/raytraced2/%04u.png",path,frameNumber);
    raytraced = imread(finalFilename,cv::IMREAD_COLOR);   // older versions might want the CV_LOAD_IMAGE_COLOR  flag

    if (alreadyLoadedImage==0)
        {

            char formatString[256]= {0};
            snprintf(formatString,256,"%%s/%%s%%0%uu.jpg",serialLength);

            //colorFrame_0_00001.jpg
            snprintf(finalFilename,2048,formatString,path,label,frameNumber/*Frame ID*/);
            //snprintf(finalFilename,2048,"%s/colorFrame_0_%05d.jpg",path,frameNumber+1);
        }



    float scale=1.0;
    cv::Mat image;
    cv::Rect roi;
    cv::Mat destinationROI;

    if ( (fileExists(finalFilename) ) || (alreadyLoadedImage!=0) )
        {
            if (alreadyLoadedImage!=0)
                {
                    image = *alreadyLoadedImage;
                    
                    if  ( 
                          ( image.size().width < 640 ) || 
                          ( image.size().height < 480 )  
                        )
                        {
                            fprintf(stderr,"Very small given frame ( %ux%u..\n",image.size().width,image.size().height);
                            return 0;
                        }
                   
                    //cv::imshow(windowName,image); //
                    //cv::waitKey(0); 
                }
                 else
            if (fileExists(finalFilename) )
                {
                    image = imread(finalFilename,cv::IMREAD_COLOR);   // older versions might want the CV_LOAD_IMAGE_COLOR  flag
                }
                

            if(image.data!=0)
                {
                    if ( image.size().height > image.size().width )
                        {
                            scale=(float) 720/image.size().height;
                        }
                    else
                        {
                            scale=(float) 1024/image.size().width;
                        }
                    if (scale>1.0)
                        {
                            scale=1.0;
                        }
                    if (scale!=1.0)
                        {
                            cv::resize(image, image, cv::Size(0,0), scale, scale);
                        }

                    //fprintf(stderr,"Image ( %u x %u )\n",image.size().width,image.size().height);
                    success=1;
                }

        }
    else
        {
            fprintf(stderr," Could not load %s image, cannot proceed to visualize it\n",finalFilename);
            return 0;
        }


    if (image.empty())
        {
            return 0;
        }

    //int offsetX=950;
    int offsetX=650;
    //cv::Mat visualization(image.size().height,offsetX+image.size().width, CV_8UC3, Scalar(0,0,0));
    cv::Mat visualization(1080,1920, CV_8UC3, Scalar(255,255,255));

    //fprintf(stderr,"Visualization will be ( %u x %u )\n",visualization.size().width,visualization.size().height);
    roi = cv::Rect( cv::Point(offsetX,0 ), cv::Size( image.size().width, image.size().height ));
    destinationROI = visualization( roi );
    //image.copyTo( destinationROI );



    //visualizeInput2DSkeletonFromSkeletonSerialized(visualization,skeleton,offsetX,0, image.size().width,image.size().height);

    //==================================================================================
    unsigned int bbSizeWidth = 700;
    unsigned int bbSizeHeight = 800;

    visualizeSkeletonSerialized(
                                image,
                                skeleton, 
                                (mnet->leftHand.loadedModels>0), 
                                (mnet->rightHand.loadedModels>0),
                                mnet->options->doFace,
                                0,0,
                                image.size().width,
                                image.size().height
                               );

    cv::Point ptMin(0,0),ptMax(0,0);
    if ( getBoundingBox(skeleton,ptMin,ptMax,offsetX,0, image.size().width,image.size().height) )
        {
            //cv::rectangle(visualization,pt1,pt2,cv::Scalar(0,0,0),-1,8,0);
            ptMin.x -= offsetX;
            ptMax.x -= offsetX;

            if (ptMin.x >= 20)
                {
                    ptMin.x-=20;
                }
            if (ptMin.y >= 20)
                {
                    ptMin.y-=20;
                }
            if (ptMax.x <= image.size().width+20)
                {
                    ptMax.x+=20;
                }
            if (ptMax.y <= image.size().height+20)
                {
                    ptMax.y+=20;
                }

        }
    else
        {
            ptMin.x=0;
            ptMax.x=image.size().width-1;
            ptMin.y=0;
            ptMax.y=image.size().height-1;

        }


    roi = cv::Rect(ptMin, cv::Size(ptMax.x-ptMin.x,ptMax.y-ptMin.y));
    //cv::Mat imageROI = image( roi );
    cv::Mat imageROI = getPaddedROI(image,ptMin.x,ptMin.y,ptMax.x-ptMin.x,ptMax.y-ptMin.y,cv::Scalar(123,123,123));

    /*
    float ratio = (float) imageROI.size().height / 800;
    unsigned int originalImageWidth=(unsigned int) imageROI.size().width * ratio;
    unsigned int originalImageHeight=(unsigned int) imageROI.size().height * ratio;
    cv::resize(imageROI,imageROI,cv::Size(originalImageWidth,originalImageHeight));
    */

    unsigned int tX = offsetX + 820 - ((unsigned int) ptMax.x-ptMin.x / 2);
    unsigned int tY = 600  - ( (unsigned int)  ptMax.y-ptMin.y / 2 );

    if (tX +  ptMax.x-ptMin.x > 1920 )
        {
            tX=0;
        }
    if (tY +  ptMax.y-ptMin.y > 1920 )
        {
            tY=0;
        }

    fprintf(stderr,"tx=%u,ty=%u\n",tX,tY);
    cv::Point targetPosition(tX,tY);
    roi = cv::Rect(targetPosition, cv::Size(ptMax.x-ptMin.x,ptMax.y-ptMin.y));

    //destinationROI = visualization( roi );
    destinationROI = getPaddedROI(visualization,roi.x,roi.y,ptMax.x-ptMin.x,ptMax.y-ptMin.y,cv::Scalar(255,255,255));
    cv::addWeighted(destinationROI,0.0, imageROI, 1.0, 0.0, destinationROI);
    //==================================================================================





    cv::Point pt1(offsetX,0);
    cv::Point pt2(offsetX+00,image.size().height);
    //cv::rectangle(visualization,pt1,pt2,cv::Scalar(0,0,0),-1,8,0);

    cv::Scalar color= cv::Scalar(123,123,123,123 /*Transparency here , although if the cv::Mat does not have an alpha channel it is useless*/);
    cv::Scalar black= cv::Scalar(0,0,0,0 /*Transparency here , although if the cv::Mat does not have an alpha channel it is useless*/);
    cv::Point txtPosition;
    txtPosition.y=30;
    float thickness=2.2;
    int fontUsed=cv::FONT_HERSHEY_SIMPLEX;





    //FORCE VIEW..!
    //numberOfMissingJoints=0;

    if (numberOfMissingJoints>40)
        {
            txtPosition.x=60;
            txtPosition.y=350;
            cv::putText(visualization,"Incomplete Data..!",txtPosition,fontUsed,1.8,color,thickness,8);
        }
    else
        {
            txtPosition.x=100;
            //cv::putText(visualization,"Front View",txtPosition,fontUsed,1.1,color,thickness,8);

            txtPosition.x=400;
            //cv::putText(visualization,"Side View",txtPosition,fontUsed,1.1,color,thickness,8);


            drawSkeleton(visualization,points2DOutputGUIForcedView,-730,-230,0);
            drawSkeleton(visualization,points2DOutputGUIForcedViewSide,-450,-230,0);
        }


    scaleSkeleton(points2DOutputGUIRealView,scale,scale);
    //drawSkeleton(visualization,points2DOutputGUIRealView,offsetX-100,0,0);


    if (!raytraced.empty())
        {
            unsigned int clipSizeWidth = 700;
            unsigned int clipSizeHeight = 800;
            cv::Mat clippedRaytraced;
            roi = cv::Rect( cv::Point(600,150),cv::Size(clipSizeWidth,clipSizeHeight));
            clippedRaytraced = raytraced(roi);

            clipSizeWidth=(unsigned int) clipSizeWidth*0.6;
            clipSizeHeight=(unsigned int) clipSizeHeight*0.6;
            cv::resize(clippedRaytraced,clippedRaytraced,cv::Size(clipSizeWidth,clipSizeHeight));

            cv::Mat clippedVisualization;
            roi = cv::Rect( cv::Point(1380,100),cv::Size(clipSizeWidth,clipSizeHeight));
            clippedVisualization = visualization(roi);

            cv::addWeighted(clippedVisualization,0.0, clippedRaytraced, 1.0, 0.0, clippedVisualization);
        }


    int NSDMWidth=160;  //body leftHand rightHand
    int NSDMHeight=160;

    int positionY = 80;
    visualizeNSDM(visualization,"Upper Body",mnet->upperBody.NSDM,1 /*angles*/,770,positionY,NSDMWidth,NSDMHeight);
    positionY+=200;
    visualizeNSDM(visualization,"Lower Body",mnet->lowerBody.NSDM,1 /*angles*/,770,positionY,NSDMWidth,NSDMHeight);
    positionY+=200;

    visualizeOrientation(
        visualization,"Orientation",
        mnet->currentSolution[4], //Orientation value
        mnet->orientationClassifications[0],
        mnet->orientationClassifications[1],
        mnet->orientationClassifications[2],
        mnet->orientationClassifications[3],
        770,
        positionY,
        NSDMWidth,
        NSDMHeight
    );


    cv::imshow(windowName,visualization); //

    if (saveVisualization)
        {
            char filename[1024];
            snprintf(filename,1024,"cr%05u.jpg",frameNumber);
            imwrite(filename,visualization);
        }

    //Did not find a file to show ..
    return success;
#else
    fprintf(stderr,"OpenCV code not present in this build, cannot show visualization..\n");
#endif
    return 0;
}

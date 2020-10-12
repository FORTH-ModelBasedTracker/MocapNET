#include "visualization.hpp"

#include "cameraControl.hpp"

int visualizeHeatmaps(struct JointEstimator2D * jointEstimator,std::vector<std::vector<float> >  heatmapNNOutput,unsigned int frameNumber)
{
    unsigned int cols = jointEstimator->heatmapWidth2DJointDetector;
    unsigned int rows = jointEstimator->heatmapHeight2DJointDetector;
    unsigned int hm = jointEstimator->numberOfHeatmaps;
    std::vector<cv::Mat> heatmaps;
    for(int i=0; i<hm; ++i)
        {
            cv::Mat h(rows,cols, CV_32FC1);
            for(int r=0; r<rows; ++r)
                {
                    for(int c=0; c<cols; ++c)
                        {
                            int pos = r*cols+c;
                            h.at<float>(r,c) = heatmapNNOutput[i][pos];
                        }
                }
            heatmaps.push_back(h);
        }

#define DISPLAY_ALL_HEATMAPS 1
    //This code segment will display every returned heatmap in it's own window..
#if DISPLAY_ALL_HEATMAPS
int visualize=1;
    if (visualize)
        {
            unsigned int x=0;
            unsigned int y=0;
            char windowLabel[128];
            snprintf(windowLabel,128,"T");
            
            for(int i=0; i<18; ++i)
                {
                    snprintf(windowLabel,128,"Heatmap %u",i);
                    if (frameNumber==0)
                        {
                            cv::namedWindow(windowLabel,1);
                            cv::moveWindow(windowLabel, x,y);
                            ++frameNumber;
                        }
                    cv::imshow(windowLabel,heatmaps[i]);
                    y=y+rows+30;
                    if (y>700)
                        {
                            x=x+cols;
                            y=0;
                        }
                }
        }
#endif
        
  return 1;
}


 
//Basic Skeleton Visualization with big fonts to be legible on cluttered skeletons
void dj_drawExtractedSkeletons(
                               cv::Mat img,
                               struct Skeletons2DDetected * sk,
                               float factorX,
                               float factorY
                              )
{
  cv::Scalar red = cv::Scalar(0,0,255);
  cv::Scalar green = cv::Scalar(0,255,0);
  cv::Scalar blue = cv::Scalar(255,0,0);
  cv::Scalar color=blue;
    
//Just the lines ( background layer)
    for (int skID=0; skID<sk->numberOfSkeletonsDetected; skID++)
        {    
           for (int i=0; i<UT_COCO_PARTS; i++)
          { 
            unsigned int jointID = heatmapCorrespondenceToBODY25[i%18];
            struct point2D * jointPoint = & sk->skeletons[skID].body.joint2D[jointID];

            unsigned int parentID = heatmapCorrespondenceToBODY25[UT_COCOSkeletonJointsParentRelationMap[i%18]];
            if (parentID!=jointID)
                {
                    struct point2D * parentPoint = & sk->skeletons[skID].body.joint2D[parentID]; 

                    if (  
                            (jointPoint->x > 0) && 
                            (jointPoint->y > 0) && 
                            (parentPoint->x > 0) && 
                            (parentPoint->y > 0) 
                        )
                        {
                            cv::Point_<float> jointPointCV; 
                            jointPointCV.x = jointPoint->x   * factorX;
                            jointPointCV.y = jointPoint->y   * factorY;
                            cv::Point_<float> parentPointCV; 
                            parentPointCV.x = parentPoint->x   * factorX;
                            parentPointCV.y = parentPoint->y   * factorY;
                            
                            switch (UT_COCOSkeletonJointsPartLabel[i%18])
                            {
                                case JOINT_2D_ESTIMATOR_PART_OF_TORSO:
                                case JOINT_2D_ESTIMATOR_PART_OF_HEAD: 
                                  color=blue;
                                break;
                                case JOINT_2D_ESTIMATOR_PART_OF_LEFT: 
                                  color=red;
                                break;
                                case JOINT_2D_ESTIMATOR_PART_OF_RIGHT: 
                                  color=green;
                                break;   
                            };
                            
                            cv::line(img,jointPointCV,parentPointCV,color,2.0);
                        }
                }
          }
        }


//Just the points and text ( foreground )
       char textInfo[512]; 
        for (int skID=0; skID<sk->numberOfSkeletonsDetected; skID++)
        {
         for (int i=0; i<UT_COCO_PARTS; i++)
          {
            unsigned int jointID = heatmapCorrespondenceToBODY25[i%18];
            struct point2D * jointPoint = & sk->skeletons[skID].body.joint2D[jointID];
            
            cv::Point_<float> jointPointCV;
            jointPointCV.x = jointPoint->x   * factorX;
            jointPointCV.y = jointPoint->y   * factorY;

            if ( (jointPointCV.x>0) && (jointPointCV.y>0) )
                {
                    cv::circle(img,jointPointCV,3,cv::Scalar(255,0,0),3,8,0);
                    snprintf(textInfo,512,"%s(%u)",UT_COCOBodyNames[i],i);
                    cv::putText(img, textInfo  , jointPointCV, cv::FONT_HERSHEY_DUPLEX, 0.3, cv::Scalar::all(255), 1,8);
                }
           }
        }
}




int cropAndResizeCVMatToMatchSkeleton(
    struct JointEstimator2D * jest,
    cv::Mat  & frame,
    struct Skeletons2DDetected * sk
)
{
    int tryForMaximumCrop=0;
    int result=0;
    
    jest->crop.frameWidth=frame.size().width;
    jest->crop.frameHeight=frame.size().height;

    if (sk->numberOfSkeletonsDetected>0)
        { 
            //------------------------------------------------------------------------------------
            sk->skeletons[0].bbox2D.minimumX = sk->skeletons[0].body.bbox2D[0].x;
            sk->skeletons[0].bbox2D.minimumY = sk->skeletons[0].body.bbox2D[0].y; 
            convertNormalized2DJointsToOriginalImageCoordinates(
                                                                jest,
                                                                &sk->skeletons[0].bbox2D.minimumX,
                                                                &sk->skeletons[0].bbox2D.minimumY,
                                                                0
                                                               );                                                               
            //------------------------------------------------------------------------------------
            sk->skeletons[0].bbox2D.maximumX = sk->skeletons[0].body.bbox2D[1].x;
            sk->skeletons[0].bbox2D.maximumY = sk->skeletons[0].body.bbox2D[1].y; 
            convertNormalized2DJointsToOriginalImageCoordinates(
                                                                jest,
                                                                &sk->skeletons[0].bbox2D.maximumX,
                                                                &sk->skeletons[0].bbox2D.maximumY,
                                                                0
                                                               );
            //------------------------------------------------------------------------------------
                                                        
            sk->skeletons[0].bbox2D.populated=1;
        }

    if (
        getBestCropWindow(
            tryForMaximumCrop,
            &jest->crop.offsetX,
            &jest->crop.offsetY,
            &jest->crop.croppedDimensionWidth,
            &jest->crop.croppedDimensionHeight,
            &sk->skeletons[0].bbox2D,
            jest->inputWidth2DJointDetector,
            jest->inputHeight2DJointDetector,
            frame.size().width,
            frame.size().height
        )
      )
        {
            if (jest->crop.croppedDimensionWidth!=jest->crop.croppedDimensionHeight)
                {
                    fprintf(stderr,"Bounding box produced was not a rectangle (%ux%u)..!\n",jest->crop.croppedDimensionWidth,jest->crop.croppedDimensionHeight);
                }
            cv::Rect rectangleROI(
                jest->crop.offsetX,
                jest->crop.offsetY,
                jest->crop.croppedDimensionWidth,
                jest->crop.croppedDimensionHeight
            );
            frame = frame(rectangleROI);
            sk->skeletons[0].bbox2D.populated=0;
            result=1;
        }
    
    cv::resize(frame,frame,cv::Size(jest->inputWidth2DJointDetector,jest->inputHeight2DJointDetector));
    return result;
}

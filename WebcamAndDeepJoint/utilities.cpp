/** @file utilities.cpp
 *  @brief This is a suite of utilities that facilitate extracting 2D points from heatmaps. This is needed since 2D estimators output heatmaps, while MocapNET operates on
 *  2D point input. This code does not utilize PAFs so although some care is taken to get subpixel accuracy from the heatmap maxima it is still less accurate than the
 *  original OpenPose implementation etc.
 *  @author Damien Michel, Pashalis Padeleris, Ammar Qammaz (AmmarkoV)
 */

#include "utilities.hpp"
#include "../MocapNETLib/jsonCocoSkeleton.h"



/*=======================================================================================================*/
cv::Rect dj_getImageRect( cv::Mat &im )
{
     return cv::Rect(0,0,im.cols,im.rows);
}
/*==================================================================================================*/
/*just create a bounding box centered around the pt and inside the image
(will not be centered if partially outside of image)*/
cv::Rect dj_getBBAround2DPoint(cv::Point_<float>  pt,int radius,cv::Mat &im)
{
     cv::Rect imRect = dj_getImageRect(im);
     cv::Rect outBB(pt.x-radius,pt.y-radius,2*radius+1,2*radius+1);
     return imRect&outBB;
}

/*======================================================================================================================================================*/
/*=======================================================================================================*/
/*change the coordinate system, and get subpixel accuracy.*/
cv::Point_<float>   dj_upscalePeakPosition(cv::Point_<float>  &pos, cv::Mat &map, cv::Mat &refOutMap)
{
     int smoothRadius = 5;

     float scaleFactorX = (float) (refOutMap.cols/(float)map.cols);
     float scaleFactorY = (float) (refOutMap.rows/(float)map.rows);

     //float scaleFactorX = (float) (refOutMap.cols/(float)map.cols);
     //float scaleFactorY = (float) (refOutMap.rows/(float)map.rows);
     //std::cerr<<"heatmap size is ("<<map.cols<<","<<map.rows<<") => ("<<refOutMap.cols<<","<<refOutMap.rows<<")\n";

     cv::Rect bb = dj_getBBAround2DPoint(pos,smoothRadius,map);

     cv::Mat area;
     cv::resize(map(bb),area,cv::Size(bb.width,bb.height));
     cv::GaussianBlur(area,area,cv::Size(3,3),0);

     //std::cerr<<"bbox is ("<<bb.x<<","<<bb.y<<") / ("<<map.cols<<","<<map.rows<<") \n";

     cv::Point_<float> maxPt;
     float maxVal = 0;
     for (int y = 0; y<area.rows; ++y)
     {
         float *mapLine = area.ptr<float>(y);
         for (int x = 0; x<area.cols; ++x)
         {
             float &valTp = mapLine[x];
             if (valTp>maxVal)
             {
                 maxVal = valTp;
                 maxPt = cv::Point_<float> (x, y);
             }
         }
     }

     //std::cerr<<"max is ("<<maxPt.x<<","<<maxPt.y<<") \n";
     cv::Point_<float> outPt = cv::Point_<float>(scaleFactorX*(bb.x+maxPt.x)  ,scaleFactorY*(bb.y+maxPt.y) ); 
     
     //std::cout<<"peak: "<<pos.x<<","<<pos.y<<"   to: "<<outPt.x<<","<<outPt.y<<" scale:"<<scaleFactorX<<""<<scaleFactorY<<std::endl;////////////

     return outPt;
}




/*==================================================================*/
/*get the refined pos of the peaks inside the map, within the coords of the outputImageRef.*/
void dj_extractPeaksFromMap(cv::Mat &mapIn,int mapID,float minThreshold, cv::Mat &outputImageRef , std::vector<unsigned int> &mapLabels, std::vector<cv::Point_<float> >  &outPoints)
{
     int maxPeakCount = 3;
     int peakRadius = 5;

     cv::Mat map = mapIn.clone();

     std::vector<cv::Point_<float> >  rawPeaks;//in the input map coords

     for (int i = 0; i<maxPeakCount; ++i)
     {
         //just get the max val for now, should be the max val around some area
         cv::Point_<float> maxPt;
         float maxVal = 0;
         for (int y = 0; y<map.rows; ++y)
         {
             float *mapLine = map.ptr<float>(y);
             for (int x = 0; x<map.cols; ++x)
             {
                 float &valTp = mapLine[x];
                 if (valTp<minThreshold) continue;
                 if (valTp>maxVal)
                 {
                     maxVal = valTp;
                     maxPt = cv::Point_<float> (x, y);
                 }
             }
         }

         if (maxVal>0)
         {
             rawPeaks.push_back(maxPt);
             cv::Rect bb = dj_getBBAround2DPoint(maxPt,peakRadius,map);
             map(bb).setTo(0);
         }
         else break;
     }

     //std::cout<<"im type: channels:"<<map.channels()<<"  depth:"<<map.depth()<<"maxval: "<<maxVal<<std::endl;//////////////////////////


     //std::cerr<<"mapID "<<mapID<<std::endl;//////////////////////////
     //std::cerr<<"rawPeaks.size="<<rawPeaks.size()<<std::endl;//////////////////////////
     for (int i=0;i<rawPeaks.size();++i)
     {
      //std::cerr<<"rawPeaks["<<i<<"]=("<<rawPeaks[i].x<<","<<rawPeaks[i].y<<") -> ";
      cv::Point_<float>  upscaledPeak = dj_upscalePeakPosition(rawPeaks[i],mapIn,outputImageRef);
      //std::cerr<<"upscaledPeak["<<i<<"]=("<<upscaledPeak.x<<","<<upscaledPeak.y<<") -> ";
      outPoints.push_back(upscaledPeak);
      mapLabels.push_back(mapID);
     }
     //std::cout<<"peaks: "<<rawPeaks.size()<<" "<<outPoints.size()<<std::endl;////////////////
}



//This is is a very stupid way to get a single set of skeleton joints but it works for one person ,
//a more sophisticated method would also require an input bounding box and only accept points inside it..
//which is a TODO :
void dj_extractSkeletonsFromPeaks(std::vector<unsigned int> mapLabels, std::vector<cv::Point_<float> >  peaks,std::vector<cv::Point_<float> > &skeletons)
{
  //Bad implementation
  skeletons.clear();
  skeletons.resize(18);
  for (int i=0; i<peaks.size(); i++)
        {
          cv::Point_<float> skelPoint = peaks[i];
          if (mapLabels[i]<18)
            { skeletons[mapLabels[i]]=skelPoint; }
        }
}



//Basic Skeleton Visualization with big fonts to be legible on cluttered skeletons
void dj_drawExtractedSkeletons(
                                                                  cv::Mat img,
                                                                  std::vector<cv::Point_<float> > skeletons,
                                                                  float factorX,
                                                                  float factorY
                                                                )
{
 char textInfo[512];

 //Just the lines ( background layer)
 for (int i=0; i<skeletons.size(); i++)
        {
          unsigned int jointID = i%18;
          cv::Point_<float>  jointPoint = skeletons[jointID];


          unsigned int parentID = UT_COCOSkeletonJointsParentRelationMap[jointID];
          if (parentID!=jointID)
          {
           cv::Point_<float> parentPoint = skeletons[parentID];

           if ( ( (jointPoint.x>0) && (jointPoint.y>0) && (parentPoint.x>0) && (parentPoint.y>0) ) )
           {
             jointPoint.x = jointPoint.x   * factorX;
             jointPoint.y = jointPoint.y   * factorY;
             parentPoint.x = parentPoint.x   * factorX;
             parentPoint.y = parentPoint.y   * factorY;
             
             cv::line(img,jointPoint,parentPoint, cv::Scalar(0,255,0), 4.0);
           }

          }
        }

 //Just the points and text ( foreground )
 for (int i=0; i<skeletons.size(); i++)
        {
          unsigned int jointID = i%18;
          cv::Point_<float> jointPoint = skeletons[jointID];

          jointPoint.x = jointPoint.x   * factorX;
          jointPoint.y = jointPoint.y   * factorY;

          if ( (jointPoint.x>0) && (jointPoint.y>0) )
           {
             cv::circle(img,jointPoint,5,cv::Scalar(255,0,0),3,8,0);
             snprintf(textInfo,512,"%s(%u)",UT_COCOBodyNames[i],i);
             cv::putText(img, textInfo  , jointPoint, cv::FONT_HERSHEY_DUPLEX, 0.2, cv::Scalar::all(255), 1,8);
           }
        }
}



std::vector<cv::Point_<float> > dj_getNeuralNetworkDetectionsForColorImage( 
                                                                                                                                                              cv::Mat colorImageOriginal ,
                                                                                                                                                              cv::Mat colorImageSmall,
                                                                                                                                                               std::vector<cv::Mat> heatmaps , 
                                                                                                                                                               float minThreshold ,
                                                                                                                                                               int visualize 
                                                                                                                                                            )
{
        std::vector<cv::Point_<float> > outPoints;
        std::vector<unsigned int> outLabels;


        for (unsigned int i=0; i<heatmaps.size(); i++)
        {
         int previousSize = outPoints.size();

         dj_extractPeaksFromMap(
                                                             heatmaps[i],        //Heatmap Input..
                                                             i,                  //ID of heatmap
                                                             minThreshold,       //Threshold
                                                             colorImageOriginal, //Input Image
                                                             outLabels,          //Output labels
                                                             outPoints           //Output points
                                                           );
         //outPoints.push_back(outPoint);
         if (outPoints.size()==previousSize)
         {
           outPoints.push_back(cv::Point_<float>(0,0));
           outLabels.push_back(666);
         }
        }
        //std::cerr<<"Outpoints "<<outPoints.size()<<"  "<<std::endl;

         // show bg:
        cv::Mat bg = heatmaps[18];
        cv::Mat bgNorm, bgRes;

        //std::cerr<<"Got bg "<<heatmaps.size()<<" heatmaps. Size "<<bg.cols<<", "<<bg.rows <<std::endl;
        cv::normalize(bg,bgNorm,0,255, CV_MINMAX, CV_8UC1);
        //std::cerr<<" bgNorm Size "<<bgNorm.cols<<", "<<bgNorm.rows <<std::endl;
        cv::resize(bgNorm,bgRes,cv::Size(0,0),8,8);
        //std::cerr<<" bgRes Size "<<bgRes.cols<<", "<<bgRes.rows <<std::endl;



        std::vector<cv::Point_<float> > skeletons;
        dj_extractSkeletonsFromPeaks(outLabels,outPoints,skeletons);


        if (visualize)
        {
         cv::imshow("2D NN Heatmaps",bgRes);
         
         //Do not taint input image..
         #define BIG_WINDOW 0
         
         #if BIG_WINDOW
           cv::Mat visualizationImage2DSkeleton = colorImageOriginal.clone(); 
           dj_drawExtractedSkeletons(visualizationImage2DSkeleton,skeletons,1.0,1.0);
           cv::imshow("2D Detections",visualizationImage2DSkeleton); 
         #else
           cv::Mat visualizationImage2DSkeleton = colorImageSmall.clone();
           float factorX =   (float) colorImageSmall.cols  / colorImageOriginal.cols ;
           float factorY =   (float) colorImageSmall.rows / colorImageOriginal.rows ;
           
           dj_drawExtractedSkeletons(
                                                                    visualizationImage2DSkeleton,
                                                                    skeletons,
                                                                    factorX ,
                                                                    factorY
                                                                  );
                                                                  
           cv::imshow("2D Detections",visualizationImage2DSkeleton);  
         #endif
          
         int k = cv::waitKey(1);
        }


        return skeletons;
}





void convertUtilitiesSkeletonFormatToBODY25(struct skeletonCOCO * sk, std::vector<cv::Point_<float> > points)
{
  sk->joint2D[BODY25_Nose].x = points[UT_COCO_Nose].x;
  sk->joint2D[BODY25_Nose].y = points[UT_COCO_Nose].y;

  sk->joint2D[BODY25_Neck].x = points[UT_COCO_Neck].x;
  sk->joint2D[BODY25_Neck].y = points[UT_COCO_Neck].y;

  sk->joint2D[BODY25_RShoulder].x = points[UT_COCO_RShoulder].x;
  sk->joint2D[BODY25_RShoulder].y = points[UT_COCO_RShoulder].y;

  sk->joint2D[BODY25_RElbow].x = points[UT_COCO_RElbow].x;
  sk->joint2D[BODY25_RElbow].y = points[UT_COCO_RElbow].y;

  sk->joint2D[BODY25_RWrist].x = points[UT_COCO_RWrist].x;
  sk->joint2D[BODY25_RWrist].y = points[UT_COCO_RWrist].y;

  sk->joint2D[BODY25_LShoulder].x = points[UT_COCO_LShoulder].x;
  sk->joint2D[BODY25_LShoulder].y = points[UT_COCO_LShoulder].y;

  sk->joint2D[BODY25_LElbow].x = points[UT_COCO_LElbow].x;
  sk->joint2D[BODY25_LElbow].y = points[UT_COCO_LElbow].y;

  sk->joint2D[BODY25_LWrist].x = points[UT_COCO_LWrist].x;
  sk->joint2D[BODY25_LWrist].y = points[UT_COCO_LWrist].y;

  if (
       (points[UT_COCO_RHip].x!=0) && (points[UT_COCO_LHip].x!=0) &&
       (points[UT_COCO_RHip].y!=0) && (points[UT_COCO_LHip].y!=0)
     )
  {
   sk->joint2D[BODY25_MidHip].x = (points[UT_COCO_RHip].x+points[UT_COCO_LHip].x)/2;
   sk->joint2D[BODY25_MidHip].y = (points[UT_COCO_RHip].y+points[UT_COCO_LHip].y)/2;
  }

  sk->joint2D[BODY25_RHip].x = points[UT_COCO_RHip].x;
  sk->joint2D[BODY25_RHip].y = points[UT_COCO_RHip].y;

  sk->joint2D[BODY25_RKnee].x = points[UT_COCO_RKnee].x;
  sk->joint2D[BODY25_RKnee].y = points[UT_COCO_RKnee].y;

  sk->joint2D[BODY25_RAnkle].x = points[UT_COCO_RAnkle].x;
  sk->joint2D[BODY25_RAnkle].y = points[UT_COCO_RAnkle].y;

  sk->joint2D[BODY25_LHip].x = points[UT_COCO_LHip].x;
  sk->joint2D[BODY25_LHip].y = points[UT_COCO_LHip].y;

  sk->joint2D[BODY25_LKnee].x = points[UT_COCO_LKnee].x;
  sk->joint2D[BODY25_LKnee].y = points[UT_COCO_LKnee].y;

  sk->joint2D[BODY25_LAnkle].x = points[UT_COCO_LAnkle].x;
  sk->joint2D[BODY25_LAnkle].y = points[UT_COCO_LAnkle].y;

  sk->joint2D[BODY25_REye].x = points[UT_COCO_REye].x;
  sk->joint2D[BODY25_REye].y = points[UT_COCO_REye].y;

  sk->joint2D[BODY25_LEye].x = points[UT_COCO_LEye].x;
  sk->joint2D[BODY25_LEye].y = points[UT_COCO_LEye].y;

  sk->joint2D[BODY25_REar].x = points[UT_COCO_REar].x;
  sk->joint2D[BODY25_REar].y = points[UT_COCO_REar].y;

  sk->joint2D[BODY25_LEar].x = points[UT_COCO_LEar].x;
  sk->joint2D[BODY25_LEar].y = points[UT_COCO_LEar].y;

  sk->joint2D[BODY25_Bkg].x = points[UT_COCO_Bkg].x;
  sk->joint2D[BODY25_Bkg].y = points[UT_COCO_Bkg].y;
  
  }
#include "utilities.hpp"

/*=======================================================================================================*/
cv::Rect dj_getImageRect( cv::Mat &im )
{
     return cv::Rect(0,0,im.cols,im.rows);
}
/*==================================================================================================*/
/*just create a bounding box centered around the pt and inside the image
(will not be centered if partially outside of image)*/
cv::Rect dj_getBBAround2DPoint(cv::Point pt,int radius,cv::Mat &im)
{
     cv::Rect imRect = dj_getImageRect(im);
     cv::Rect outBB(pt.x-radius,pt.y-radius,2*radius+1,2*radius+1);
     return imRect&outBB;
}

/*======================================================================================================================================================*/
/*=======================================================================================================*/
/*change the coordinate system, and get subpixel accuracy.*/
cv::Point dj_upscalePeakPosition(cv::Point &pos, cv::Mat &map, cv::Mat &refOutMap)
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

     cv::Point maxPt;
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
                 maxPt = cv::Point(x, y);
             }
         }
     }

     //std::cerr<<"max is ("<<maxPt.x<<","<<maxPt.y<<") \n";
     cv::Point2f outPtf(scaleFactorX*(bb.x+maxPt.x)  ,scaleFactorY*(bb.y+maxPt.y) );
     cv::Point   outPt = outPtf;
     //cv::Point2f outPtf((pos.x+0.5)*scaleFactorX-0.5,(pos.y+0.5)*scaleFactorY-0.5);
     //std::cout<<"peak: "<<pos.x<<","<<pos.y<<"   to: "<<outPt.x<<","<<outPt.y<<" scale:"<<scaleFactorX<<""<<scaleFactorY<<std::endl;////////////

     return outPt;
}




/*==================================================================*/
/*get the refined pos of the peaks inside the map, within the coords of the outputImageRef.*/
void dj_extractPeaksFromMap(cv::Mat &mapIn,int mapID,float minThreshold, cv::Mat &outputImageRef , std::vector<unsigned int> &mapLabels, std::vector<cv::Point> &outPoints)
{
     int maxPeakCount = 3;
     int peakRadius = 5;

     cv::Mat map = mapIn.clone();

     std::vector<cv::Point> rawPeaks;//in the input map coords

     for (int i = 0; i<maxPeakCount; ++i)
     {
         //just get the max val for now, should be the max val around some area
         cv::Point maxPt;
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
                     maxPt = cv::Point(x, y);
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
      cv::Point upscaledPeak = dj_upscalePeakPosition(rawPeaks[i],mapIn,outputImageRef);
      //std::cerr<<"upscaledPeak["<<i<<"]=("<<upscaledPeak.x<<","<<upscaledPeak.y<<") -> ";
      outPoints.push_back(upscaledPeak);
      mapLabels.push_back(mapID);
     }
     //std::cout<<"peaks: "<<rawPeaks.size()<<" "<<outPoints.size()<<std::endl;////////////////
}



//This is is a very stupid way to get a single set of skeleton joints but it works for one person ,
//a more sophisticated method would also require an input bounding box and only accept points inside it..
//which is a TODO :
void dj_extractSkeletonsFromPeaks(std::vector<unsigned int> mapLabels, std::vector<cv::Point> peaks,std::vector<cv::Point> &skeletons)
{
  //Bad implementation
  skeletons.clear();
  skeletons.resize(18);
  for (int i=0; i<peaks.size(); i++)
        {
          cv::Point skelPoint = peaks[i];
          if (mapLabels[i]<18)
            { skeletons[mapLabels[i]]=skelPoint; }
        }
}



//Basic Skeleton Visualization with big fonts to be legible on cluttered skeletons
void dj_drawExtractedSkeletons(cv::Mat img,std::vector<cv::Point> skeletons)
{
 char textInfo[512];

 //Just the lines ( background layer)
 for (int i=0; i<skeletons.size(); i++)
        {
          unsigned int jointID = i%18;
          cv::Point jointPoint = skeletons[jointID];


          unsigned int parentID = UT_COCOSkeletonJointsParentRelationMap[jointID];
          if (parentID!=jointID)
          {
           cv::Point parentPoint = skeletons[parentID];

           if ( ( (jointPoint.x>0) && (jointPoint.y>0) && (parentPoint.x>0) && (parentPoint.y>0) ) )
           {
             cv::line(img,jointPoint,parentPoint, cv::Scalar(0,255,0), 5.0);
           }

          }
        }

 //Just the points and text ( foreground )
 for (int i=0; i<skeletons.size(); i++)
        {
          unsigned int jointID = i%18;
          cv::Point jointPoint = skeletons[jointID];


          if ( (jointPoint.x>0) && (jointPoint.y>0) )
           {
             cv::circle(img,jointPoint,5,cv::Scalar(255,0,0),3,8,0);
             snprintf(textInfo,512,"%s(%u)",UT_COCOBodyNames[i],i);
             cv::putText(img, textInfo  , jointPoint, cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar::all(255), 1,8);
           }
        }
}



std::vector<cv::Point> dj_getNeuralNetworkDetectionsForColorImage( cv::Mat colorImageOriginal , std::vector<cv::Mat> heatmaps  ,  float minThreshold , int visualize )
{
        std::vector<cv::Point> outPoints;
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
           outPoints.push_back(cv::Point(0,0));
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



        std::vector<cv::Point> skeletons;
        dj_extractSkeletonsFromPeaks(outLabels,outPoints,skeletons);


        if (visualize)
        {
         //Do not taint input image..
         cv::Mat colorImage = colorImageOriginal.clone();
         dj_drawExtractedSkeletons(colorImage,skeletons);
         cv::imshow("BG",bgRes);
         cv::imshow("DETECTION",colorImage);
         int k = cv::waitKey(1);
        }


        return skeletons;
}

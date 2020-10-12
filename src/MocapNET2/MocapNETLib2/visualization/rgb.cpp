
#if USE_OPENCV

#include "opencv2/opencv.hpp"
using namespace cv;

int visualizeCameraIntensities(const char* windowName, cv::Mat &imgOriginal,int forceColors)
{
    float fontSize = 0.3;
    unsigned int verticalSpace=25;
    unsigned int horizontalSpace=10;

    unsigned int x=0,y=10;
    char text[512];
    if ( (imgOriginal.rows!=0) && (imgOriginal.cols!=0) )
        {
            cv::Mat img(32,32, CV_8UC3, cv::Scalar(0,0,0));
            cv::resize(imgOriginal, img, img.size() ,0,0,INTER_NEAREST);
            //cv::imshow(windowName,img);
            cv::Mat imgV((3+img.cols) * horizontalSpace *3 ,img.rows * verticalSpace, CV_8UC3,cv::Scalar(0,0,0));
            for(int r=0; r<img.rows; ++r)
                {
                    x=0;
                    for(int c=0; c<img.cols; ++c)
                        {
                            unsigned char * p = img.ptr(r,c); // Y first, X after
                            cv::Point jointPoint(x,y);
                            snprintf(text,512,"%u",p[0]);
                            if (forceColors) p[0]=255;
                            cv::putText(imgV, text  , jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(p[0],0,0), 0.2, cv::LINE_4 );
                            x=x+horizontalSpace;
                            jointPoint.x=x;
                            snprintf(text,512,"%u",p[1]);
                            if (forceColors) p[1]=255;
                            cv::putText(imgV, text  , jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(0,p[1],0), 0.2, cv::LINE_4 );
                            x=x+horizontalSpace;
                            jointPoint.x=x;
                            snprintf(text,512,"%u",p[2]);
                            if (forceColors) p[2]=255;
                            cv::putText(imgV, text  , jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(0,0,p[2]), 0.2, cv::LINE_4 );
                            x=x+horizontalSpace;
                            jointPoint.x=x;
                        }
                    y=y+verticalSpace;
                }


            cv::Point jointPoint(10,10);
            fontSize = 0.5;
            cv::putText(imgV,"Raw RGB Image Data", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,255,255), 0.2, cv::LINE_8);
            cv::imshow(windowName,imgV);
            cv::waitKey(1);
            return 1;
        }
    return 0;
}




int visualizeCameraChannels(const char* windowName,cv::Mat &img,int channelNumber)
{
    if ( (img.rows!=0) && (img.cols!=0) )
        {

            Mat channel[3];
            // The actual splitting.
            split(img, channel);

            cv::Point jointPoint(10,10);
            float fontSize = 0.5;


            if (channelNumber<3)
                {
                    if (channelNumber==0)
                        {
                            cv::putText(channel[channelNumber],"Blue channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,0,0), 0.2, cv::LINE_8);
                        }
                    else if (channelNumber==1)
                        {
                            cv::putText(channel[channelNumber],"Green channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(0,255,0), 0.2, cv::LINE_8);
                        }
                    else if (channelNumber==2)
                        {
                            cv::putText(channel[channelNumber],"Red channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(0,0,255), 0.2, cv::LINE_8);
                        }
                    cv::imshow(windowName, channel[channelNumber]);
                }
            else // if (channelNumber>=3)
                {
                    cv::Mat imageMerged;

                    if (channelNumber==3)
                        {
                            channel[0]=cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
                            cv::merge(channel,3,imageMerged);
                            cv::putText(imageMerged,"Green + Red channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(0,255,255), 0.2, cv::LINE_8);
                        }
                    else   //Set blue channel to 0
                        if (channelNumber==4)
                            {
                                channel[1]=cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
                                cv::merge(channel,3,imageMerged);
                                cv::putText(imageMerged,"Blue + Red channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,0,255), 0.2, cv::LINE_8);
                            }
                        else   //Set blue channel to 0
                            if (channelNumber==5)
                                {
                                    channel[2]=cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
                                    cv::merge(channel,3,imageMerged);
                                    cv::putText(imageMerged,"Green + Blue channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,255,0), 0.2, cv::LINE_8);
                                }
                            else     //Set blue channel to 0
                                {
                                    cv::merge(channel,3,imageMerged);
                                    cv::putText(imageMerged,"Red + Green + Blue channel", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,255,255), 0.2, cv::LINE_8);
                                }

                    cv::imshow(windowName,imageMerged);
                }


            return 1;
        }
    return 0;
}



int visualizeCameraEdges(const char* windowName,cv::Mat &img)
{
    cv::Mat edges;
    //cv::cvtColor(img, edges,BGR2GRAY);

    cv::Canny(edges, edges, 30, 60);

    cv::Point jointPoint(10,10);
    float fontSize = 0.5;
    cv::putText(edges,"Edges", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,255,255), 0.2, cv::LINE_8);
    cv::imshow(windowName, edges);
    return 1;
}



int visualizeCameraFeatures(const char* windowName,cv::Mat &img)
{
    std::vector<cv::KeyPoint> keypoints;

    cv::Mat imgCopy; //= img.clone();
    cv::resize(img, imgCopy, img.size() ,0,0,INTER_NEAREST);
    cv::FAST(imgCopy,keypoints,100.0,false);
    char coordinates[256];


    for (int i=0; i<keypoints.size(); i++)
        {
            cv::circle(imgCopy,keypoints[i].pt,5,cv::Scalar(0,255,0),3,8,0);
            cv::Point jointPoint(keypoints[i].pt.x,keypoints[i].pt.y);
            snprintf(coordinates,256,"%0.1f,%0.1f",keypoints[i].pt.x,keypoints[i].pt.y);
            cv::putText(imgCopy,coordinates, jointPoint, cv::FONT_HERSHEY_DUPLEX,0.4, cv::Scalar(0,255,255), 0.2, cv::LINE_4);
        }


    cv::Point jointPoint(10,10);
    float fontSize = 0.5;
    cv::putText(imgCopy,"Features", jointPoint, cv::FONT_HERSHEY_DUPLEX,fontSize, cv::Scalar(255,255,255), 0.2, cv::LINE_8);
    cv::imshow(windowName,imgCopy);
    
    return 1;
}



int visualizeFigure(const char* windowName,cv::Mat &img)
{
    if ( (img.rows!=0) && (img.cols!=0) )
        {
            cv::imshow(windowName, img);
            return 1;
        }
    return 0;
}

#endif

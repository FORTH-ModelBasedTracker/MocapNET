#include "forthJointDetector.hpp"


#include "../deepjoint/deepjointlib/deepjoint.hpp"
#include <opencv2/opencv.hpp>


Estimator * estimator=0;


int forthJointDetectorStart(const char * model , const char * inputLayer , const char * outputLayer)
{
  //estimator = new Estimator("/home/ammar/Documents/Programming/FORTH/mbv_applications/deepjoint/models/openpose_model.pb", "input_1", "k2tfout_1");
  //estimator = new Estimator("/home/ammar/Documents/Programming/FORTH/mbv_applications/deepjoint/models/vnect_sm_7.6k.pb", "input_1", "k2tfout_0");
  //estimator = new Estimator("/home/ammar/Documents/Programming/FORTH/mbv_applications/deepjoint/models/mobnet2_tiny_vnect_sm_1.9k.pb", "input_1", "k2tfout_0");
  //estimator = new Estimator("/home/ammar/Documents/Programming/FORTH/mbv_applications/deepjoint/models/tiny_multistage_sm.pb", "input_1", "k2tfout_0");

  fprintf(stderr,"FORTH DeepJoint getting initialized will use %s model\n",model);
  fprintf(stderr,"Input Layer: %s\n",inputLayer);
  fprintf(stderr,"Output Layer: %s\n",outputLayer);
  estimator = new Estimator(model,inputLayer,outputLayer);
}


void populateSkeletonStructFromDeepJoint(std::vector<cv::Point> skeletons, struct skeletonCOCO * skel)
{
   for (int i=0; i<skeletons.size(); i++)
        {
          unsigned int jointID = i%18;
          cv::Point jointPoint = skeletons[jointID];

           skel->joint2D[i].x = jointPoint.x;
           skel->joint2D[i].y = jointPoint.y;
           skel->jointAccuracy[i] = 1.0;


           if ( (skel->joint2D[i].x!=skel->joint2D[i].x) || (skel->joint2D[i].y!=skel->joint2D[i].y) )
           {
               //If we have NaN we kill it with fire..
               skel->joint2D[i].x=0.0;
               skel->joint2D[i].y=0.0;
               skel->jointAccuracy[i] = 0.0;
           }
        }

  //Overwrite join
  skel->joint2D[COCO_Nose].x =  ( skel->joint2D[COCO_LEye].x + skel->joint2D[COCO_REye].x ) / 2;
  skel->joint2D[COCO_Nose].y =  ( skel->joint2D[COCO_LEye].y + skel->joint2D[COCO_REye].y ) / 2;

}


std::vector<cv::Point> getNeuralNetworkDetectionsForColorImage(
                                                                unsigned char * colorFrame ,
                                                                unsigned int colorWidth,
                                                                unsigned int colorHeight
                                                              )
{
        cv::Mat colorImageOriginal(
                                   cv::Size(
                                             colorWidth,
                                             colorHeight
                                           ),
                                   CV_8UC3,
                                   (char *) colorFrame ,
                                   cv::Mat::AUTO_STEP
                                  );

        cv::Mat colorImage = colorImageOriginal.clone();

        cv::cvtColor(
                      colorImage,
                      colorImage,
                      CV_RGB2BGR
                    );

        return estimator->predictAndReturnSingleSkeletonOf2DCOCOJoints(
                                                                        colorImage, //Color Image
                                                                        0.6,        //Threshold
                                                                        1           //Visualization
                                                                       );
}




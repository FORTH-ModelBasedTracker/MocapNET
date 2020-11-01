#include "jointEstimator2D.hpp"
#include <vector>
#include <algorithm>    // std::sort
#include <stdio.h>
#include <string.h>
#include <time.h>

unsigned long tickBaseJointEstimator = 0;

unsigned long GetTickCountMicrosecondsJointEstimator()
{
    struct timespec ts;
    if ( clock_gettime(CLOCK_MONOTONIC,&ts) != 0)
    {
        return 0;
    }

    if (tickBaseJointEstimator==0)
    {
        tickBaseJointEstimator = ts.tv_sec*1000000 + ts.tv_nsec/1000;
        return 0;
    }

    return ( ts.tv_sec*1000000 + ts.tv_nsec/1000 ) - tickBaseJointEstimator;
}


int dumpHeatmapToCSVFile(const char * filename,struct JointEstimator2D * jnet,std::vector<float> heatmap)
{
    unsigned int width = jnet->heatmapWidth2DJointDetector;
    unsigned int height = jnet->heatmapHeight2DJointDetector;

    FILE * fp = fopen(filename,"w");
    if (fp!=0)
    {
        //---------------------------
        //          Header
        //---------------------------
        for (int x=0; x<width; x++)
        {
            if (x!=0)
            {
                fprintf(fp,",");
            }
            fprintf(fp,"x=%u",x);
        }
        fprintf(fp,"\n");
        //---------------------------

        //---------------------------
        //            Body
        //---------------------------
        unsigned int i=0;
        for (int y=0; y<height; y++)
        {
            for (int x=0; x<width; x++)
            {
                if (x!=0)
                {
                    fprintf(fp,",");
                }
                if (heatmap[i]>0.01)
                {
                    fprintf(fp,"%f",heatmap[i]);
                }
                else if (-0.01>heatmap[i])
                {
                    fprintf(fp,"%f",heatmap[i]);
                }
                else
                {
                    fprintf(fp,"0");
                }
                ++i;
            }
            fprintf(fp,"\n");
        }
        //---------------------------
        fclose(fp);
        return 1;
    }
    return 0;
}


int loadJointEstimator2D(struct JointEstimator2D * jnet,int joint2DEstimatorSelected,int usePAFs,unsigned int forceCPU)
{
    memset(jnet,0,sizeof(struct JointEstimator2D));

    const char   networkPathOpenPoseMiniStatic[]="dataset/combinedModel/openpose_model.pb";
    const char   networkPathVnectStatic[]="dataset/combinedModel/vnect_sm_pafs_8.1k.pb";
    const char   networkPathFORTHStatic[]="dataset/combinedModel/mobnet2_tiny_vnect_sm_1.9k.pb";
    jnet->networkPath = (char*) networkPathFORTHStatic;
    jnet->joint2DSensitivityPercent=30;

    char   networkInputLayer[]="input_1";
    char   networkOutputLayer[]="k2tfout_0";
    jnet->numberOfOutputTensors = 3;
    jnet->heatmapWidth2DJointDetector= 46;
    jnet->heatmapHeight2DJointDetector= 46;

    jnet->inputWidth2DJointDetector = 368;
    jnet->inputHeight2DJointDetector = 368;
    jnet->numberOfHeatmaps = 19;

    //-----------------------------------------------------------------------------
    switch (joint2DEstimatorSelected)
    {
    case JOINT_2D_ESTIMATOR_FORTH :
        jnet->networkPath=(char*) networkPathFORTHStatic;
        networkOutputLayer[8]='0';
        jnet->joint2DSensitivityPercent=30;
        jnet->numberOfOutputTensors = 3;
        break;
    case JOINT_2D_ESTIMATOR_VNECT :
        jnet->networkPath = (char*) networkPathVnectStatic;
        networkOutputLayer[8]='1';
        jnet->joint2DSensitivityPercent=20;
        jnet->numberOfOutputTensors = 4;
        break;
    case JOINT_2D_ESTIMATOR_OPENPOSE :
        jnet->networkPath=(char*) networkPathOpenPoseMiniStatic;
        networkOutputLayer[8]='1';
        jnet->joint2DSensitivityPercent=40;
        jnet->numberOfOutputTensors = 4;
        break;
    default :
        fprintf(stderr,"No 2D Joint Estimator Selected..\n");
        break;
    };

    //-----------------------------------------------------------------------------
    if (
        loadTensorflowInstance(
            &jnet->network,
            jnet->networkPath,
            networkInputLayer,
            networkOutputLayer,
            forceCPU
        )
    )
    {
        return 1;
    }
    //-----------------------------------------------------------------------------
    return 0;
}



int unloadJointEstimator2D(struct JointEstimator2D * jnet)
{
    return unloadTensorflow(&jnet->network);
}


int sortBlobsBasedOnMaximumActivation(struct HeatmapBlob blobA,struct HeatmapBlob blobB)
{
    return blobA.peakValue > blobB.peakValue;
}



int updateSubpixelPeak(struct JointEstimator2D * jnet,struct HeatmapBlob * blob,std::vector<float> heatmap,unsigned int width,unsigned int height)
{
    //Uncomment to disable subpixel code..
    //blob->subpixelPeakX = (float) blob->peakX;
    //blob->subpixelPeakY = (float) blob->peakY;
    //return 0;

#define DEBUG_SUBPIXEL_GRADIENT 0

#if DEBUG_SUBPIXEL_GRADIENT
    std::vector<float> gradientX(width*height);
    std::vector<float> gradientY(width*height);
#endif


    if (blob->peakValue==0.0)
    {
        return 0;
    }
    float vX=(float) blob->peakX;
    float vY=(float) blob->peakY;
    float normalizedValue=0.0;
    unsigned int location=0;

    for (signed int y=0; y<blob->height; y++)
    {
        for (signed int x=0; x<blob->width; x++)
        {
            //fprintf(stderr,"Blob element (x=%u,y=%u) ",x,y);
            location = (width * (y+blob->y)) + (x+blob->x);
            //fprintf(stderr,"Location %u",location);

            if (location<heatmap.size())
            {
                normalizedValue=(float) heatmap[location] / blob->peakValue;
                normalizedValue= normalizedValue / ( blob->width * blob->height);
                //fprintf(stderr,"NormalValue %0.2f",normalizedValue);

                signed int peakInRelationToBlobX =  (signed int) ((signed int) blob->peakX - (signed int) blob->x);
                signed int peakInRelationToBlobY =  (signed int) ((signed int) blob->peakY - (signed int) blob->y);

                signed int relativePositionToPeakX = (signed int) ( (signed int) x - (signed int) peakInRelationToBlobX);
                signed int relativePositionToPeakY = (signed int) ( (signed int) y - (signed int) peakInRelationToBlobY);
                //fprintf(stderr,"Ppeak %d,%d",posX,posY);



                float nX=(float) relativePositionToPeakX*normalizedValue;
                float nY=(float) relativePositionToPeakY*normalizedValue;
                //fprintf(stderr,"Add %f,%f\n",nX,nY);

#if DEBUG_SUBPIXEL_GRADIENT
                gradientX[location]=nX;
                gradientY[location]=nY;
#endif


                if ( (nX!=nX) || (nY!=nY) )
                {
                    //Handle NaN
                }
                else
                {
                    //fprintf(stderr,"%f,%f ",nX,nY);
                    vX=vX+nX;
                    vY=vY+nY;
                }
            }
            else
            {
                fprintf(stderr,"updateSubpixelPeak overflow..\n");
                return 0;
            }
        }
    }

#if DEBUG_SUBPIXEL_GRADIENT
    char filename[512];
    snprintf(filename,512,"GradientX.csv");
    dumpHeatmapToCSVFile(filename,jnet,gradientX);
    snprintf(filename,512,"GradientY.csv");
    dumpHeatmapToCSVFile(filename,jnet,gradientY);
    snprintf(filename,512,"Values.csv");
    dumpHeatmapToCSVFile(filename,jnet,heatmap);
#endif

    blob->subpixelPeakX = vX;
    blob->subpixelPeakY = vY;
    return 1;
}


int isNeighborWithBlob(char * blobLabels,int x,int y,int width,int height)
{
    for (int iY=-1; iY<1; iY++)
    {
        for (int iX=-1; iX<=1; iX++)
        {
            if ( (iX==0) && (iY==0) )
            {

            }
            else if (
                (iX+x>=0) &&
                (iY+y>=0) &&
                (iX+x<width) &&
                (iY+y<height)
            )
            {
                int location = width * (y+iY) + (x+iX);
                if (blobLabels[location]!=0)
                {
                    return blobLabels[location];
                }
            }
        }
    }

    return 0;
}




std::vector<struct HeatmapBlob>  getBlobsFromHeatmap(struct JointEstimator2D * jnet,unsigned int heatmapID,std::vector<float> heatmap)
{
    unsigned int width = jnet->heatmapWidth2DJointDetector;
    unsigned int height = jnet->heatmapHeight2DJointDetector;

    struct HeatmapBlob emptyBlob;
    emptyBlob.x=width+1;
    emptyBlob.y=height+1;
    emptyBlob.width=0;
    emptyBlob.height=0;
    emptyBlob.width=0;

    emptyBlob.peakX=0;
    emptyBlob.peakY=0;
    emptyBlob.peakValue=0.0;

    emptyBlob.subpixelPeakX=0.0;
    emptyBlob.subpixelPeakY=0.0;

    std::vector<struct HeatmapBlob> blobsEncountered;
    char blobLabels[width*height]= {0};
    unsigned int currentBlobLabel=1;
    int onHorizontalBlob=0;
    int labelID=0;

    unsigned int i=0;
    float threshold = 0.1; //(float) jnet->joint2DSensitivityPercent/100;
    for (int y=0; y<height; y++)
    {
        for (int x=0; x<width; x++)
        {
            if ( heatmap[i] > threshold )
            {

                if (!onHorizontalBlob)
                {
                    //We were not on a horizontal blob so we need to search..!
                    labelID=isNeighborWithBlob(blobLabels,x,y,width,height);
                    if (!labelID)
                    {
                        //Our search did not give us a result
                        //This is a new blob disconnected from previous blobs..!
                        blobLabels[i]=currentBlobLabel;
                        onHorizontalBlob=currentBlobLabel;
                        labelID=currentBlobLabel-1;
                        ++currentBlobLabel;
                        blobsEncountered.push_back(emptyBlob);
                        onHorizontalBlob=1;
                    }
                    else
                    {
                        //Already Existing Blob found we will add this pixel to it
                        onHorizontalBlob=labelID;
                        blobLabels[i]=onHorizontalBlob;
                        labelID=labelID-1;
                    }
                }
                else
                {
                    //If we are already on a horizontal blob dont do expensive search
                    blobLabels[i]=onHorizontalBlob;
                    labelID=onHorizontalBlob-1;
                }


                if (labelID<blobsEncountered.size())
                {
                    //fprintf(stderr,"Hit at %u,%u\n",x,y);
                    //fprintf(stderr,"%f ",heatmap[i]);
                    //Update Blob Start at X Axis
                    if (x<blobsEncountered[labelID].x)
                    {
                        blobsEncountered[labelID].x=x;
                    }

                    //Update Blob Start at Y Axis
                    if (y<blobsEncountered[labelID].y)
                    {
                        blobsEncountered[labelID].y=y;
                    }

                    //Update Blob Width
                    int thisLineWidth=1+x-blobsEncountered[labelID].x;
                    if (thisLineWidth>blobsEncountered[labelID].width)
                    {
                        blobsEncountered[labelID].width=thisLineWidth;
                    }

                    //Update Blob Height
                    int thisLineHeight=1+y-blobsEncountered[labelID].y;
                    if (thisLineHeight>blobsEncountered[labelID].height)
                    {
                        blobsEncountered[labelID].height=thisLineHeight;
                    }

                    //Update Peak to use as center later..
                    if (blobsEncountered[labelID].peakValue<heatmap[i])
                    {
                        blobsEncountered[labelID].peakValue=heatmap[i];
                        blobsEncountered[labelID].peakX=x;
                        blobsEncountered[labelID].peakY=y;
                    }
                }
            }//Above activation threshold..
            else
            {
                //We just left a blob..
                onHorizontalBlob=0;
            }
            ++i;
        } //End of X loop
        //We just left a line so the blob has ended..
        onHorizontalBlob=0;
    } //End of Y loop

    for (int blobID=0; blobID<blobsEncountered.size(); blobID++)
    {
        updateSubpixelPeak(jnet,&blobsEncountered[blobID],heatmap,width,height);
    }

    //Make sure the best value is first..
    std::sort(blobsEncountered.begin(), blobsEncountered.end(),sortBlobsBasedOnMaximumActivation);


    /*
    fprintf(stderr,"Heatmap %u %s--------\n",heatmapID,UT_COCOBodyNames[heatmapID]);
    for (int blobID=0; blobID<blobsEncountered.size(); blobID++)
    {
     fprintf(stderr,"Blob %u/%lu - ",blobID,blobsEncountered.size());
     fprintf(stderr,"x=%u,y=%u(%ux%u) "
     ,blobsEncountered[blobID].x,blobsEncountered[blobID].y
     ,blobsEncountered[blobID].width,blobsEncountered[blobID].height
     );

     fprintf(stderr,"pX=%u,pY=%u>",blobsEncountered[blobID].peakX,blobsEncountered[blobID].peakY);
     fprintf(stderr,"%0.2f",blobsEncountered[blobID].peakValue);

     fprintf(stderr," subpixel(%0.2f,%0.2f)",blobsEncountered[blobID].subpixelPeakX,blobsEncountered[blobID].subpixelPeakY);
     fprintf(stderr,"\n");
    }
    */

    return blobsEncountered;
}




int estimate2DSkeletonsFromHeatmaps(struct JointEstimator2D * jnet,struct Skeletons2DDetected * result,std::vector<std::vector<float> > heatmaps)
{
    //char filename[512];
    //fprintf(stderr,"New Frame:\n");

    unsigned long startTime  = GetTickCountMicrosecondsJointEstimator();
    int blobsProceesed=0;
    for (int heatmapID=0; heatmapID<heatmaps.size(); heatmapID++)
    {
        unsigned int body25ID = heatmapCorrespondenceToBODY25[heatmapID];

        if (heatmapID!=UT_COCO_Bkg)
        {
            result->skeletons[0].body.joint2D[body25ID].x = 0;
            result->skeletons[0].body.joint2D[body25ID].y = 0;
            result->skeletons[0].body.active[body25ID]=0.0;
            //--------------------------------------------------------
            std::vector<struct HeatmapBlob>  blobs = getBlobsFromHeatmap(jnet,heatmapID,heatmaps[heatmapID]);
            if (blobs.size()>0)
            {
                result->skeletons[0].body.isPopulated=1;
                float x=(float) blobs[0].subpixelPeakX/ jnet->heatmapWidth2DJointDetector;
                float y=(float) blobs[0].subpixelPeakY/ jnet->heatmapHeight2DJointDetector;

                result->skeletons[0].body.joint2D[body25ID].x = x;
                result->skeletons[0].body.joint2D[body25ID].y = y;
                if ( (x!=0) && (y!=0) ) {
                    result->skeletons[0].body.active[body25ID]=1.0;
                }

                if (blobsProceesed==0)
                {
                    //Update Bbox Min
                    result->skeletons[0].body.bbox2D[0].x = x;
                    result->skeletons[0].body.bbox2D[0].y = y;
                    //Update Bbox Max
                    result->skeletons[0].body.bbox2D[1].x = x;
                    result->skeletons[0].body.bbox2D[1].y = y;
                }
                else
                {
                    // if Min X > current X
                    if (result->skeletons[0].body.bbox2D[0].x  > x )
                    {
                        result->skeletons[0].body.bbox2D[0].x =x;
                    }
                    // if Min Y > current Y
                    if (result->skeletons[0].body.bbox2D[0].y  > y )
                    {
                        result->skeletons[0].body.bbox2D[0].y =y;
                    }
                    // if Max X < current X
                    if (result->skeletons[0].body.bbox2D[1].x  < x )
                    {
                        result->skeletons[0].body.bbox2D[1].x =x;
                    }
                    // if Max Y < current Y
                    if (result->skeletons[0].body.bbox2D[1].y  < y )
                    {
                        result->skeletons[0].body.bbox2D[1].y =y;
                    }
                }

                ++blobsProceesed;
            }
            //--------------------------------------------------------
            //snprintf(filename,512,"heatmap_%u.csv",heatmapID);
            //dumpHeatmapToCSVFile(filename,jnet,heatmaps[0]);
            //--------------------------------------------------------
        }
    }


    /*
     The following joints  are not covered by the current networks
     BODY25_MidHip,
     BODY25_LBigToe,
     BODY25_LSmallToe,
     BODY25_LHeel,
     BODY25_RBigToe,
     BODY25_RSmallToe,
     BODY25_RHeel,
    */

    //Mid hip can be approximated by LHip and RHip
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (
        (result->skeletons[0].body.joint2D[BODY25_RHip].x!=0) && (result->skeletons[0].body.joint2D[BODY25_LHip].x!=0) &&
        (result->skeletons[0].body.joint2D[BODY25_RHip].y!=0) && (result->skeletons[0].body.joint2D[BODY25_LHip].y!=0)
    )
    {
        result->skeletons[0].body.joint2D[BODY25_MidHip].x = (float) (result->skeletons[0].body.joint2D[BODY25_RHip].x+result->skeletons[0].body.joint2D[BODY25_LHip].x)/2;
        result->skeletons[0].body.joint2D[BODY25_MidHip].y = (float) (result->skeletons[0].body.joint2D[BODY25_RHip].y+result->skeletons[0].body.joint2D[BODY25_LHip].y)/2;
        result->skeletons[0].body.jointAccuracy[BODY25_MidHip] = (result->skeletons[0].body.jointAccuracy[BODY25_RHip]+result->skeletons[0].body.jointAccuracy[BODY25_LHip])/2;
        result->skeletons[0].body.active[BODY25_MidHip] = 1.0;
        //fprintf(stderr,"Mid hip populated and resides @  (%0.2f,%0.2f)\n",result->skeletons[0].body.joint2D[BODY25_MidHip].x,result->skeletons[0].body.joint2D[BODY25_MidHip].y);
    } else
    {
        result->skeletons[0].body.joint2D[BODY25_MidHip].x = 0;
        result->skeletons[0].body.joint2D[BODY25_MidHip].y = 0;
        result->skeletons[0].body.jointAccuracy[BODY25_MidHip] = 0;
        result->skeletons[0].body.active[BODY25_MidHip] = 0.0;
        //fprintf(stderr,"Cannot populate Mid  hip, this will degrade output!\n");
    }
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    result->numberOfSkeletonsDetected=1;

    unsigned long endTime  = GetTickCountMicrosecondsJointEstimator();

// fprintf(stderr,"Resolving joint subpixel peaks took %lu microseconds\n",endTime-startTime);

    return 1;
}




int convertNormalized2DJointsToOriginalImageCoordinates(
    struct JointEstimator2D * jnet,
    float * x,
    float * y,
    int correctOffset
)
{
    //It is very common for points to be empty, they cannot be normalized so its best if we have
    //a quick path for them..
    if  ( (*x==0.0) && (*y==0.0) ) {
        return 1;
    }

    if (*x>1.0) {
        *x=1.0;
    }
    if (*y>1.0) {
        *y=1.0;
    }

    float normalizedX = *x;
    float normalizedY = *y;

    //fprintf(stderr,"Initial point is %0.2f,%0.2f\n",normalizedX,normalizedY);

    float frameOfReferenceOfTensorflowX = normalizedX * jnet->inputWidth2DJointDetector;
    float frameOfReferenceOfTensorflowY = normalizedY * jnet->inputHeight2DJointDetector;

    //fprintf(stderr,"Tensorflow point is %0.2f,%0.2f\n",frameOfReferenceOfTensorflowX,frameOfReferenceOfTensorflowY);

    float scaleFromTensorflowToBoundingBoxX = (float) jnet->crop.croppedDimensionWidth / jnet->inputWidth2DJointDetector;
    float scaleFromTensorflowToBoundingBoxY = (float) jnet->crop.croppedDimensionHeight / jnet->inputHeight2DJointDetector;

    *x = /*jnet->crop.offsetX +*/ (frameOfReferenceOfTensorflowX * scaleFromTensorflowToBoundingBoxX);
    *y = /*jnet->crop.offsetY +*/ (frameOfReferenceOfTensorflowY * scaleFromTensorflowToBoundingBoxY);

    if (correctOffset)
    {
        *x += jnet->crop.offsetX;
        *y += jnet->crop.offsetY;
    }
    //fprintf(stderr,"Final point is %0.2f,%0.2f\n",*x,*y);

    return 1;
}




std::vector<std::vector<float> >  getHeatmaps(struct JointEstimator2D * jnet,unsigned char * rgbData,unsigned int width,unsigned int height)
{
// pass the frame to the Estimator

    unsigned long startTime  = GetTickCountMicrosecondsJointEstimator();
    std::vector<std::vector<float> > joint2DOutput = predictTensorflowOnArrayOfHeatmaps(
                &jnet->network,
                (unsigned int) width,
                (unsigned int) height,
                (float*) rgbData,
                jnet->heatmapWidth2DJointDetector,
                jnet->heatmapHeight2DJointDetector,
                jnet->numberOfOutputTensors
            );
    unsigned long endTime  = GetTickCountMicrosecondsJointEstimator();

//fprintf(stderr,"Running 2D joint estimator took %lu microseconds\n",endTime-startTime);
    return joint2DOutput;
}



int  estimate2DSkeletonsFromImage(struct JointEstimator2D * jnet,struct Skeletons2DDetected * result,unsigned char * rgbData,unsigned int width,unsigned int height)
{
    /*
    unsigned long startTime  = GetTickCountMicroseconds();
    std::vector<cv::Point_<float> > pointsOf2DSkeleton = predictAndReturnSingleSkeletonOf2DCOCOJoints(
                &jnet->network,
                bgr,
                minThreshold,
                visualize,
                saveVisualization,
                frameNumber,
                inputWidth2DJointDetector,
                inputHeight2DJointDetector,
                heatmapWidth2DJointDetector,
                heatmapHeight2DJointDetector,
                numberOfHeatmaps,
                numberOfOutputTensors
            );
    unsigned long endTime = GetTickCountMicroseconds();
    *fps = convertStartEndTimeFromMicrosecondsToFPS(startTime,endTime);
    */



    // pass the frame to the Estimator
    std::vector<std::vector<float> > joint2DOutput = predictTensorflowOnArrayOfHeatmaps(
                &jnet->network,
                (unsigned int) width,
                (unsigned int) height,
                (float*) rgbData,
                jnet->heatmapWidth2DJointDetector,
                jnet->heatmapHeight2DJointDetector,
                jnet->numberOfOutputTensors
            );


    return 0;
}


int restore2DJointsToInputFrameCoordinates(struct JointEstimator2D * jnet,struct Skeletons2DDetected * input)
{
    for (unsigned int skID=0; skID<input->numberOfSkeletonsDetected; skID++)
    {
        for (int i=0; i<BODY25_PARTS; i++)
        {
            //fprintf(stderr,"normalized(%0.2f,%0.2f) ",input->skeletons[skID].body.joint2D[i].x,input->skeletons[skID].body.joint2D[i].y);
            convertNormalized2DJointsToOriginalImageCoordinates(jnet,&input->skeletons[skID].body.joint2D[i].x,&input->skeletons[skID].body.joint2D[i].y,1);
            //fprintf(stderr,"regular(%0.2f,%0.2f) ",input->skeletons[skID].body.joint2D[i].x,input->skeletons[skID].body.joint2D[i].y);
        }

        /* //Currently dont have hand and head networks..!
               for (int i=0; i<COCO_HAND_PARTS; i++)
                   {
                       convertNormalized2DJointsToOriginalImageCoordinates(jnet,&input->skeletons[skID].leftHand.joint2D[i].x,&input->skeletons[skID].leftHand.joint2D[i].y,1);
                   }


               for (int i=0; i<COCO_HAND_PARTS; i++)
                   {
                       convertNormalized2DJointsToOriginalImageCoordinates(jnet,&input->skeletons[skID].rightHand.joint2D[i].x,&input->skeletons[skID].rightHand.joint2D[i].y,1);
                   }

               for (int i=0; i<OP_HEAD_PARTS; i++)
                   {
                       convertNormalized2DJointsToOriginalImageCoordinates(jnet,&input->skeletons[skID].head.joint2D[i].x,&input->skeletons[skID].head.joint2D[i].y,1);
                   }
        */
    }
    return 1;
}



float percentOf2DPointsMissing(struct Skeletons2DDetected * input)
{
    if (input->numberOfSkeletonsDetected==0) {
        return 100.0;
    }

    unsigned int jointsThatExist=0;
    unsigned int jointsThatShouldExist=0;
    if (input->skeletons[0].body.isPopulated)
    {
        jointsThatShouldExist=BODY25_PARTS-2;
        for (int jID=0; jID<BODY25_PARTS; jID++)
        {
            if ( input->skeletons[0].body.active[jID] )
            {
                ++jointsThatExist;
            }
        }

        //fprintf(stderr,"%u/%u joints observed\n",jointsThatExist,jointsThatShouldExist);
        float percentOf2DPointsThatExist = (float) 100 * jointsThatExist / jointsThatShouldExist;
        return (float) 100.0 - percentOf2DPointsThatExist;
    }

    //fprintf(stderr,"Body is not populated\n");
    return 100.0;
}
#include "singleThreaded.hpp"
#include "core.hpp"


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */



std::vector<float> gatherResults(
                                 struct MocapNET2 * mnet,
                                 std::vector<float> resultBody,
                                 std::vector<float> resultUpperBody,
                                 std::vector<float> resultLowerBody,
                                 std::vector<float> resultLeftHand,
                                 std::vector<float> resultRightHand,
                                 std::vector<float> resultFace
                                )
{
    std::vector<float> result;
    //----------------------------------------------------------------------------------------------
    for (int i=0; i<MOCAPNET_OUTPUT_NUMBER; i++)
        {
            result.push_back(0.0);
        }
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    //  This is the place where we gather the various parts of the solution we have retrieved and
    //  gather them to populate the final BVH armature, we only gather from sources that have been
    //  loaded, if a part of the body is not important in this instance it will be considered
    //  to be the default..
    //----------------------------------------------------------------------------------------------
    if (mnet->body.loadedModels>0)
        {
            mocapnetBody_fillResultVector(result,resultBody);
        }

    if (mnet->upperBody.loadedModels>0)
        {
            mocapnetUpperBody_fillResultVector(result,resultUpperBody);
        }

    if (mnet->lowerBody.loadedModels>0)
        {
            mocapnetLowerBody_fillResultVector(result,resultLowerBody);
        }
    //----------------------------------------------------------------------------------------------


    return result;
}




std::vector<float> singleThreadedMocapNET(
                                           struct MocapNET2 * mnet,
                                           struct skeletonSerialized * input,
                                           int doLowerbody,
                                           int doHands,
                                           int doFace,
                                           int doGestureDetection,
                                           unsigned int useInverseKinematics,
                                           int doOutputFiltering
                                         )
{
    std::vector<float> resultBody,resultLowerBody,resultUpperBody,resultLeftHand,resultRightHand,resultFace;
    //---------------------------------------------------------------------------------------------------------------------------------------------
    //resultBody = mocapnetBody_evaluateInput(mnet,input);
    //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
    mocapnetUpperBody_getOrientation(mnet,input);
    resultUpperBody = mocapnetUpperBody_evaluateInput(mnet,input);
    //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
    if ( (doLowerbody) && (mnet->lowerBody.loadedModels>0) )
        {
            resultLowerBody = mocapnetLowerBody_evaluateInput(mnet,input);
        }
    else
        {
            mnet->lowerBody.NSDM.clear();
        }
    //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
    if ( (doHands) && (mnet->leftHand.loadedModels>0) )
        {
            //Not implemented..
            //resultLeftHand = mocapnetLeftHand_evaluateInput(mnet,input);
        }
    else
        {
            mnet->rightHand.NSDM.clear();
        }
    //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
    if ( (doHands) && (mnet->rightHand.loadedModels>0) )
        {
            //Not implemented..
            //resultRightHand = mocapnetRightHand_evaluateInput(mnet,input);
        }
    else
        {
            mnet->leftHand.NSDM.clear();
        }

    if ( (doFace) && (mnet->face.loadedModels>0) )
        {
            //Not implemented..
            //resultFace = mocapnetFace_evaluateInput(mnet,input);
        }
    else
        {
            mnet->face.NSDM.clear();
        }


    //---------------------------------------------------------------------------------------------------------------------------------------------
     std::vector<float>  result = gatherResults(
                                                 mnet,
                                                 resultBody,
                                                 resultUpperBody,
                                                 resultLowerBody,
                                                 resultLeftHand,
                                                 resultRightHand,
                                                 resultFace
                                               );
    //---------------------------------------------------------------------------------------------------------------------------------------------


  return result;
}

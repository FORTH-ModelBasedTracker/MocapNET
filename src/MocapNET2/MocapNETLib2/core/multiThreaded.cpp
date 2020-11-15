#include "multiThreaded.hpp"
#include "singleThreaded.hpp"
#include "core.hpp"

void * mocapNETWorkerThread(void * arg)
{
  //We are a thread so lets retrieve our variables..
  struct threadContext * ptr = (struct threadContext *) arg;
  fprintf(stderr,"MNET Thread-%u: Started..!\n",ptr->threadID);
  struct mocapNETContext * contextArray = (struct mocapNETContext *) ptr->argumentToPass;
  struct mocapNETContext * ctx = &contextArray[ptr->threadID];

  struct MocapNET2 * mnet = ctx->mnet;
  struct skeletonSerialized * input = ctx->input;
  int doLowerbody = ctx->doLowerbody;
  int doHands = ctx->doHands;
  int doFace = ctx->doFace;
  int doGestureDetection = ctx->doGestureDetection;
  unsigned int useInverseKinematics = ctx->useInverseKinematics;
  int doOutputFiltering = ctx->doOutputFiltering;

  std::vector<float> result;

  threadpoolWorkerInitialWait(ptr);

  while (threadpoolWorkerLoopCondition(ptr))
  {
    switch (ptr->threadID)
    {
       case 0:
            result = mocapnetUpperBody_evaluateInput(mnet,input);
       break;
       //----------------------------------------------------------------
       case 1:
         if ( (doLowerbody) && (mnet->lowerBody.loadedModels>0) )
          {
            result = mocapnetLowerBody_evaluateInput(mnet,input);
          }
       break;
       //----------------------------------------------------------------
       case 2:
         if ( (doHands) && (mnet->leftHand.loadedModels>0) )
          {
            //TODO add hands
            //result = mocapnetLeftHand_evaluateInput(mnet,input);
          }
       break;
       //----------------------------------------------------------------
       case 3:
         if ( (doHands) && (mnet->rightHand.loadedModels>0) )
          {
            //TODO add hands
            //result = mocapnetRightHand_evaluateInput(mnet,input);
          }
       break;
    };

    //--------------------------------
    threadpoolWorkerLoopEnd(ptr);
  }

  return 0;
}



std::vector<float> multiThreadedMocapNET(
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
    #if USE_BVH
     if (mnet->options->doMultiThreadedIK)
      {
         struct mocapNETContext ctx[4];

         for (int i=0; i<4; i++)
         {
          ctx[i].mnet=mnet;
          ctx[i].input=input;
          ctx[i].doLowerbody=doLowerbody;
          ctx[i].doHands=doHands;
          ctx[i].doFace=doFace;
          ctx[i].doGestureDetection=doGestureDetection;
          ctx[i].useInverseKinematics=useInverseKinematics;
          ctx[i].doOutputFiltering=doOutputFiltering;
         }

       int okToRunMTCode=0;
       if (!mnet->threadPool.initialized)
            {
                if (
                    threadpoolCreate(
                                      &mnet->threadPool,
                                      4,
                                      (void*) mocapNETWorkerThread,
                                      ctx
                                     )
                   )
                  {
                    fprintf(stderr,"MNET2: Survived threadpool creation \n");
                    nanoSleepT(1000*1000);
                    okToRunMTCode=1;
                  }
            } else
            {
              okToRunMTCode=1;
            }


         if (okToRunMTCode)
         {
          threadpoolMainThreadPrepareWorkForWorkers(&mnet->threadPool);
               mocapnetUpperBody_getOrientation(mnet,input);
          threadpoolMainThreadWaitForWorkersToFinish(&mnet->threadPool);

          std::vector<float> result = gatherResults(
                                                    mnet,
                                                    mnet->body.result,
                                                    mnet->upperBody.result,
                                                    mnet->lowerBody.result,
                                                    mnet->leftHand.result,
                                                    mnet->rightHand.result,
                                                    mnet->face.result
                                                   );
          return result;
         }
      }
    #endif

    //If we have reached this point it means that the multi-threaded code has failed..!
    //Fallback on single-threaded code
    return singleThreadedMocapNET(
                                    mnet,
                                    input,
                                    doLowerbody,
                                    doHands,
                                    doFace,
                                    doGestureDetection,
                                    useInverseKinematics,
                                    doOutputFiltering
                                   );
}

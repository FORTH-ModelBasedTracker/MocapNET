#ifndef MOCAPNET_CONFIGURATION_H_INCLUDED
#define MOCAPNET_CONFIGURATION_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

//Neural network orientations centered around 0
#define NN_ORIENTATIONS_TRAINED_AROUND_ZERO_AND_REQUIRE_TRICK 0

//Also swap bvh rotations before IK step
#define APPLY_BVH_FIX_TO_IK_INPUT 0

//Test swapped 
#define SWAP_LEFT_RIGHT_ENSEMBLES 0

//Hands mode ( 1 / 3 (deprecated) / 5 )
#define HANDS_MODE 1

//Use flip for RHand Regression..!
#define RHAND_FLIP 1


//Limits synced to scripts/createRandomizedDatset.sh
const float FRONT_MIN_ORIENTATION = -45.0;
const float FRONT_MAX_ORIENTATION = 45.0;
//--------------------------------
const float BACK_MIN_ORIENTATION = 135.0;
const float BACK_MAX_ORIENTATION = 225.0;
const float BACK_ALT_MIN_ORIENTATION = -225;
const float BACK_ALT_MAX_ORIENTATION = -135;
//--------------------------------
const float LEFT_MIN_ORIENTATION = -135.0;
const float LEFT_MAX_ORIENTATION = -45.0;
//--------------------------------
const float RIGHT_MIN_ORIENTATION = 45.0;
const float RIGHT_MAX_ORIENTATION = 135.0;
//--------------------------------


#ifdef __cplusplus
}
#endif

#endif // MOCAPNET_CONFIGURATION_H_INCLUDED

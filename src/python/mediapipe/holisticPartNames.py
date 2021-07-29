
#MocapNET list of expected inputs
#frameNumber,skeletonID,totalSkeletons,2DX_head,2DY_head,visible_head,2DX_neck,2DY_neck,visible_neck,2DX_rshoulder,2DY_rshoulder,visible_rshoulder,2DX_relbow,2DY_relbow,visible_relbow,2DX_rhand,2DY_rhand,visible_rhand,2DX_lshoulder,2DY_lshoulder,visible_lshoulder,2DX_lelbow,2DY_lelbow,visible_lelbow,2DX_lhand,2DY_lhand,visible_lhand,2DX_hip,2DY_hip,visible_hip,2DX_rhip,2DY_rhip,visible_rhip,2DX_rknee,2DY_rknee,visible_rknee,2DX_rfoot,2DY_rfoot,visible_rfoot,2DX_lhip,2DY_lhip,visible_lhip,2DX_lknee,2DY_lknee,visible_lknee,2DX_lfoot,2DY_lfoot,visible_lfoot,2DX_endsite_eye.r,2DY_endsite_eye.r,visible_endsite_eye.r,2DX_endsite_eye.l,2DY_endsite_eye.l,visible_endsite_eye.l,2DX_rear,2DY_rear,visible_rear,2DX_lear,2DY_lear,visible_lear,2DX_endsite_toe1-2.l,2DY_endsite_toe1-2.l,visible_endsite_toe1-2.l,2DX_endsite_toe5-3.l,2DY_endsite_toe5-3.l,visible_endsite_toe5-3.l,2DX_lheel,2DY_lheel,visible_lheel,2DX_endsite_toe1-2.r,2DY_endsite_toe1-2.r,visible_endsite_toe1-2.r,2DX_endsite_toe5-3.r,2DY_endsite_toe5-3.r,visible_endsite_toe5-3.r,2DX_rheel,2DY_rheel,visible_rheel,2DX_bkg,2DY_bkg,visible_bkg,


def getHolisticBodyNameList():
 bn=list()
 #---------------------------------------------------
 bn.append("head")              #0  - nose
 bn.append("head_leye_0")       #1  - left_eye_inner
 bn.append("endsite_eye.l")     #2  - left_eye
 bn.append("head_leye_3")       #3  - left_eye_outer
 bn.append("head_reye_3")       #4  - right_eye_inner
 bn.append("endsite_eye.r")     #5  - right_eye
 bn.append("head_reye_0")       #6  - right_eye_outer
 bn.append("lear")              #7  - left_ear
 bn.append("rear")              #8  - right_ear
 bn.append("head_outmouth_0")   #9  - mouth_left
 bn.append("head_outmouth_6")   #10 - mouth_right 
 bn.append("lshoulder")         #11 - left_shoulder
 bn.append("rshoulder")         #12 - right_shoulder
 bn.append("lelbow")            #13 - left_elbow
 bn.append("relbow")            #14 - right_elbow
 bn.append("lhand")             #15 - left_wrist
 bn.append("rhand")             #16 - right_wrist
 bn.append("left_hand_pinky_4") #17 - left_pinky
 bn.append("right_hand_pinky_4")#18 - right_pinky
 bn.append("left_hand_index_4") #19 - left_index
 bn.append("right_hand_index_4")#20 - right_index
 bn.append("left_hand_thumb_4") #21 - left_thumb
 bn.append("right_hand_thumb_4")#22 - right_thumb
 bn.append("lhip")              #23 - left_hip
 bn.append("rhip")              #24 - right_hip
 bn.append("lknee")             #25 - left_knee
 bn.append("rknee")             #26 - right_knee
 bn.append("lfoot")             #27 - left_ankle
 bn.append("rfoot")             #28 - right_ankle
 bn.append("lheel")             #29 - left_heel
 bn.append("rheel")             #30 - right_heel
 bn.append("endsite_toe1-2.l")  #31 - left_foot_index
 bn.append("endsite_toe1-2.r")  #32 - right_foot_index
 return bn
#---------------------------------------------------




#2DX_lhand,2DY_lhand,visible_lhand,2DX_lthumb,2DY_lthumb,visible_lthumb,2DX_finger1-2.l,2DY_finger1-2.l,visible_finger1-2.l,2DX_finger1-3.l,2DY_finger1-3.l,visible_finger1-3.l,2DX_endsite_finger1-3.l,2DY_endsite_finger1-3.l,visible_endsite_finger1-3.l,2DX_finger2-1.l,2DY_finger2-1.l,visible_finger2-1.l,2DX_finger2-2.l,2DY_finger2-2.l,visible_finger2-2.l,2DX_finger2-3.l,2DY_finger2-3.l,visible_finger2-3.l,2DX_endsite_finger2-3.l,2DY_endsite_finger2-3.l,visible_endsite_finger2-3.l,2DX_finger3-1.l,2DY_finger3-1.l,visible_finger3-1.l,2DX_finger3-2.l,2DY_finger3-2.l,visible_finger3-2.l,2DX_finger3-3.l,2DY_finger3-3.l,visible_finger3-3.l,2DX_endsite_finger3-3.l,2DY_endsite_finger3-3.l,visible_endsite_finger3-3.l,2DX_finger4-1.l,2DY_finger4-1.l,visible_finger4-1.l,2DX_finger4-2.l,2DY_finger4-2.l,visible_finger4-2.l,2DX_finger4-3.l,2DY_finger4-3.l,visible_finger4-3.l,2DX_endsite_finger4-3.l,2DY_endsite_finger4-3.l,visible_endsite_finger4-3.l,2DX_finger5-1.l,2DY_finger5-1.l,visible_finger5-1.l,2DX_finger5-2.l,2DY_finger5-2.l,visible_finger5-2.l,2DX_finger5-3.l,2DY_finger5-3.l,visible_finger5-3.l,2DX_endsite_finger5-3.l,2DY_endsite_finger5-3.l,visible_endsite_finger5-3.l
def getHolisticLHandNameList():
 bn=list()
 #---------------------------------------------------
 bn.append("lhand")                    #0   - wrist
 bn.append("lthumb")                   #1   - thumb_cmc
 bn.append("finger1-2.l")              #2   - thumb_mcp
 bn.append("finger1-3.l")              #3   - thumb_ip
 bn.append("endsite_finger1-3.l")      #4   - thumb_tip
 bn.append("finger2-1.l")              #5   - index_finger_mcp
 bn.append("finger2-2.l")              #6   - index_finger_pip
 bn.append("finger2-3.l")              #7   - index_finger_dip
 bn.append("endsite_finger2-3.l")      #8   - index_finger_tip
 bn.append("finger3-1.l")              #9   - middle_finger_mcp
 bn.append("finger3-2.l")              #10  - middle_finger_pip
 bn.append("finger3-3.l")              #11  - middle_finger_dip
 bn.append("endsite_finger3-3.l")      #12  - middle_finger_tip
 bn.append("finger4-1.l")              #13  - ring_finger_mcp
 bn.append("finger4-2.l")              #14  - ring_finger_pip
 bn.append("finger4-3.l")              #15  - ring_finger_dip
 bn.append("endsite_finger4-3.l")      #16  - ring_tip
 bn.append("finger5-1.l")              #17  - pinky_mcp
 bn.append("finger5-2.l")              #18  - pinky_pip
 bn.append("finger5-3.l")              #19  - pinky_dip
 bn.append("endsite_finger5-3.l")      #20  - pinky_tip
 return bn
#---------------------------------------------------




#2DX_rhand,2DY_rhand,visible_rhand,2DX_rthumb,2DY_rthumb,visible_rthumb,2DX_finger1-2.r,2DY_finger1-2.r,visible_finger1-2.r,2DX_finger1-3.r,2DY_finger1-3.r,visible_finger1-3.r,2DX_endsite_finger1-3.r,2DY_endsite_finger1-3.r,visible_endsite_finger1-3.r,2DX_finger2-1.r,2DY_finger2-1.r,visible_finger2-1.r,2DX_finger2-2.r,2DY_finger2-2.r,visible_finger2-2.r,2DX_finger2-3.r,2DY_finger2-3.r,visible_finger2-3.r,2DX_endsite_finger2-3.r,2DY_endsite_finger2-3.r,visible_endsite_finger2-3.r,2DX_finger3-1.r,2DY_finger3-1.r,visible_finger3-1.r,2DX_finger3-2.r,2DY_finger3-2.r,visible_finger3-2.r,2DX_finger3-3.r,2DY_finger3-3.r,visible_finger3-3.r,2DX_endsite_finger3-3.r,2DY_endsite_finger3-3.r,visible_endsite_finger3-3.r,2DX_finger4-1.r,2DY_finger4-1.r,visible_finger4-1.r,2DX_finger4-2.r,2DY_finger4-2.r,visible_finger4-2.r,2DX_finger4-3.r,2DY_finger4-3.r,visible_finger4-3.r,2DX_endsite_finger4-3.r,2DY_endsite_finger4-3.r,visible_endsite_finger4-3.r,2DX_finger5-1.r,2DY_finger5-1.r,visible_finger5-1.r,2DX_finger5-2.r,2DY_finger5-2.r,visible_finger5-2.r,2DX_finger5-3.r,2DY_finger5-3.r,visible_finger5-3.r,2DX_endsite_finger5-3.r,2DY_endsite_finger5-3.r,visible_endsite_finger5-3.r
def getHolisticRHandNameList():
 bn=list()
 #---------------------------------------------------
 bn.append("rhand")                    #0   - wrist
 bn.append("rthumb")                   #1   - thumb_cmc
 bn.append("finger1-2.r")              #2   - thumb_mcp
 bn.append("finger1-3.r")              #3   - thumb_ip
 bn.append("endsite_finger1-3.r")      #4   - thumb_tip
 bn.append("finger2-1.r")              #5   - index_finger_mcp
 bn.append("finger2-2.r")              #6   - index_finger_pip
 bn.append("finger2-3.r")              #7   - index_finger_dip
 bn.append("endsite_finger2-3.r")      #8   - index_finger_tip
 bn.append("finger3-1.r")              #9   - middle_finger_mcp
 bn.append("finger3-2.r")              #10  - middle_finger_pip
 bn.append("finger3-3.r")              #11  - middle_finger_dip
 bn.append("endsite_finger3-3.r")      #12  - middle_finger_tip
 bn.append("finger4-1.r")              #13  - ring_finger_mcp
 bn.append("finger4-2.r")              #14  - ring_finger_pip
 bn.append("finger4-3.r")              #15  - ring_finger_dip
 bn.append("endsite_finger4-3.r")      #16  - ring_tip
 bn.append("finger5-1.r")              #17  - pinky_mcp
 bn.append("finger5-2.r")              #18  - pinky_pip
 bn.append("finger5-3.r")              #19  - pinky_dip
 bn.append("endsite_finger5-3.r")      #20  - pinky_tip
 return bn
#---------------------------------------------------



#2DX_head_rchin_0,2DY_head_rchin_0,visible_head_rchin_0,2DX_head_rchin_1,2DY_head_rchin_1,visible_head_rchin_1,2DX_head_rchin_2,2DY_head_rchin_2,visible_head_rchin_2,2DX_head_rchin_3,2DY_head_rchin_3,visible_head_rchin_3,2DX_head_rchin_4,2DY_head_rchin_4,visible_head_rchin_4,2DX_head_rchin_5,2DY_head_rchin_5,visible_head_rchin_5,2DX_head_rchin_6,2DY_head_rchin_6,visible_head_rchin_6,2DX_head_rchin_7,2DY_head_rchin_7,visible_head_rchin_7,2DX_head_chin,2DY_head_chin,visible_head_chin,2DX_head_lchin_7,2DY_head_lchin_7,visible_head_lchin_7,2DX_head_lchin_6,2DY_head_lchin_6,visible_head_lchin_6,2DX_head_lchin_5,2DY_head_lchin_5,visible_head_lchin_5,2DX_head_lchin_4,2DY_head_lchin_4,visible_head_lchin_4,2DX_head_lchin_3,2DY_head_lchin_3,visible_head_lchin_3,2DX_head_lchin_2,2DY_head_lchin_2,visible_head_lchin_2,2DX_head_lchin_1,2DY_head_lchin_1,visible_head_lchin_1,2DX_head_lchin_0,2DY_head_lchin_0,visible_head_lchin_0,2DX_head_reyebrow_0,2DY_head_reyebrow_0,visible_head_reyebrow_0,2DX_head_reyebrow_1,2DY_head_reyebrow_1,visible_head_reyebrow_1,2DX_head_reyebrow_2,2DY_head_reyebrow_2,visible_head_reyebrow_2,2DX_head_reyebrow_3,2DY_head_reyebrow_3,visible_head_reyebrow_3,2DX_head_reyebrow_4,2DY_head_reyebrow_4,visible_head_reyebrow_4,2DX_head_leyebrow_4,2DY_head_leyebrow_4,visible_head_leyebrow_4,2DX_head_leyebrow_3,2DY_head_leyebrow_3,visible_head_leyebrow_3,2DX_head_leyebrow_2,2DY_head_leyebrow_2,visible_head_leyebrow_2,2DX_head_leyebrow_1,2DY_head_leyebrow_1,visible_head_leyebrow_1,2DX_head_leyebrow_0,2DY_head_leyebrow_0,visible_head_leyebrow_0,2DX_head_nosebone_0,2DY_head_nosebone_0,visible_head_nosebone_0,2DX_head_nosebone_1,2DY_head_nosebone_1,visible_head_nosebone_1,2DX_head_nosebone_2,2DY_head_nosebone_2,visible_head_nosebone_2,2DX_head_nosebone_3,2DY_head_nosebone_3,visible_head_nosebone_3,2DX_head_nostrills_0,2DY_head_nostrills_0,visible_head_nostrills_0,2DX_head_nostrills_1,2DY_head_nostrills_1,visible_head_nostrills_1,2DX_head_nostrills_2,2DY_head_nostrills_2,visible_head_nostrills_2,2DX_head_nostrills_3,2DY_head_nostrills_3,visible_head_nostrills_3,2DX_head_nostrills_4,2DY_head_nostrills_4,visible_head_nostrills_4,2DX_head_reye_0,2DY_head_reye_0,visible_head_reye_0,2DX_head_reye_1,2DY_head_reye_1,visible_head_reye_1,2DX_head_reye_2,2DY_head_reye_2,visible_head_reye_2,2DX_head_reye_3,2DY_head_reye_3,visible_head_reye_3,2DX_head_reye_4,2DY_head_reye_4,visible_head_reye_4,2DX_head_reye_5,2DY_head_reye_5,visible_head_reye_5,2DX_head_leye_0,2DY_head_leye_0,visible_head_leye_0,2DX_head_leye_1,2DY_head_leye_1,visible_head_leye_1,2DX_head_leye_2,2DY_head_leye_2,visible_head_leye_2,2DX_head_leye_3,2DY_head_leye_3,visible_head_leye_3,2DX_head_leye_4,2DY_head_leye_4,visible_head_leye_4,2DX_head_leye_5,2DY_head_leye_5,visible_head_leye_5,2DX_head_outmouth_0,2DY_head_outmouth_0,visible_head_outmouth_0,2DX_head_outmouth_1,2DY_head_outmouth_1,visible_head_outmouth_1,2DX_head_outmouth_2,2DY_head_outmouth_2,visible_head_outmouth_2,2DX_head_outmouth_3,2DY_head_outmouth_3,visible_head_outmouth_3,2DX_head_outmouth_4,2DY_head_outmouth_4,visible_head_outmouth_4,2DX_head_outmouth_5,2DY_head_outmouth_5,visible_head_outmouth_5,2DX_head_outmouth_6,2DY_head_outmouth_6,visible_head_outmouth_6,2DX_head_outmouth_7,2DY_head_outmouth_7,visible_head_outmouth_7,2DX_head_outmouth_8,2DY_head_outmouth_8,visible_head_outmouth_8,2DX_head_outmouth_9,2DY_head_outmouth_9,visible_head_outmouth_9,2DX_head_outmouth_10,2DY_head_outmouth_10,visible_head_outmouth_10,2DX_head_outmouth_11,2DY_head_outmouth_11,visible_head_outmouth_11,2DX_head_inmouth_0,2DY_head_inmouth_0,visible_head_inmouth_0,2DX_head_inmouth_1,2DY_head_inmouth_1,visible_head_inmouth_1,2DX_head_inmouth_2,2DY_head_inmouth_2,visible_head_inmouth_2,2DX_head_inmouth_3,2DY_head_inmouth_3,visible_head_inmouth_3,2DX_head_inmouth_4,2DY_head_inmouth_4,visible_head_inmouth_4,2DX_head_inmouth_5,2DY_head_inmouth_5,visible_head_inmouth_5,2DX_head_inmouth_6,2DY_head_inmouth_6,visible_head_inmouth_6,2DX_head_inmouth_7,2DY_head_inmouth_7,visible_head_inmouth_7,2DX_head_reye,2DY_head_reye,visible_head_reye,2DX_head_leye,2DY_head_leye,visible_head_leye









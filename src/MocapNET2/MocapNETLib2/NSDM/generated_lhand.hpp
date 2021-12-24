/** @file generated_lhand.hpp
 *  @brief A Description the lhand input of a Tensorflow network required for MocapNET 
 *  @author Ammar Qammaz (AmmarkoV)
 *  Automatically generated @ 2021-03-02 15:23:42.772981 using : 
 *     python3 exportCPPCodeFromJSONConfiguration.py --front lhand --config dataset/lhand_configuration.json
 *     please note that since the names of the labels are both affected by the dataset/lhand_configuration.json configuration 
 *     as well as the ground truth, if you have made any weird additions you might consider running the ./createRandomizedDataset.sh and ./createTestDataset.sh scripts 
 */

#pragma once

#include <iostream>
#include <math.h>
#include <vector>
#include "../tools.hpp"

/** @brief This is an array of names for all uncompressed 2D inputs expected. */
static const unsigned int mocapNET_InputLength_WithoutNSDM_lhand = 63; 

/** @brief Use rich diagonal, part of networks after 31-01-2021 */
static const unsigned int richDiagonal_lhand = 1; 

/** @brief An array of strings that contains the label for each expected input. */
static const char * mocapNET_lhand[] = 
{
 "2DX_lhand", //0 
 "2DY_lhand", //1 
 "visible_lhand", //2 
 "2DX_finger2-1.l", //3 
 "2DY_finger2-1.l", //4 
 "visible_finger2-1.l", //5 
 "2DX_finger2-2.l", //6 
 "2DY_finger2-2.l", //7 
 "visible_finger2-2.l", //8 
 "2DX_finger2-3.l", //9 
 "2DY_finger2-3.l", //10 
 "visible_finger2-3.l", //11 
 "2DX_EndSite_finger2-3.l", //12 
 "2DY_EndSite_finger2-3.l", //13 
 "visible_EndSite_finger2-3.l", //14 
 "2DX_finger3-1.l", //15 
 "2DY_finger3-1.l", //16 
 "visible_finger3-1.l", //17 
 "2DX_finger3-2.l", //18 
 "2DY_finger3-2.l", //19 
 "visible_finger3-2.l", //20 
 "2DX_finger3-3.l", //21 
 "2DY_finger3-3.l", //22 
 "visible_finger3-3.l", //23 
 "2DX_EndSite_finger3-3.l", //24 
 "2DY_EndSite_finger3-3.l", //25 
 "visible_EndSite_finger3-3.l", //26 
 "2DX_finger4-1.l", //27 
 "2DY_finger4-1.l", //28 
 "visible_finger4-1.l", //29 
 "2DX_finger4-2.l", //30 
 "2DY_finger4-2.l", //31 
 "visible_finger4-2.l", //32 
 "2DX_finger4-3.l", //33 
 "2DY_finger4-3.l", //34 
 "visible_finger4-3.l", //35 
 "2DX_EndSite_finger4-3.l", //36 
 "2DY_EndSite_finger4-3.l", //37 
 "visible_EndSite_finger4-3.l", //38 
 "2DX_finger5-1.l", //39 
 "2DY_finger5-1.l", //40 
 "visible_finger5-1.l", //41 
 "2DX_finger5-2.l", //42 
 "2DY_finger5-2.l", //43 
 "visible_finger5-2.l", //44 
 "2DX_finger5-3.l", //45 
 "2DY_finger5-3.l", //46 
 "visible_finger5-3.l", //47 
 "2DX_EndSite_finger5-3.l", //48 
 "2DY_EndSite_finger5-3.l", //49 
 "visible_EndSite_finger5-3.l", //50 
 "2DX_lthumb", //51 
 "2DY_lthumb", //52 
 "visible_lthumb", //53 
 "2DX_finger1-2.l", //54 
 "2DY_finger1-2.l", //55 
 "visible_finger1-2.l", //56 
 "2DX_finger1-3.l", //57 
 "2DY_finger1-3.l", //58 
 "visible_finger1-3.l", //59 
 "2DX_EndSite_finger1-3.l", //60 
 "2DY_EndSite_finger1-3.l", //61 
 "visible_EndSite_finger1-3.l", //62 
//This is where regular input ends and the NSDM data kicks in..
 "angleUsedFor2DRotation_0", //63 
 "lhandY-finger5-1.lY-Angle", //64 
 "lhandY-finger5-2.lY-Angle", //65 
 "lhandY-finger5-3.lY-Angle", //66 
 "lhandY-finger4-1.lY-Angle", //67 
 "lhandY-finger4-2.lY-Angle", //68 
 "lhandY-finger4-3.lY-Angle", //69 
 "lhandY-finger3-1.lY-Angle", //70 
 "lhandY-finger3-2.lY-Angle", //71 
 "lhandY-finger3-3.lY-Angle", //72 
 "lhandY-finger2-1.lY-Angle", //73 
 "lhandY-finger2-2.lY-Angle", //74 
 "lhandY-finger2-3.lY-Angle", //75 
 "lhandY-lthumbY-Angle", //76 
 "lhandY-finger1-2.lY-Angle", //77 
 "lhandY-finger1-3.lY-Angle", //78 
 "finger5-1.lY-lhandY-Angle", //79 
 "angleUsedFor2DRotation_1", //80 
 "finger5-1.lY-finger5-2.lY-Angle", //81 
 "finger5-1.lY-finger5-3.lY-Angle", //82 
 "finger5-1.lY-finger4-1.lY-Angle", //83 
 "finger5-1.lY-finger4-2.lY-Angle", //84 
 "finger5-1.lY-finger4-3.lY-Angle", //85 
 "finger5-1.lY-finger3-1.lY-Angle", //86 
 "finger5-1.lY-finger3-2.lY-Angle", //87 
 "finger5-1.lY-finger3-3.lY-Angle", //88 
 "finger5-1.lY-finger2-1.lY-Angle", //89 
 "finger5-1.lY-finger2-2.lY-Angle", //90 
 "finger5-1.lY-finger2-3.lY-Angle", //91 
 "finger5-1.lY-lthumbY-Angle", //92 
 "finger5-1.lY-finger1-2.lY-Angle", //93 
 "finger5-1.lY-finger1-3.lY-Angle", //94 
 "finger5-2.lY-lhandY-Angle", //95 
 "finger5-2.lY-finger5-1.lY-Angle", //96 
 "angleUsedFor2DRotation_2", //97 
 "finger5-2.lY-finger5-3.lY-Angle", //98 
 "finger5-2.lY-finger4-1.lY-Angle", //99 
 "finger5-2.lY-finger4-2.lY-Angle", //100 
 "finger5-2.lY-finger4-3.lY-Angle", //101 
 "finger5-2.lY-finger3-1.lY-Angle", //102 
 "finger5-2.lY-finger3-2.lY-Angle", //103 
 "finger5-2.lY-finger3-3.lY-Angle", //104 
 "finger5-2.lY-finger2-1.lY-Angle", //105 
 "finger5-2.lY-finger2-2.lY-Angle", //106 
 "finger5-2.lY-finger2-3.lY-Angle", //107 
 "finger5-2.lY-lthumbY-Angle", //108 
 "finger5-2.lY-finger1-2.lY-Angle", //109 
 "finger5-2.lY-finger1-3.lY-Angle", //110 
 "finger5-3.lY-lhandY-Angle", //111 
 "finger5-3.lY-finger5-1.lY-Angle", //112 
 "finger5-3.lY-finger5-2.lY-Angle", //113 
 "angleUsedFor2DRotation_3", //114 
 "finger5-3.lY-finger4-1.lY-Angle", //115 
 "finger5-3.lY-finger4-2.lY-Angle", //116 
 "finger5-3.lY-finger4-3.lY-Angle", //117 
 "finger5-3.lY-finger3-1.lY-Angle", //118 
 "finger5-3.lY-finger3-2.lY-Angle", //119 
 "finger5-3.lY-finger3-3.lY-Angle", //120 
 "finger5-3.lY-finger2-1.lY-Angle", //121 
 "finger5-3.lY-finger2-2.lY-Angle", //122 
 "finger5-3.lY-finger2-3.lY-Angle", //123 
 "finger5-3.lY-lthumbY-Angle", //124 
 "finger5-3.lY-finger1-2.lY-Angle", //125 
 "finger5-3.lY-finger1-3.lY-Angle", //126 
 "finger4-1.lY-lhandY-Angle", //127 
 "finger4-1.lY-finger5-1.lY-Angle", //128 
 "finger4-1.lY-finger5-2.lY-Angle", //129 
 "finger4-1.lY-finger5-3.lY-Angle", //130 
 "angleUsedFor2DRotation_4", //131 
 "finger4-1.lY-finger4-2.lY-Angle", //132 
 "finger4-1.lY-finger4-3.lY-Angle", //133 
 "finger4-1.lY-finger3-1.lY-Angle", //134 
 "finger4-1.lY-finger3-2.lY-Angle", //135 
 "finger4-1.lY-finger3-3.lY-Angle", //136 
 "finger4-1.lY-finger2-1.lY-Angle", //137 
 "finger4-1.lY-finger2-2.lY-Angle", //138 
 "finger4-1.lY-finger2-3.lY-Angle", //139 
 "finger4-1.lY-lthumbY-Angle", //140 
 "finger4-1.lY-finger1-2.lY-Angle", //141 
 "finger4-1.lY-finger1-3.lY-Angle", //142 
 "finger4-2.lY-lhandY-Angle", //143 
 "finger4-2.lY-finger5-1.lY-Angle", //144 
 "finger4-2.lY-finger5-2.lY-Angle", //145 
 "finger4-2.lY-finger5-3.lY-Angle", //146 
 "finger4-2.lY-finger4-1.lY-Angle", //147 
 "angleUsedFor2DRotation_5", //148 
 "finger4-2.lY-finger4-3.lY-Angle", //149 
 "finger4-2.lY-finger3-1.lY-Angle", //150 
 "finger4-2.lY-finger3-2.lY-Angle", //151 
 "finger4-2.lY-finger3-3.lY-Angle", //152 
 "finger4-2.lY-finger2-1.lY-Angle", //153 
 "finger4-2.lY-finger2-2.lY-Angle", //154 
 "finger4-2.lY-finger2-3.lY-Angle", //155 
 "finger4-2.lY-lthumbY-Angle", //156 
 "finger4-2.lY-finger1-2.lY-Angle", //157 
 "finger4-2.lY-finger1-3.lY-Angle", //158 
 "finger4-3.lY-lhandY-Angle", //159 
 "finger4-3.lY-finger5-1.lY-Angle", //160 
 "finger4-3.lY-finger5-2.lY-Angle", //161 
 "finger4-3.lY-finger5-3.lY-Angle", //162 
 "finger4-3.lY-finger4-1.lY-Angle", //163 
 "finger4-3.lY-finger4-2.lY-Angle", //164 
 "angleUsedFor2DRotation_6", //165 
 "finger4-3.lY-finger3-1.lY-Angle", //166 
 "finger4-3.lY-finger3-2.lY-Angle", //167 
 "finger4-3.lY-finger3-3.lY-Angle", //168 
 "finger4-3.lY-finger2-1.lY-Angle", //169 
 "finger4-3.lY-finger2-2.lY-Angle", //170 
 "finger4-3.lY-finger2-3.lY-Angle", //171 
 "finger4-3.lY-lthumbY-Angle", //172 
 "finger4-3.lY-finger1-2.lY-Angle", //173 
 "finger4-3.lY-finger1-3.lY-Angle", //174 
 "finger3-1.lY-lhandY-Angle", //175 
 "finger3-1.lY-finger5-1.lY-Angle", //176 
 "finger3-1.lY-finger5-2.lY-Angle", //177 
 "finger3-1.lY-finger5-3.lY-Angle", //178 
 "finger3-1.lY-finger4-1.lY-Angle", //179 
 "finger3-1.lY-finger4-2.lY-Angle", //180 
 "finger3-1.lY-finger4-3.lY-Angle", //181 
 "angleUsedFor2DRotation_7", //182 
 "finger3-1.lY-finger3-2.lY-Angle", //183 
 "finger3-1.lY-finger3-3.lY-Angle", //184 
 "finger3-1.lY-finger2-1.lY-Angle", //185 
 "finger3-1.lY-finger2-2.lY-Angle", //186 
 "finger3-1.lY-finger2-3.lY-Angle", //187 
 "finger3-1.lY-lthumbY-Angle", //188 
 "finger3-1.lY-finger1-2.lY-Angle", //189 
 "finger3-1.lY-finger1-3.lY-Angle", //190 
 "finger3-2.lY-lhandY-Angle", //191 
 "finger3-2.lY-finger5-1.lY-Angle", //192 
 "finger3-2.lY-finger5-2.lY-Angle", //193 
 "finger3-2.lY-finger5-3.lY-Angle", //194 
 "finger3-2.lY-finger4-1.lY-Angle", //195 
 "finger3-2.lY-finger4-2.lY-Angle", //196 
 "finger3-2.lY-finger4-3.lY-Angle", //197 
 "finger3-2.lY-finger3-1.lY-Angle", //198 
 "angleUsedFor2DRotation_8", //199 
 "finger3-2.lY-finger3-3.lY-Angle", //200 
 "finger3-2.lY-finger2-1.lY-Angle", //201 
 "finger3-2.lY-finger2-2.lY-Angle", //202 
 "finger3-2.lY-finger2-3.lY-Angle", //203 
 "finger3-2.lY-lthumbY-Angle", //204 
 "finger3-2.lY-finger1-2.lY-Angle", //205 
 "finger3-2.lY-finger1-3.lY-Angle", //206 
 "finger3-3.lY-lhandY-Angle", //207 
 "finger3-3.lY-finger5-1.lY-Angle", //208 
 "finger3-3.lY-finger5-2.lY-Angle", //209 
 "finger3-3.lY-finger5-3.lY-Angle", //210 
 "finger3-3.lY-finger4-1.lY-Angle", //211 
 "finger3-3.lY-finger4-2.lY-Angle", //212 
 "finger3-3.lY-finger4-3.lY-Angle", //213 
 "finger3-3.lY-finger3-1.lY-Angle", //214 
 "finger3-3.lY-finger3-2.lY-Angle", //215 
 "angleUsedFor2DRotation_9", //216 
 "finger3-3.lY-finger2-1.lY-Angle", //217 
 "finger3-3.lY-finger2-2.lY-Angle", //218 
 "finger3-3.lY-finger2-3.lY-Angle", //219 
 "finger3-3.lY-lthumbY-Angle", //220 
 "finger3-3.lY-finger1-2.lY-Angle", //221 
 "finger3-3.lY-finger1-3.lY-Angle", //222 
 "finger2-1.lY-lhandY-Angle", //223 
 "finger2-1.lY-finger5-1.lY-Angle", //224 
 "finger2-1.lY-finger5-2.lY-Angle", //225 
 "finger2-1.lY-finger5-3.lY-Angle", //226 
 "finger2-1.lY-finger4-1.lY-Angle", //227 
 "finger2-1.lY-finger4-2.lY-Angle", //228 
 "finger2-1.lY-finger4-3.lY-Angle", //229 
 "finger2-1.lY-finger3-1.lY-Angle", //230 
 "finger2-1.lY-finger3-2.lY-Angle", //231 
 "finger2-1.lY-finger3-3.lY-Angle", //232 
 "angleUsedFor2DRotation_10", //233 
 "finger2-1.lY-finger2-2.lY-Angle", //234 
 "finger2-1.lY-finger2-3.lY-Angle", //235 
 "finger2-1.lY-lthumbY-Angle", //236 
 "finger2-1.lY-finger1-2.lY-Angle", //237 
 "finger2-1.lY-finger1-3.lY-Angle", //238 
 "finger2-2.lY-lhandY-Angle", //239 
 "finger2-2.lY-finger5-1.lY-Angle", //240 
 "finger2-2.lY-finger5-2.lY-Angle", //241 
 "finger2-2.lY-finger5-3.lY-Angle", //242 
 "finger2-2.lY-finger4-1.lY-Angle", //243 
 "finger2-2.lY-finger4-2.lY-Angle", //244 
 "finger2-2.lY-finger4-3.lY-Angle", //245 
 "finger2-2.lY-finger3-1.lY-Angle", //246 
 "finger2-2.lY-finger3-2.lY-Angle", //247 
 "finger2-2.lY-finger3-3.lY-Angle", //248 
 "finger2-2.lY-finger2-1.lY-Angle", //249 
 "angleUsedFor2DRotation_11", //250 
 "finger2-2.lY-finger2-3.lY-Angle", //251 
 "finger2-2.lY-lthumbY-Angle", //252 
 "finger2-2.lY-finger1-2.lY-Angle", //253 
 "finger2-2.lY-finger1-3.lY-Angle", //254 
 "finger2-3.lY-lhandY-Angle", //255 
 "finger2-3.lY-finger5-1.lY-Angle", //256 
 "finger2-3.lY-finger5-2.lY-Angle", //257 
 "finger2-3.lY-finger5-3.lY-Angle", //258 
 "finger2-3.lY-finger4-1.lY-Angle", //259 
 "finger2-3.lY-finger4-2.lY-Angle", //260 
 "finger2-3.lY-finger4-3.lY-Angle", //261 
 "finger2-3.lY-finger3-1.lY-Angle", //262 
 "finger2-3.lY-finger3-2.lY-Angle", //263 
 "finger2-3.lY-finger3-3.lY-Angle", //264 
 "finger2-3.lY-finger2-1.lY-Angle", //265 
 "finger2-3.lY-finger2-2.lY-Angle", //266 
 "angleUsedFor2DRotation_12", //267 
 "finger2-3.lY-lthumbY-Angle", //268 
 "finger2-3.lY-finger1-2.lY-Angle", //269 
 "finger2-3.lY-finger1-3.lY-Angle", //270 
 "lthumbY-lhandY-Angle", //271 
 "lthumbY-finger5-1.lY-Angle", //272 
 "lthumbY-finger5-2.lY-Angle", //273 
 "lthumbY-finger5-3.lY-Angle", //274 
 "lthumbY-finger4-1.lY-Angle", //275 
 "lthumbY-finger4-2.lY-Angle", //276 
 "lthumbY-finger4-3.lY-Angle", //277 
 "lthumbY-finger3-1.lY-Angle", //278 
 "lthumbY-finger3-2.lY-Angle", //279 
 "lthumbY-finger3-3.lY-Angle", //280 
 "lthumbY-finger2-1.lY-Angle", //281 
 "lthumbY-finger2-2.lY-Angle", //282 
 "lthumbY-finger2-3.lY-Angle", //283 
 "angleUsedFor2DRotation_13", //284 
 "lthumbY-finger1-2.lY-Angle", //285 
 "lthumbY-finger1-3.lY-Angle", //286 
 "finger1-2.lY-lhandY-Angle", //287 
 "finger1-2.lY-finger5-1.lY-Angle", //288 
 "finger1-2.lY-finger5-2.lY-Angle", //289 
 "finger1-2.lY-finger5-3.lY-Angle", //290 
 "finger1-2.lY-finger4-1.lY-Angle", //291 
 "finger1-2.lY-finger4-2.lY-Angle", //292 
 "finger1-2.lY-finger4-3.lY-Angle", //293 
 "finger1-2.lY-finger3-1.lY-Angle", //294 
 "finger1-2.lY-finger3-2.lY-Angle", //295 
 "finger1-2.lY-finger3-3.lY-Angle", //296 
 "finger1-2.lY-finger2-1.lY-Angle", //297 
 "finger1-2.lY-finger2-2.lY-Angle", //298 
 "finger1-2.lY-finger2-3.lY-Angle", //299 
 "finger1-2.lY-lthumbY-Angle", //300 
 "angleUsedFor2DRotation_14", //301 
 "finger1-2.lY-finger1-3.lY-Angle", //302 
 "finger1-3.lY-lhandY-Angle", //303 
 "finger1-3.lY-finger5-1.lY-Angle", //304 
 "finger1-3.lY-finger5-2.lY-Angle", //305 
 "finger1-3.lY-finger5-3.lY-Angle", //306 
 "finger1-3.lY-finger4-1.lY-Angle", //307 
 "finger1-3.lY-finger4-2.lY-Angle", //308 
 "finger1-3.lY-finger4-3.lY-Angle", //309 
 "finger1-3.lY-finger3-1.lY-Angle", //310 
 "finger1-3.lY-finger3-2.lY-Angle", //311 
 "finger1-3.lY-finger3-3.lY-Angle", //312 
 "finger1-3.lY-finger2-1.lY-Angle", //313 
 "finger1-3.lY-finger2-2.lY-Angle", //314 
 "finger1-3.lY-finger2-3.lY-Angle", //315 
 "finger1-3.lY-lthumbY-Angle", //316 
 "finger1-3.lY-finger1-2.lY-Angle", //317 
 "angleUsedFor2DRotation_15", //318 
 "end"
};
/** @brief Programmer friendly enumerator of expected inputs*/
enum mocapNET_lhand_enum 
{
 MNET_LHAND_IN_2DX_LHAND = 0, //0 
 MNET_LHAND_IN_2DY_LHAND, //1 
 MNET_LHAND_IN_VISIBLE_LHAND, //2 
 MNET_LHAND_IN_2DX_FINGER2_1_L, //3 
 MNET_LHAND_IN_2DY_FINGER2_1_L, //4 
 MNET_LHAND_IN_VISIBLE_FINGER2_1_L, //5 
 MNET_LHAND_IN_2DX_FINGER2_2_L, //6 
 MNET_LHAND_IN_2DY_FINGER2_2_L, //7 
 MNET_LHAND_IN_VISIBLE_FINGER2_2_L, //8 
 MNET_LHAND_IN_2DX_FINGER2_3_L, //9 
 MNET_LHAND_IN_2DY_FINGER2_3_L, //10 
 MNET_LHAND_IN_VISIBLE_FINGER2_3_L, //11 
 MNET_LHAND_IN_2DX_ENDSITE_FINGER2_3_L, //12 
 MNET_LHAND_IN_2DY_ENDSITE_FINGER2_3_L, //13 
 MNET_LHAND_IN_VISIBLE_ENDSITE_FINGER2_3_L, //14 
 MNET_LHAND_IN_2DX_FINGER3_1_L, //15 
 MNET_LHAND_IN_2DY_FINGER3_1_L, //16 
 MNET_LHAND_IN_VISIBLE_FINGER3_1_L, //17 
 MNET_LHAND_IN_2DX_FINGER3_2_L, //18 
 MNET_LHAND_IN_2DY_FINGER3_2_L, //19 
 MNET_LHAND_IN_VISIBLE_FINGER3_2_L, //20 
 MNET_LHAND_IN_2DX_FINGER3_3_L, //21 
 MNET_LHAND_IN_2DY_FINGER3_3_L, //22 
 MNET_LHAND_IN_VISIBLE_FINGER3_3_L, //23 
 MNET_LHAND_IN_2DX_ENDSITE_FINGER3_3_L, //24 
 MNET_LHAND_IN_2DY_ENDSITE_FINGER3_3_L, //25 
 MNET_LHAND_IN_VISIBLE_ENDSITE_FINGER3_3_L, //26 
 MNET_LHAND_IN_2DX_FINGER4_1_L, //27 
 MNET_LHAND_IN_2DY_FINGER4_1_L, //28 
 MNET_LHAND_IN_VISIBLE_FINGER4_1_L, //29 
 MNET_LHAND_IN_2DX_FINGER4_2_L, //30 
 MNET_LHAND_IN_2DY_FINGER4_2_L, //31 
 MNET_LHAND_IN_VISIBLE_FINGER4_2_L, //32 
 MNET_LHAND_IN_2DX_FINGER4_3_L, //33 
 MNET_LHAND_IN_2DY_FINGER4_3_L, //34 
 MNET_LHAND_IN_VISIBLE_FINGER4_3_L, //35 
 MNET_LHAND_IN_2DX_ENDSITE_FINGER4_3_L, //36 
 MNET_LHAND_IN_2DY_ENDSITE_FINGER4_3_L, //37 
 MNET_LHAND_IN_VISIBLE_ENDSITE_FINGER4_3_L, //38 
 MNET_LHAND_IN_2DX_FINGER5_1_L, //39 
 MNET_LHAND_IN_2DY_FINGER5_1_L, //40 
 MNET_LHAND_IN_VISIBLE_FINGER5_1_L, //41 
 MNET_LHAND_IN_2DX_FINGER5_2_L, //42 
 MNET_LHAND_IN_2DY_FINGER5_2_L, //43 
 MNET_LHAND_IN_VISIBLE_FINGER5_2_L, //44 
 MNET_LHAND_IN_2DX_FINGER5_3_L, //45 
 MNET_LHAND_IN_2DY_FINGER5_3_L, //46 
 MNET_LHAND_IN_VISIBLE_FINGER5_3_L, //47 
 MNET_LHAND_IN_2DX_ENDSITE_FINGER5_3_L, //48 
 MNET_LHAND_IN_2DY_ENDSITE_FINGER5_3_L, //49 
 MNET_LHAND_IN_VISIBLE_ENDSITE_FINGER5_3_L, //50 
 MNET_LHAND_IN_2DX_LTHUMB, //51 
 MNET_LHAND_IN_2DY_LTHUMB, //52 
 MNET_LHAND_IN_VISIBLE_LTHUMB, //53 
 MNET_LHAND_IN_2DX_FINGER1_2_L, //54 
 MNET_LHAND_IN_2DY_FINGER1_2_L, //55 
 MNET_LHAND_IN_VISIBLE_FINGER1_2_L, //56 
 MNET_LHAND_IN_2DX_FINGER1_3_L, //57 
 MNET_LHAND_IN_2DY_FINGER1_3_L, //58 
 MNET_LHAND_IN_VISIBLE_FINGER1_3_L, //59 
 MNET_LHAND_IN_2DX_ENDSITE_FINGER1_3_L, //60 
 MNET_LHAND_IN_2DY_ENDSITE_FINGER1_3_L, //61 
 MNET_LHAND_IN_VISIBLE_ENDSITE_FINGER1_3_L, //62 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_0, //63 
 MNET_LHAND_IN_LHANDY_FINGER5_1_LY_ANGLE, //64 
 MNET_LHAND_IN_LHANDY_FINGER5_2_LY_ANGLE, //65 
 MNET_LHAND_IN_LHANDY_FINGER5_3_LY_ANGLE, //66 
 MNET_LHAND_IN_LHANDY_FINGER4_1_LY_ANGLE, //67 
 MNET_LHAND_IN_LHANDY_FINGER4_2_LY_ANGLE, //68 
 MNET_LHAND_IN_LHANDY_FINGER4_3_LY_ANGLE, //69 
 MNET_LHAND_IN_LHANDY_FINGER3_1_LY_ANGLE, //70 
 MNET_LHAND_IN_LHANDY_FINGER3_2_LY_ANGLE, //71 
 MNET_LHAND_IN_LHANDY_FINGER3_3_LY_ANGLE, //72 
 MNET_LHAND_IN_LHANDY_FINGER2_1_LY_ANGLE, //73 
 MNET_LHAND_IN_LHANDY_FINGER2_2_LY_ANGLE, //74 
 MNET_LHAND_IN_LHANDY_FINGER2_3_LY_ANGLE, //75 
 MNET_LHAND_IN_LHANDY_LTHUMBY_ANGLE, //76 
 MNET_LHAND_IN_LHANDY_FINGER1_2_LY_ANGLE, //77 
 MNET_LHAND_IN_LHANDY_FINGER1_3_LY_ANGLE, //78 
 MNET_LHAND_IN_FINGER5_1_LY_LHANDY_ANGLE, //79 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_1, //80 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER5_2_LY_ANGLE, //81 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER5_3_LY_ANGLE, //82 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER4_1_LY_ANGLE, //83 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER4_2_LY_ANGLE, //84 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER4_3_LY_ANGLE, //85 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER3_1_LY_ANGLE, //86 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER3_2_LY_ANGLE, //87 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER3_3_LY_ANGLE, //88 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER2_1_LY_ANGLE, //89 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER2_2_LY_ANGLE, //90 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER2_3_LY_ANGLE, //91 
 MNET_LHAND_IN_FINGER5_1_LY_LTHUMBY_ANGLE, //92 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER1_2_LY_ANGLE, //93 
 MNET_LHAND_IN_FINGER5_1_LY_FINGER1_3_LY_ANGLE, //94 
 MNET_LHAND_IN_FINGER5_2_LY_LHANDY_ANGLE, //95 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER5_1_LY_ANGLE, //96 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_2, //97 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER5_3_LY_ANGLE, //98 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER4_1_LY_ANGLE, //99 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER4_2_LY_ANGLE, //100 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER4_3_LY_ANGLE, //101 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER3_1_LY_ANGLE, //102 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER3_2_LY_ANGLE, //103 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER3_3_LY_ANGLE, //104 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER2_1_LY_ANGLE, //105 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER2_2_LY_ANGLE, //106 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER2_3_LY_ANGLE, //107 
 MNET_LHAND_IN_FINGER5_2_LY_LTHUMBY_ANGLE, //108 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER1_2_LY_ANGLE, //109 
 MNET_LHAND_IN_FINGER5_2_LY_FINGER1_3_LY_ANGLE, //110 
 MNET_LHAND_IN_FINGER5_3_LY_LHANDY_ANGLE, //111 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER5_1_LY_ANGLE, //112 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER5_2_LY_ANGLE, //113 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_3, //114 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER4_1_LY_ANGLE, //115 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER4_2_LY_ANGLE, //116 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER4_3_LY_ANGLE, //117 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER3_1_LY_ANGLE, //118 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER3_2_LY_ANGLE, //119 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER3_3_LY_ANGLE, //120 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER2_1_LY_ANGLE, //121 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER2_2_LY_ANGLE, //122 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER2_3_LY_ANGLE, //123 
 MNET_LHAND_IN_FINGER5_3_LY_LTHUMBY_ANGLE, //124 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER1_2_LY_ANGLE, //125 
 MNET_LHAND_IN_FINGER5_3_LY_FINGER1_3_LY_ANGLE, //126 
 MNET_LHAND_IN_FINGER4_1_LY_LHANDY_ANGLE, //127 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER5_1_LY_ANGLE, //128 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER5_2_LY_ANGLE, //129 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER5_3_LY_ANGLE, //130 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_4, //131 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER4_2_LY_ANGLE, //132 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER4_3_LY_ANGLE, //133 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER3_1_LY_ANGLE, //134 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER3_2_LY_ANGLE, //135 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER3_3_LY_ANGLE, //136 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER2_1_LY_ANGLE, //137 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER2_2_LY_ANGLE, //138 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER2_3_LY_ANGLE, //139 
 MNET_LHAND_IN_FINGER4_1_LY_LTHUMBY_ANGLE, //140 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER1_2_LY_ANGLE, //141 
 MNET_LHAND_IN_FINGER4_1_LY_FINGER1_3_LY_ANGLE, //142 
 MNET_LHAND_IN_FINGER4_2_LY_LHANDY_ANGLE, //143 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER5_1_LY_ANGLE, //144 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER5_2_LY_ANGLE, //145 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER5_3_LY_ANGLE, //146 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER4_1_LY_ANGLE, //147 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_5, //148 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER4_3_LY_ANGLE, //149 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER3_1_LY_ANGLE, //150 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER3_2_LY_ANGLE, //151 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER3_3_LY_ANGLE, //152 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER2_1_LY_ANGLE, //153 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER2_2_LY_ANGLE, //154 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER2_3_LY_ANGLE, //155 
 MNET_LHAND_IN_FINGER4_2_LY_LTHUMBY_ANGLE, //156 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER1_2_LY_ANGLE, //157 
 MNET_LHAND_IN_FINGER4_2_LY_FINGER1_3_LY_ANGLE, //158 
 MNET_LHAND_IN_FINGER4_3_LY_LHANDY_ANGLE, //159 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER5_1_LY_ANGLE, //160 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER5_2_LY_ANGLE, //161 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER5_3_LY_ANGLE, //162 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER4_1_LY_ANGLE, //163 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER4_2_LY_ANGLE, //164 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_6, //165 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER3_1_LY_ANGLE, //166 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER3_2_LY_ANGLE, //167 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER3_3_LY_ANGLE, //168 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER2_1_LY_ANGLE, //169 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER2_2_LY_ANGLE, //170 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER2_3_LY_ANGLE, //171 
 MNET_LHAND_IN_FINGER4_3_LY_LTHUMBY_ANGLE, //172 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER1_2_LY_ANGLE, //173 
 MNET_LHAND_IN_FINGER4_3_LY_FINGER1_3_LY_ANGLE, //174 
 MNET_LHAND_IN_FINGER3_1_LY_LHANDY_ANGLE, //175 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER5_1_LY_ANGLE, //176 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER5_2_LY_ANGLE, //177 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER5_3_LY_ANGLE, //178 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER4_1_LY_ANGLE, //179 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER4_2_LY_ANGLE, //180 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER4_3_LY_ANGLE, //181 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_7, //182 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER3_2_LY_ANGLE, //183 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER3_3_LY_ANGLE, //184 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER2_1_LY_ANGLE, //185 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER2_2_LY_ANGLE, //186 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER2_3_LY_ANGLE, //187 
 MNET_LHAND_IN_FINGER3_1_LY_LTHUMBY_ANGLE, //188 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER1_2_LY_ANGLE, //189 
 MNET_LHAND_IN_FINGER3_1_LY_FINGER1_3_LY_ANGLE, //190 
 MNET_LHAND_IN_FINGER3_2_LY_LHANDY_ANGLE, //191 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER5_1_LY_ANGLE, //192 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER5_2_LY_ANGLE, //193 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER5_3_LY_ANGLE, //194 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER4_1_LY_ANGLE, //195 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER4_2_LY_ANGLE, //196 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER4_3_LY_ANGLE, //197 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER3_1_LY_ANGLE, //198 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_8, //199 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER3_3_LY_ANGLE, //200 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER2_1_LY_ANGLE, //201 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER2_2_LY_ANGLE, //202 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER2_3_LY_ANGLE, //203 
 MNET_LHAND_IN_FINGER3_2_LY_LTHUMBY_ANGLE, //204 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER1_2_LY_ANGLE, //205 
 MNET_LHAND_IN_FINGER3_2_LY_FINGER1_3_LY_ANGLE, //206 
 MNET_LHAND_IN_FINGER3_3_LY_LHANDY_ANGLE, //207 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER5_1_LY_ANGLE, //208 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER5_2_LY_ANGLE, //209 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER5_3_LY_ANGLE, //210 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER4_1_LY_ANGLE, //211 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER4_2_LY_ANGLE, //212 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER4_3_LY_ANGLE, //213 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER3_1_LY_ANGLE, //214 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER3_2_LY_ANGLE, //215 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_9, //216 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER2_1_LY_ANGLE, //217 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER2_2_LY_ANGLE, //218 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER2_3_LY_ANGLE, //219 
 MNET_LHAND_IN_FINGER3_3_LY_LTHUMBY_ANGLE, //220 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER1_2_LY_ANGLE, //221 
 MNET_LHAND_IN_FINGER3_3_LY_FINGER1_3_LY_ANGLE, //222 
 MNET_LHAND_IN_FINGER2_1_LY_LHANDY_ANGLE, //223 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER5_1_LY_ANGLE, //224 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER5_2_LY_ANGLE, //225 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER5_3_LY_ANGLE, //226 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER4_1_LY_ANGLE, //227 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER4_2_LY_ANGLE, //228 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER4_3_LY_ANGLE, //229 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER3_1_LY_ANGLE, //230 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER3_2_LY_ANGLE, //231 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER3_3_LY_ANGLE, //232 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_10, //233 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER2_2_LY_ANGLE, //234 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER2_3_LY_ANGLE, //235 
 MNET_LHAND_IN_FINGER2_1_LY_LTHUMBY_ANGLE, //236 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER1_2_LY_ANGLE, //237 
 MNET_LHAND_IN_FINGER2_1_LY_FINGER1_3_LY_ANGLE, //238 
 MNET_LHAND_IN_FINGER2_2_LY_LHANDY_ANGLE, //239 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER5_1_LY_ANGLE, //240 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER5_2_LY_ANGLE, //241 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER5_3_LY_ANGLE, //242 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER4_1_LY_ANGLE, //243 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER4_2_LY_ANGLE, //244 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER4_3_LY_ANGLE, //245 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER3_1_LY_ANGLE, //246 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER3_2_LY_ANGLE, //247 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER3_3_LY_ANGLE, //248 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER2_1_LY_ANGLE, //249 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_11, //250 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER2_3_LY_ANGLE, //251 
 MNET_LHAND_IN_FINGER2_2_LY_LTHUMBY_ANGLE, //252 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER1_2_LY_ANGLE, //253 
 MNET_LHAND_IN_FINGER2_2_LY_FINGER1_3_LY_ANGLE, //254 
 MNET_LHAND_IN_FINGER2_3_LY_LHANDY_ANGLE, //255 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER5_1_LY_ANGLE, //256 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER5_2_LY_ANGLE, //257 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER5_3_LY_ANGLE, //258 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER4_1_LY_ANGLE, //259 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER4_2_LY_ANGLE, //260 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER4_3_LY_ANGLE, //261 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER3_1_LY_ANGLE, //262 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER3_2_LY_ANGLE, //263 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER3_3_LY_ANGLE, //264 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER2_1_LY_ANGLE, //265 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER2_2_LY_ANGLE, //266 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_12, //267 
 MNET_LHAND_IN_FINGER2_3_LY_LTHUMBY_ANGLE, //268 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER1_2_LY_ANGLE, //269 
 MNET_LHAND_IN_FINGER2_3_LY_FINGER1_3_LY_ANGLE, //270 
 MNET_LHAND_IN_LTHUMBY_LHANDY_ANGLE, //271 
 MNET_LHAND_IN_LTHUMBY_FINGER5_1_LY_ANGLE, //272 
 MNET_LHAND_IN_LTHUMBY_FINGER5_2_LY_ANGLE, //273 
 MNET_LHAND_IN_LTHUMBY_FINGER5_3_LY_ANGLE, //274 
 MNET_LHAND_IN_LTHUMBY_FINGER4_1_LY_ANGLE, //275 
 MNET_LHAND_IN_LTHUMBY_FINGER4_2_LY_ANGLE, //276 
 MNET_LHAND_IN_LTHUMBY_FINGER4_3_LY_ANGLE, //277 
 MNET_LHAND_IN_LTHUMBY_FINGER3_1_LY_ANGLE, //278 
 MNET_LHAND_IN_LTHUMBY_FINGER3_2_LY_ANGLE, //279 
 MNET_LHAND_IN_LTHUMBY_FINGER3_3_LY_ANGLE, //280 
 MNET_LHAND_IN_LTHUMBY_FINGER2_1_LY_ANGLE, //281 
 MNET_LHAND_IN_LTHUMBY_FINGER2_2_LY_ANGLE, //282 
 MNET_LHAND_IN_LTHUMBY_FINGER2_3_LY_ANGLE, //283 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_13, //284 
 MNET_LHAND_IN_LTHUMBY_FINGER1_2_LY_ANGLE, //285 
 MNET_LHAND_IN_LTHUMBY_FINGER1_3_LY_ANGLE, //286 
 MNET_LHAND_IN_FINGER1_2_LY_LHANDY_ANGLE, //287 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER5_1_LY_ANGLE, //288 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER5_2_LY_ANGLE, //289 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER5_3_LY_ANGLE, //290 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER4_1_LY_ANGLE, //291 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER4_2_LY_ANGLE, //292 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER4_3_LY_ANGLE, //293 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER3_1_LY_ANGLE, //294 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER3_2_LY_ANGLE, //295 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER3_3_LY_ANGLE, //296 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER2_1_LY_ANGLE, //297 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER2_2_LY_ANGLE, //298 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER2_3_LY_ANGLE, //299 
 MNET_LHAND_IN_FINGER1_2_LY_LTHUMBY_ANGLE, //300 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_14, //301 
 MNET_LHAND_IN_FINGER1_2_LY_FINGER1_3_LY_ANGLE, //302 
 MNET_LHAND_IN_FINGER1_3_LY_LHANDY_ANGLE, //303 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER5_1_LY_ANGLE, //304 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER5_2_LY_ANGLE, //305 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER5_3_LY_ANGLE, //306 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER4_1_LY_ANGLE, //307 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER4_2_LY_ANGLE, //308 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER4_3_LY_ANGLE, //309 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER3_1_LY_ANGLE, //310 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER3_2_LY_ANGLE, //311 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER3_3_LY_ANGLE, //312 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER2_1_LY_ANGLE, //313 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER2_2_LY_ANGLE, //314 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER2_3_LY_ANGLE, //315 
 MNET_LHAND_IN_FINGER1_3_LY_LTHUMBY_ANGLE, //316 
 MNET_LHAND_IN_FINGER1_3_LY_FINGER1_2_LY_ANGLE, //317 
 MNET_LHAND_IN_ANGLEUSEDFOR2DROTATION_15, //318 
 MNET_LHAND_IN_NUMBER
};

/** @brief Programmer friendly enumerator of expected outputs
 TODO: CAREFULL!*/
enum mocapNET_Output_lhand_enum 
{
 MOCAPNET_LHAND_OUTPUT_LHAND_XPOSITION = 0, //0 
 MOCAPNET_LHAND_OUTPUT_LHAND_YPOSITION, //1 
 MOCAPNET_LHAND_OUTPUT_LHAND_ZPOSITION, //2 
 MOCAPNET_LHAND_OUTPUT_LHAND_WROTATION, //3 
 MOCAPNET_LHAND_OUTPUT_LHAND_XROTATION, //4 
 MOCAPNET_LHAND_OUTPUT_LHAND_YROTATION, //5 
 MOCAPNET_LHAND_OUTPUT_LHAND_ZROTATION, //6 
 MOCAPNET_LHAND_OUTPUT_FINGER2_1_L_ZROTATION, //7 
 MOCAPNET_LHAND_OUTPUT_FINGER2_1_L_XROTATION, //8 
 MOCAPNET_LHAND_OUTPUT_FINGER2_1_L_YROTATION, //9 
 MOCAPNET_LHAND_OUTPUT_FINGER2_2_L_ZROTATION, //10 
 MOCAPNET_LHAND_OUTPUT_FINGER2_2_L_XROTATION, //11 
 MOCAPNET_LHAND_OUTPUT_FINGER2_2_L_YROTATION, //12 
 MOCAPNET_LHAND_OUTPUT_FINGER2_3_L_ZROTATION, //13 
 MOCAPNET_LHAND_OUTPUT_FINGER2_3_L_XROTATION, //14 
 MOCAPNET_LHAND_OUTPUT_FINGER2_3_L_YROTATION, //15 
 MOCAPNET_LHAND_OUTPUT_FINGER3_1_L_ZROTATION, //16 
 MOCAPNET_LHAND_OUTPUT_FINGER3_1_L_XROTATION, //17 
 MOCAPNET_LHAND_OUTPUT_FINGER3_1_L_YROTATION, //18 
 MOCAPNET_LHAND_OUTPUT_FINGER3_2_L_ZROTATION, //19 
 MOCAPNET_LHAND_OUTPUT_FINGER3_2_L_XROTATION, //20 
 MOCAPNET_LHAND_OUTPUT_FINGER3_2_L_YROTATION, //21 
 MOCAPNET_LHAND_OUTPUT_FINGER3_3_L_ZROTATION, //22 
 MOCAPNET_LHAND_OUTPUT_FINGER3_3_L_XROTATION, //23 
 MOCAPNET_LHAND_OUTPUT_FINGER3_3_L_YROTATION, //24 
 MOCAPNET_LHAND_OUTPUT_FINGER4_1_L_ZROTATION, //25 
 MOCAPNET_LHAND_OUTPUT_FINGER4_1_L_XROTATION, //26 
 MOCAPNET_LHAND_OUTPUT_FINGER4_1_L_YROTATION, //27 
 MOCAPNET_LHAND_OUTPUT_FINGER4_2_L_ZROTATION, //28 
 MOCAPNET_LHAND_OUTPUT_FINGER4_2_L_XROTATION, //29 
 MOCAPNET_LHAND_OUTPUT_FINGER4_2_L_YROTATION, //30 
 MOCAPNET_LHAND_OUTPUT_FINGER4_3_L_ZROTATION, //31 
 MOCAPNET_LHAND_OUTPUT_FINGER4_3_L_XROTATION, //32 
 MOCAPNET_LHAND_OUTPUT_FINGER4_3_L_YROTATION, //33 
 MOCAPNET_LHAND_OUTPUT_FINGER5_1_L_ZROTATION, //34 
 MOCAPNET_LHAND_OUTPUT_FINGER5_1_L_XROTATION, //35 
 MOCAPNET_LHAND_OUTPUT_FINGER5_1_L_YROTATION, //36 
 MOCAPNET_LHAND_OUTPUT_FINGER5_2_L_ZROTATION, //37 
 MOCAPNET_LHAND_OUTPUT_FINGER5_2_L_XROTATION, //38 
 MOCAPNET_LHAND_OUTPUT_FINGER5_2_L_YROTATION, //39 
 MOCAPNET_LHAND_OUTPUT_FINGER5_3_L_ZROTATION, //40 
 MOCAPNET_LHAND_OUTPUT_FINGER5_3_L_XROTATION, //41 
 MOCAPNET_LHAND_OUTPUT_FINGER5_3_L_YROTATION, //42 
 MOCAPNET_LHAND_OUTPUT_LTHUMBBASE_ZROTATION, //43 
 MOCAPNET_LHAND_OUTPUT_LTHUMBBASE_XROTATION, //44 
 MOCAPNET_LHAND_OUTPUT_LTHUMBBASE_YROTATION, //45 
 MOCAPNET_LHAND_OUTPUT_LTHUMB_ZROTATION, //46 
 MOCAPNET_LHAND_OUTPUT_LTHUMB_XROTATION, //47 
 MOCAPNET_LHAND_OUTPUT_LTHUMB_YROTATION, //48 
 MOCAPNET_LHAND_OUTPUT_FINGER1_2_L_ZROTATION, //49 
 MOCAPNET_LHAND_OUTPUT_FINGER1_2_L_XROTATION, //50 
 MOCAPNET_LHAND_OUTPUT_FINGER1_2_L_YROTATION, //51 
 MOCAPNET_LHAND_OUTPUT_FINGER1_3_L_ZROTATION, //52 
 MOCAPNET_LHAND_OUTPUT_FINGER1_3_L_XROTATION, //53 
 MOCAPNET_LHAND_OUTPUT_FINGER1_3_L_YROTATION, //54 
 MOCAPNET_LHAND_OUTPUT_NUMBER
};

/** @brief Programmer friendly enumerator of NSDM elments*/
enum mocapNET_NSDM_lhand_enum 
{
 MNET_NSDM_LHAND_LHAND = 0, //0 
 MNET_NSDM_LHAND_FINGER5_1_L, //1 
 MNET_NSDM_LHAND_FINGER5_2_L, //2 
 MNET_NSDM_LHAND_FINGER5_3_L, //3 
 MNET_NSDM_LHAND_FINGER4_1_L, //4 
 MNET_NSDM_LHAND_FINGER4_2_L, //5 
 MNET_NSDM_LHAND_FINGER4_3_L, //6 
 MNET_NSDM_LHAND_FINGER3_1_L, //7 
 MNET_NSDM_LHAND_FINGER3_2_L, //8 
 MNET_NSDM_LHAND_FINGER3_3_L, //9 
 MNET_NSDM_LHAND_FINGER2_1_L, //10 
 MNET_NSDM_LHAND_FINGER2_2_L, //11 
 MNET_NSDM_LHAND_FINGER2_3_L, //12 
 MNET_NSDM_LHAND_LTHUMB, //13 
 MNET_NSDM_LHAND_FINGER1_2_L, //14 
 MNET_NSDM_LHAND_FINGER1_3_L, //15 
 MNET_NSDM_LHAND_NUMBER
};

/** @brief This is a lookup table to immediately resolve referred Joints*/
static const int mocapNET_ResolveJoint_lhand[] = 
{
 0, //0 
 13, //1 
 14, //2 
 15, //3 
 9, //4 
 10, //5 
 11, //6 
 5, //7 
 6, //8 
 7, //9 
 1, //10 
 2, //11 
 3, //12 
 17, //13 
 18, //14 
 19, //15 
 0//end of array
};

/** @brief This is a lookup table to immediately resolve referred Joints of second targets*/
static const int mocapNET_ResolveSecondTargetJoint_lhand[] = 
{
 0, //0 
 0, //1 
 0, //2 
 0, //3 
 0, //4 
 0, //5 
 0, //6 
 0, //7 
 0, //8 
 0, //9 
 0, //10 
 0, //11 
 0, //12 
 0, //13 
 0, //14 
 0, //15 
 0//end of array
};

/** @brief This is the configuration of NSDM elements :
 *  A value of 0 is a normal 2D point 
 *  A value of 1 is a 2D point plus some offset
 *  A value of 2 is a virtual point between two 2D points */
static const int mocapNET_ArtificialJoint_lhand[] = 
{
 0, //0 
 0, //1 
 0, //2 
 0, //3 
 0, //4 
 0, //5 
 0, //6 
 0, //7 
 0, //8 
 0, //9 
 0, //10 
 0, //11 
 0, //12 
 0, //13 
 0, //14 
 0, //15 
 0//end of array
};

/** @brief These are X offsets for artificial joints of type 1 ( see mocapNET_ArtificialJoint_lhand )*/
static const float mocapNET_ArtificialJointXOffset_lhand[] = 
{
 0, //0 
 0, //1 
 0, //2 
 0, //3 
 0, //4 
 0, //5 
 0, //6 
 0, //7 
 0, //8 
 0, //9 
 0, //10 
 0, //11 
 0, //12 
 0, //13 
 0, //14 
 0, //15 
 0//end of array
};

/** @brief These are Y offsets for artificial joints of type 1 ( see mocapNET_ArtificialJoint_lhand )*/
static const float mocapNET_ArtificialJointYOffset_lhand[] = 
{
 0, //0 
 0, //1 
 0, //2 
 0, //3 
 0, //4 
 0, //5 
 0, //6 
 0, //7 
 0, //8 
 0, //9 
 0, //10 
 0, //11 
 0, //12 
 0, //13 
 0, //14 
 0, //15 
 0//end of array
};

/** @brief These are 2D Joints that are used as starting points for scaling vectors*/
static const int mocapNET_ScalingStart_lhand[] = 
{
 0, //0 
 0, //1 
 0, //2 
 0, //3 
 0, //4 
 0//end of array
};

/** @brief These are 2D Joints that are used as ending points for scaling vectors*/
static const int mocapNET_ScalingEnd_lhand[] = 
{
 13, //0 
 9, //1 
 5, //2 
 1, //3 
 17, //4 
 0//end of array
};

/** @brief These is a 2D Joints that is used as alignment for the skeleton*/
static const int mocapNET_AlignmentStart_lhand[] = 
{
 0, //0 
 0//end of array
};

/** @brief These is a 2D Joints that is used as alignment for the skeleton*/
static const int mocapNET_AlignmentEnd_lhand[] = 
{
 5, //0 
 0//end of array
};

/** @brief This function can be used to debug NSDM input and find in a user friendly what is missing..!*/
static int lhandCountMissingNSDMElements(std::vector<float> mocapNETInput,int verbose)
{
    unsigned int numberOfZeros=0;
    for (int i=0; i<mocapNETInput.size(); i++)
        {
            if (mocapNETInput[i]==0)
                {
                   if (verbose) { fprintf(stderr,"lhand NSDM %s missing\n",mocapNET_lhand[i]); }
                    ++numberOfZeros;
                }
        }
    return numberOfZeros;
}

/** @brief Another debug function to make sure the joint order is correctly resolved..!*/
static void lhandDebugNSDMJointOrder(char ** labels)
{ 
    for (int i=0; i<MNET_NSDM_LHAND_NUMBER; i++)
        {
            fprintf(stderr,"NSDM lhand %s => skeletonSerialized %s\n ",mocapNET_lhand[i],labels[i]);       
        }
}

/** @brief This function returns the euclidean distance between two input 2D joints and zero if either of them is invalid*/
static float getJoint2DDistance_LHAND(std::vector<float> in,int jointA,int jointB)
 {
    float aX=in[jointA*3+0];
    float aY=in[jointA*3+1];
    float bX=in[jointB*3+0];
    float bY=in[jointB*3+1];
    if ( ((aX==0) && (aY==0)) || ((bX==0) && (bY==0)) ) {return 0.0;}
        

    float xDistance=(float) bX-aX;
    float yDistance=(float) bY-aY;
    return sqrt( (xDistance*xDistance) + (yDistance*yDistance) );
 }

/** @brief This function returns a vector of NSDM values ready for use with the MocapNET lhand network */
static std::vector<float> lhandCreateNDSM(std::vector<float> in,float alignmentAngle2D,int havePositionalElements,int haveAngularElements,int doNormalization,int eNSRM)
 {
    std::vector<float> result;
    for (int i=0; i<MNET_NSDM_LHAND_NUMBER; i++)
    {
     //------------------------------------
     unsigned int iJointID=mocapNET_ResolveJoint_lhand[i];
     float iX=in[iJointID*3+0];
     float iY=in[iJointID*3+1];
     float iVisibility=in[iJointID*3+2];
     int invalidPointI=0;

     //------------------------------------
     unsigned int jJointID;
     float jX=0.0;
     float jY=0.0;
     float jVisibility=0.0;
     int invalidPointJ=0;

     //------------------------------------

       if ((iX==0) && (iY==0))
       { 
         for (int j=0; j<MNET_NSDM_LHAND_NUMBER; j++)
                      {
                          if (havePositionalElements) { result.push_back(0.0); result.push_back(0.0); } 
                          if (haveAngularElements)    { result.push_back(0.0); } 
                      }
         invalidPointI=1;
       } else 
       {  
     for (int j=0; j<MNET_NSDM_LHAND_NUMBER; j++)
     {
       jJointID=mocapNET_ResolveJoint_lhand[j];
       jX=in[jJointID*3+0];
       jY=in[jJointID*3+1];
       jVisibility=in[jJointID*3+2];
       invalidPointJ=0;

       if ((jX==0) && (jY==0))
       { 
         if (havePositionalElements) { result.push_back(0.0); result.push_back(0.0); }
         if (haveAngularElements)    { result.push_back(0.0); }
         invalidPointJ=1;

       } 
       else
       {
    //---------------------------------
    if (invalidPointJ||invalidPointI)
        {
                if (havePositionalElements) { result.push_back(0.0); result.push_back(0.0); }
                if (haveAngularElements)    { result.push_back(0.0); }
        } else 
        {
                if (havePositionalElements)
                {
                 result.push_back(iX-jX);
                 result.push_back(iY-jY);
                }
                if (haveAngularElements)
                {
                 result.push_back(getAngleToAlignToZero_tools(iX,iY,jX,jY));
                }
        }
    //---------------------------------
       }  // proper j Joint

       }   // j for loop end
     } // proper i Joint
    }  // i for loop end


 if (doNormalization)
 {
if (havePositionalElements)
{
  //Normalization-------------------------------------------------------------------------------
  unsigned int numberOfNSDMScalingRules=5;
      if (numberOfNSDMScalingRules>0)
        {
         unsigned int numberOfDistanceSamples=0;
         float sumOfDistanceSamples=0.0;
         for ( int i=0; i<numberOfNSDMScalingRules; i++) 
            {
             unsigned int iJointID=mocapNET_ScalingStart_lhand[i];
             unsigned int jJointID=mocapNET_ScalingEnd_lhand[i];
             float distance = getJoint2DDistance_LHAND(in,iJointID,jJointID);
             if (distance>0.0)
               {
                numberOfDistanceSamples=numberOfDistanceSamples+1;
                sumOfDistanceSamples=sumOfDistanceSamples+distance;
               }
            }
//------------------------------------------------------------------------------------------------- 
        float scaleDistance=1.0;
//------------------------------------------------------------------------------------------------- 
        if (numberOfDistanceSamples>0)
            { scaleDistance=(float) sumOfDistanceSamples/numberOfDistanceSamples; }
//-------------------------------------------------------------------------------------------------
        if (scaleDistance!=1.0)
            {
             for (int i=0; i<result.size(); i++)
                  { result[i]=result[i]/scaleDistance; } 
            }
          } //If there are normalization rules..
//-------------------------------------------------------------------------------------------------
       for (int i=0; i<result.size(); i++)
            {
              if (result[i]!=0.0)
                { result[i]=0.5+result[i]; }
            }
//-------------------------------------------------------------------------------------------------
}



//-------------------------------------------------------------------------------------------------
if ( (haveAngularElements) && (!havePositionalElements) )
{
  float minValue=result[0], maxValue=result[0];
  for (int i=0; i<result.size(); i++)
  {
    if (result[i]<minValue) { minValue=result[i];} 
    if (result[i]>maxValue) { maxValue=result[i];} 
  }
  fprintf(stderr,"Original Min Value %0.2f, Max Value %0.2f \n",minValue,maxValue);


             unsigned int iJointID=mocapNET_AlignmentStart_lhand[0];
             unsigned int jJointID=mocapNET_AlignmentEnd_lhand[0];
             float aX=in[iJointID*3+0];
             float aY=in[iJointID*3+1];
             float bX=in[jJointID*3+0];
             float bY=in[jJointID*3+1];
             float alignmentAngle=getAngleToAlignToZero_tools(aX,aY,bX,bY);
       for (int i=0; i<result.size(); i++)
              {
               if (result[i]!=0.0)
                 { result[i]= alignmentAngle + result[i]; }
              }
  minValue=result[0], maxValue=result[0];
  for (int i=0; i<result.size(); i++)
  {
    if (result[i]<minValue) { minValue=result[i];} 
    if (result[i]>maxValue) { maxValue=result[i];} 
  }
  fprintf(stderr,"Aligned Min Value %0.2f, Max Value %0.2f \n",minValue,maxValue);


}
//-------------------------------------------------------------------------------------------------


} //If normalization is enabled..


//New normalization code that overrides diagonal of Matrix
if (eNSRM)
{
unsigned int elementID=0;
unsigned int firstJointID=mocapNET_ResolveJoint_lhand[0];
for (unsigned int i=0; i<MNET_NSDM_LHAND_NUMBER; i++)
{
for (unsigned int j=0; j<MNET_NSDM_LHAND_NUMBER; j++)
{
if (i==j)
{
  result[elementID]=alignmentAngle2D;
  //Rest diagonal elements will encode euclidean distances from base joint
  //if eNSRM flag is set.. 
  if (i>0)
      {  
        unsigned int jJointID=mocapNET_ResolveJoint_lhand[j];  
        result[elementID]=getJoint2DDistance_LHAND(in,firstJointID,jJointID); 
      }
}
elementID+=1;
}
}
}



 return result;
 }

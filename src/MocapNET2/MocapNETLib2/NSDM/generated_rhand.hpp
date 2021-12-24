/** @file generated_rhand.hpp
 *  @brief A Description the rhand input of a Tensorflow network required for MocapNET 
 *  @author Ammar Qammaz (AmmarkoV)
 *  Automatically generated @ 2021-03-02 15:24:17.397745 using : 
 *     python3 exportCPPCodeFromJSONConfiguration.py --front rhand --config dataset/rhand_configuration.json
 *     please note that since the names of the labels are both affected by the dataset/rhand_configuration.json configuration 
 *     as well as the ground truth, if you have made any weird additions you might consider running the ./createRandomizedDataset.sh and ./createTestDataset.sh scripts 
 */

#pragma once

#include <iostream>
#include <math.h>
#include <vector>
#include "../tools.hpp"

/** @brief This is an array of names for all uncompressed 2D inputs expected. */
static const unsigned int mocapNET_InputLength_WithoutNSDM_rhand = 63; 

/** @brief Use rich diagonal, part of networks after 31-01-2021 */
static const unsigned int richDiagonal_rhand = 1; 

/** @brief An array of strings that contains the label for each expected input. */
static const char * mocapNET_rhand[] = 
{
 "2DX_rhand", //0 
 "2DY_rhand", //1 
 "visible_rhand", //2 
 "2DX_finger2-1.r", //3 
 "2DY_finger2-1.r", //4 
 "visible_finger2-1.r", //5 
 "2DX_finger2-2.r", //6 
 "2DY_finger2-2.r", //7 
 "visible_finger2-2.r", //8 
 "2DX_finger2-3.r", //9 
 "2DY_finger2-3.r", //10 
 "visible_finger2-3.r", //11 
 "2DX_EndSite_finger2-3.r", //12 
 "2DY_EndSite_finger2-3.r", //13 
 "visible_EndSite_finger2-3.r", //14 
 "2DX_finger3-1.r", //15 
 "2DY_finger3-1.r", //16 
 "visible_finger3-1.r", //17 
 "2DX_finger3-2.r", //18 
 "2DY_finger3-2.r", //19 
 "visible_finger3-2.r", //20 
 "2DX_finger3-3.r", //21 
 "2DY_finger3-3.r", //22 
 "visible_finger3-3.r", //23 
 "2DX_EndSite_finger3-3.r", //24 
 "2DY_EndSite_finger3-3.r", //25 
 "visible_EndSite_finger3-3.r", //26 
 "2DX_finger4-1.r", //27 
 "2DY_finger4-1.r", //28 
 "visible_finger4-1.r", //29 
 "2DX_finger4-2.r", //30 
 "2DY_finger4-2.r", //31 
 "visible_finger4-2.r", //32 
 "2DX_finger4-3.r", //33 
 "2DY_finger4-3.r", //34 
 "visible_finger4-3.r", //35 
 "2DX_EndSite_finger4-3.r", //36 
 "2DY_EndSite_finger4-3.r", //37 
 "visible_EndSite_finger4-3.r", //38 
 "2DX_finger5-1.r", //39 
 "2DY_finger5-1.r", //40 
 "visible_finger5-1.r", //41 
 "2DX_finger5-2.r", //42 
 "2DY_finger5-2.r", //43 
 "visible_finger5-2.r", //44 
 "2DX_finger5-3.r", //45 
 "2DY_finger5-3.r", //46 
 "visible_finger5-3.r", //47 
 "2DX_EndSite_finger5-3.r", //48 
 "2DY_EndSite_finger5-3.r", //49 
 "visible_EndSite_finger5-3.r", //50 
 "2DX_rthumb", //51 
 "2DY_rthumb", //52 
 "visible_rthumb", //53 
 "2DX_finger1-2.r", //54 
 "2DY_finger1-2.r", //55 
 "visible_finger1-2.r", //56 
 "2DX_finger1-3.r", //57 
 "2DY_finger1-3.r", //58 
 "visible_finger1-3.r", //59 
 "2DX_EndSite_finger1-3.r", //60 
 "2DY_EndSite_finger1-3.r", //61 
 "visible_EndSite_finger1-3.r", //62 
//This is where regular input ends and the NSDM data kicks in..
 "angleUsedFor2DRotation_0", //63 
 "rhandY-finger5-1.rY-Angle", //64 
 "rhandY-finger5-2.rY-Angle", //65 
 "rhandY-finger5-3.rY-Angle", //66 
 "rhandY-finger4-1.rY-Angle", //67 
 "rhandY-finger4-2.rY-Angle", //68 
 "rhandY-finger4-3.rY-Angle", //69 
 "rhandY-finger3-1.rY-Angle", //70 
 "rhandY-finger3-2.rY-Angle", //71 
 "rhandY-finger3-3.rY-Angle", //72 
 "rhandY-finger2-1.rY-Angle", //73 
 "rhandY-finger2-2.rY-Angle", //74 
 "rhandY-finger2-3.rY-Angle", //75 
 "rhandY-rthumbY-Angle", //76 
 "rhandY-finger1-2.rY-Angle", //77 
 "rhandY-finger1-3.rY-Angle", //78 
 "finger5-1.rY-rhandY-Angle", //79 
 "angleUsedFor2DRotation_1", //80 
 "finger5-1.rY-finger5-2.rY-Angle", //81 
 "finger5-1.rY-finger5-3.rY-Angle", //82 
 "finger5-1.rY-finger4-1.rY-Angle", //83 
 "finger5-1.rY-finger4-2.rY-Angle", //84 
 "finger5-1.rY-finger4-3.rY-Angle", //85 
 "finger5-1.rY-finger3-1.rY-Angle", //86 
 "finger5-1.rY-finger3-2.rY-Angle", //87 
 "finger5-1.rY-finger3-3.rY-Angle", //88 
 "finger5-1.rY-finger2-1.rY-Angle", //89 
 "finger5-1.rY-finger2-2.rY-Angle", //90 
 "finger5-1.rY-finger2-3.rY-Angle", //91 
 "finger5-1.rY-rthumbY-Angle", //92 
 "finger5-1.rY-finger1-2.rY-Angle", //93 
 "finger5-1.rY-finger1-3.rY-Angle", //94 
 "finger5-2.rY-rhandY-Angle", //95 
 "finger5-2.rY-finger5-1.rY-Angle", //96 
 "angleUsedFor2DRotation_2", //97 
 "finger5-2.rY-finger5-3.rY-Angle", //98 
 "finger5-2.rY-finger4-1.rY-Angle", //99 
 "finger5-2.rY-finger4-2.rY-Angle", //100 
 "finger5-2.rY-finger4-3.rY-Angle", //101 
 "finger5-2.rY-finger3-1.rY-Angle", //102 
 "finger5-2.rY-finger3-2.rY-Angle", //103 
 "finger5-2.rY-finger3-3.rY-Angle", //104 
 "finger5-2.rY-finger2-1.rY-Angle", //105 
 "finger5-2.rY-finger2-2.rY-Angle", //106 
 "finger5-2.rY-finger2-3.rY-Angle", //107 
 "finger5-2.rY-rthumbY-Angle", //108 
 "finger5-2.rY-finger1-2.rY-Angle", //109 
 "finger5-2.rY-finger1-3.rY-Angle", //110 
 "finger5-3.rY-rhandY-Angle", //111 
 "finger5-3.rY-finger5-1.rY-Angle", //112 
 "finger5-3.rY-finger5-2.rY-Angle", //113 
 "angleUsedFor2DRotation_3", //114 
 "finger5-3.rY-finger4-1.rY-Angle", //115 
 "finger5-3.rY-finger4-2.rY-Angle", //116 
 "finger5-3.rY-finger4-3.rY-Angle", //117 
 "finger5-3.rY-finger3-1.rY-Angle", //118 
 "finger5-3.rY-finger3-2.rY-Angle", //119 
 "finger5-3.rY-finger3-3.rY-Angle", //120 
 "finger5-3.rY-finger2-1.rY-Angle", //121 
 "finger5-3.rY-finger2-2.rY-Angle", //122 
 "finger5-3.rY-finger2-3.rY-Angle", //123 
 "finger5-3.rY-rthumbY-Angle", //124 
 "finger5-3.rY-finger1-2.rY-Angle", //125 
 "finger5-3.rY-finger1-3.rY-Angle", //126 
 "finger4-1.rY-rhandY-Angle", //127 
 "finger4-1.rY-finger5-1.rY-Angle", //128 
 "finger4-1.rY-finger5-2.rY-Angle", //129 
 "finger4-1.rY-finger5-3.rY-Angle", //130 
 "angleUsedFor2DRotation_4", //131 
 "finger4-1.rY-finger4-2.rY-Angle", //132 
 "finger4-1.rY-finger4-3.rY-Angle", //133 
 "finger4-1.rY-finger3-1.rY-Angle", //134 
 "finger4-1.rY-finger3-2.rY-Angle", //135 
 "finger4-1.rY-finger3-3.rY-Angle", //136 
 "finger4-1.rY-finger2-1.rY-Angle", //137 
 "finger4-1.rY-finger2-2.rY-Angle", //138 
 "finger4-1.rY-finger2-3.rY-Angle", //139 
 "finger4-1.rY-rthumbY-Angle", //140 
 "finger4-1.rY-finger1-2.rY-Angle", //141 
 "finger4-1.rY-finger1-3.rY-Angle", //142 
 "finger4-2.rY-rhandY-Angle", //143 
 "finger4-2.rY-finger5-1.rY-Angle", //144 
 "finger4-2.rY-finger5-2.rY-Angle", //145 
 "finger4-2.rY-finger5-3.rY-Angle", //146 
 "finger4-2.rY-finger4-1.rY-Angle", //147 
 "angleUsedFor2DRotation_5", //148 
 "finger4-2.rY-finger4-3.rY-Angle", //149 
 "finger4-2.rY-finger3-1.rY-Angle", //150 
 "finger4-2.rY-finger3-2.rY-Angle", //151 
 "finger4-2.rY-finger3-3.rY-Angle", //152 
 "finger4-2.rY-finger2-1.rY-Angle", //153 
 "finger4-2.rY-finger2-2.rY-Angle", //154 
 "finger4-2.rY-finger2-3.rY-Angle", //155 
 "finger4-2.rY-rthumbY-Angle", //156 
 "finger4-2.rY-finger1-2.rY-Angle", //157 
 "finger4-2.rY-finger1-3.rY-Angle", //158 
 "finger4-3.rY-rhandY-Angle", //159 
 "finger4-3.rY-finger5-1.rY-Angle", //160 
 "finger4-3.rY-finger5-2.rY-Angle", //161 
 "finger4-3.rY-finger5-3.rY-Angle", //162 
 "finger4-3.rY-finger4-1.rY-Angle", //163 
 "finger4-3.rY-finger4-2.rY-Angle", //164 
 "angleUsedFor2DRotation_6", //165 
 "finger4-3.rY-finger3-1.rY-Angle", //166 
 "finger4-3.rY-finger3-2.rY-Angle", //167 
 "finger4-3.rY-finger3-3.rY-Angle", //168 
 "finger4-3.rY-finger2-1.rY-Angle", //169 
 "finger4-3.rY-finger2-2.rY-Angle", //170 
 "finger4-3.rY-finger2-3.rY-Angle", //171 
 "finger4-3.rY-rthumbY-Angle", //172 
 "finger4-3.rY-finger1-2.rY-Angle", //173 
 "finger4-3.rY-finger1-3.rY-Angle", //174 
 "finger3-1.rY-rhandY-Angle", //175 
 "finger3-1.rY-finger5-1.rY-Angle", //176 
 "finger3-1.rY-finger5-2.rY-Angle", //177 
 "finger3-1.rY-finger5-3.rY-Angle", //178 
 "finger3-1.rY-finger4-1.rY-Angle", //179 
 "finger3-1.rY-finger4-2.rY-Angle", //180 
 "finger3-1.rY-finger4-3.rY-Angle", //181 
 "angleUsedFor2DRotation_7", //182 
 "finger3-1.rY-finger3-2.rY-Angle", //183 
 "finger3-1.rY-finger3-3.rY-Angle", //184 
 "finger3-1.rY-finger2-1.rY-Angle", //185 
 "finger3-1.rY-finger2-2.rY-Angle", //186 
 "finger3-1.rY-finger2-3.rY-Angle", //187 
 "finger3-1.rY-rthumbY-Angle", //188 
 "finger3-1.rY-finger1-2.rY-Angle", //189 
 "finger3-1.rY-finger1-3.rY-Angle", //190 
 "finger3-2.rY-rhandY-Angle", //191 
 "finger3-2.rY-finger5-1.rY-Angle", //192 
 "finger3-2.rY-finger5-2.rY-Angle", //193 
 "finger3-2.rY-finger5-3.rY-Angle", //194 
 "finger3-2.rY-finger4-1.rY-Angle", //195 
 "finger3-2.rY-finger4-2.rY-Angle", //196 
 "finger3-2.rY-finger4-3.rY-Angle", //197 
 "finger3-2.rY-finger3-1.rY-Angle", //198 
 "angleUsedFor2DRotation_8", //199 
 "finger3-2.rY-finger3-3.rY-Angle", //200 
 "finger3-2.rY-finger2-1.rY-Angle", //201 
 "finger3-2.rY-finger2-2.rY-Angle", //202 
 "finger3-2.rY-finger2-3.rY-Angle", //203 
 "finger3-2.rY-rthumbY-Angle", //204 
 "finger3-2.rY-finger1-2.rY-Angle", //205 
 "finger3-2.rY-finger1-3.rY-Angle", //206 
 "finger3-3.rY-rhandY-Angle", //207 
 "finger3-3.rY-finger5-1.rY-Angle", //208 
 "finger3-3.rY-finger5-2.rY-Angle", //209 
 "finger3-3.rY-finger5-3.rY-Angle", //210 
 "finger3-3.rY-finger4-1.rY-Angle", //211 
 "finger3-3.rY-finger4-2.rY-Angle", //212 
 "finger3-3.rY-finger4-3.rY-Angle", //213 
 "finger3-3.rY-finger3-1.rY-Angle", //214 
 "finger3-3.rY-finger3-2.rY-Angle", //215 
 "angleUsedFor2DRotation_9", //216 
 "finger3-3.rY-finger2-1.rY-Angle", //217 
 "finger3-3.rY-finger2-2.rY-Angle", //218 
 "finger3-3.rY-finger2-3.rY-Angle", //219 
 "finger3-3.rY-rthumbY-Angle", //220 
 "finger3-3.rY-finger1-2.rY-Angle", //221 
 "finger3-3.rY-finger1-3.rY-Angle", //222 
 "finger2-1.rY-rhandY-Angle", //223 
 "finger2-1.rY-finger5-1.rY-Angle", //224 
 "finger2-1.rY-finger5-2.rY-Angle", //225 
 "finger2-1.rY-finger5-3.rY-Angle", //226 
 "finger2-1.rY-finger4-1.rY-Angle", //227 
 "finger2-1.rY-finger4-2.rY-Angle", //228 
 "finger2-1.rY-finger4-3.rY-Angle", //229 
 "finger2-1.rY-finger3-1.rY-Angle", //230 
 "finger2-1.rY-finger3-2.rY-Angle", //231 
 "finger2-1.rY-finger3-3.rY-Angle", //232 
 "angleUsedFor2DRotation_10", //233 
 "finger2-1.rY-finger2-2.rY-Angle", //234 
 "finger2-1.rY-finger2-3.rY-Angle", //235 
 "finger2-1.rY-rthumbY-Angle", //236 
 "finger2-1.rY-finger1-2.rY-Angle", //237 
 "finger2-1.rY-finger1-3.rY-Angle", //238 
 "finger2-2.rY-rhandY-Angle", //239 
 "finger2-2.rY-finger5-1.rY-Angle", //240 
 "finger2-2.rY-finger5-2.rY-Angle", //241 
 "finger2-2.rY-finger5-3.rY-Angle", //242 
 "finger2-2.rY-finger4-1.rY-Angle", //243 
 "finger2-2.rY-finger4-2.rY-Angle", //244 
 "finger2-2.rY-finger4-3.rY-Angle", //245 
 "finger2-2.rY-finger3-1.rY-Angle", //246 
 "finger2-2.rY-finger3-2.rY-Angle", //247 
 "finger2-2.rY-finger3-3.rY-Angle", //248 
 "finger2-2.rY-finger2-1.rY-Angle", //249 
 "angleUsedFor2DRotation_11", //250 
 "finger2-2.rY-finger2-3.rY-Angle", //251 
 "finger2-2.rY-rthumbY-Angle", //252 
 "finger2-2.rY-finger1-2.rY-Angle", //253 
 "finger2-2.rY-finger1-3.rY-Angle", //254 
 "finger2-3.rY-rhandY-Angle", //255 
 "finger2-3.rY-finger5-1.rY-Angle", //256 
 "finger2-3.rY-finger5-2.rY-Angle", //257 
 "finger2-3.rY-finger5-3.rY-Angle", //258 
 "finger2-3.rY-finger4-1.rY-Angle", //259 
 "finger2-3.rY-finger4-2.rY-Angle", //260 
 "finger2-3.rY-finger4-3.rY-Angle", //261 
 "finger2-3.rY-finger3-1.rY-Angle", //262 
 "finger2-3.rY-finger3-2.rY-Angle", //263 
 "finger2-3.rY-finger3-3.rY-Angle", //264 
 "finger2-3.rY-finger2-1.rY-Angle", //265 
 "finger2-3.rY-finger2-2.rY-Angle", //266 
 "angleUsedFor2DRotation_12", //267 
 "finger2-3.rY-rthumbY-Angle", //268 
 "finger2-3.rY-finger1-2.rY-Angle", //269 
 "finger2-3.rY-finger1-3.rY-Angle", //270 
 "rthumbY-rhandY-Angle", //271 
 "rthumbY-finger5-1.rY-Angle", //272 
 "rthumbY-finger5-2.rY-Angle", //273 
 "rthumbY-finger5-3.rY-Angle", //274 
 "rthumbY-finger4-1.rY-Angle", //275 
 "rthumbY-finger4-2.rY-Angle", //276 
 "rthumbY-finger4-3.rY-Angle", //277 
 "rthumbY-finger3-1.rY-Angle", //278 
 "rthumbY-finger3-2.rY-Angle", //279 
 "rthumbY-finger3-3.rY-Angle", //280 
 "rthumbY-finger2-1.rY-Angle", //281 
 "rthumbY-finger2-2.rY-Angle", //282 
 "rthumbY-finger2-3.rY-Angle", //283 
 "angleUsedFor2DRotation_13", //284 
 "rthumbY-finger1-2.rY-Angle", //285 
 "rthumbY-finger1-3.rY-Angle", //286 
 "finger1-2.rY-rhandY-Angle", //287 
 "finger1-2.rY-finger5-1.rY-Angle", //288 
 "finger1-2.rY-finger5-2.rY-Angle", //289 
 "finger1-2.rY-finger5-3.rY-Angle", //290 
 "finger1-2.rY-finger4-1.rY-Angle", //291 
 "finger1-2.rY-finger4-2.rY-Angle", //292 
 "finger1-2.rY-finger4-3.rY-Angle", //293 
 "finger1-2.rY-finger3-1.rY-Angle", //294 
 "finger1-2.rY-finger3-2.rY-Angle", //295 
 "finger1-2.rY-finger3-3.rY-Angle", //296 
 "finger1-2.rY-finger2-1.rY-Angle", //297 
 "finger1-2.rY-finger2-2.rY-Angle", //298 
 "finger1-2.rY-finger2-3.rY-Angle", //299 
 "finger1-2.rY-rthumbY-Angle", //300 
 "angleUsedFor2DRotation_14", //301 
 "finger1-2.rY-finger1-3.rY-Angle", //302 
 "finger1-3.rY-rhandY-Angle", //303 
 "finger1-3.rY-finger5-1.rY-Angle", //304 
 "finger1-3.rY-finger5-2.rY-Angle", //305 
 "finger1-3.rY-finger5-3.rY-Angle", //306 
 "finger1-3.rY-finger4-1.rY-Angle", //307 
 "finger1-3.rY-finger4-2.rY-Angle", //308 
 "finger1-3.rY-finger4-3.rY-Angle", //309 
 "finger1-3.rY-finger3-1.rY-Angle", //310 
 "finger1-3.rY-finger3-2.rY-Angle", //311 
 "finger1-3.rY-finger3-3.rY-Angle", //312 
 "finger1-3.rY-finger2-1.rY-Angle", //313 
 "finger1-3.rY-finger2-2.rY-Angle", //314 
 "finger1-3.rY-finger2-3.rY-Angle", //315 
 "finger1-3.rY-rthumbY-Angle", //316 
 "finger1-3.rY-finger1-2.rY-Angle", //317 
 "angleUsedFor2DRotation_15", //318 
 "end"
};
/** @brief Programmer friendly enumerator of expected inputs*/
enum mocapNET_rhand_enum 
{
 MNET_RHAND_IN_2DX_RHAND = 0, //0 
 MNET_RHAND_IN_2DY_RHAND, //1 
 MNET_RHAND_IN_VISIBLE_RHAND, //2 
 MNET_RHAND_IN_2DX_FINGER2_1_R, //3 
 MNET_RHAND_IN_2DY_FINGER2_1_R, //4 
 MNET_RHAND_IN_VISIBLE_FINGER2_1_R, //5 
 MNET_RHAND_IN_2DX_FINGER2_2_R, //6 
 MNET_RHAND_IN_2DY_FINGER2_2_R, //7 
 MNET_RHAND_IN_VISIBLE_FINGER2_2_R, //8 
 MNET_RHAND_IN_2DX_FINGER2_3_R, //9 
 MNET_RHAND_IN_2DY_FINGER2_3_R, //10 
 MNET_RHAND_IN_VISIBLE_FINGER2_3_R, //11 
 MNET_RHAND_IN_2DX_ENDSITE_FINGER2_3_R, //12 
 MNET_RHAND_IN_2DY_ENDSITE_FINGER2_3_R, //13 
 MNET_RHAND_IN_VISIBLE_ENDSITE_FINGER2_3_R, //14 
 MNET_RHAND_IN_2DX_FINGER3_1_R, //15 
 MNET_RHAND_IN_2DY_FINGER3_1_R, //16 
 MNET_RHAND_IN_VISIBLE_FINGER3_1_R, //17 
 MNET_RHAND_IN_2DX_FINGER3_2_R, //18 
 MNET_RHAND_IN_2DY_FINGER3_2_R, //19 
 MNET_RHAND_IN_VISIBLE_FINGER3_2_R, //20 
 MNET_RHAND_IN_2DX_FINGER3_3_R, //21 
 MNET_RHAND_IN_2DY_FINGER3_3_R, //22 
 MNET_RHAND_IN_VISIBLE_FINGER3_3_R, //23 
 MNET_RHAND_IN_2DX_ENDSITE_FINGER3_3_R, //24 
 MNET_RHAND_IN_2DY_ENDSITE_FINGER3_3_R, //25 
 MNET_RHAND_IN_VISIBLE_ENDSITE_FINGER3_3_R, //26 
 MNET_RHAND_IN_2DX_FINGER4_1_R, //27 
 MNET_RHAND_IN_2DY_FINGER4_1_R, //28 
 MNET_RHAND_IN_VISIBLE_FINGER4_1_R, //29 
 MNET_RHAND_IN_2DX_FINGER4_2_R, //30 
 MNET_RHAND_IN_2DY_FINGER4_2_R, //31 
 MNET_RHAND_IN_VISIBLE_FINGER4_2_R, //32 
 MNET_RHAND_IN_2DX_FINGER4_3_R, //33 
 MNET_RHAND_IN_2DY_FINGER4_3_R, //34 
 MNET_RHAND_IN_VISIBLE_FINGER4_3_R, //35 
 MNET_RHAND_IN_2DX_ENDSITE_FINGER4_3_R, //36 
 MNET_RHAND_IN_2DY_ENDSITE_FINGER4_3_R, //37 
 MNET_RHAND_IN_VISIBLE_ENDSITE_FINGER4_3_R, //38 
 MNET_RHAND_IN_2DX_FINGER5_1_R, //39 
 MNET_RHAND_IN_2DY_FINGER5_1_R, //40 
 MNET_RHAND_IN_VISIBLE_FINGER5_1_R, //41 
 MNET_RHAND_IN_2DX_FINGER5_2_R, //42 
 MNET_RHAND_IN_2DY_FINGER5_2_R, //43 
 MNET_RHAND_IN_VISIBLE_FINGER5_2_R, //44 
 MNET_RHAND_IN_2DX_FINGER5_3_R, //45 
 MNET_RHAND_IN_2DY_FINGER5_3_R, //46 
 MNET_RHAND_IN_VISIBLE_FINGER5_3_R, //47 
 MNET_RHAND_IN_2DX_ENDSITE_FINGER5_3_R, //48 
 MNET_RHAND_IN_2DY_ENDSITE_FINGER5_3_R, //49 
 MNET_RHAND_IN_VISIBLE_ENDSITE_FINGER5_3_R, //50 
 MNET_RHAND_IN_2DX_RTHUMB, //51 
 MNET_RHAND_IN_2DY_RTHUMB, //52 
 MNET_RHAND_IN_VISIBLE_RTHUMB, //53 
 MNET_RHAND_IN_2DX_FINGER1_2_R, //54 
 MNET_RHAND_IN_2DY_FINGER1_2_R, //55 
 MNET_RHAND_IN_VISIBLE_FINGER1_2_R, //56 
 MNET_RHAND_IN_2DX_FINGER1_3_R, //57 
 MNET_RHAND_IN_2DY_FINGER1_3_R, //58 
 MNET_RHAND_IN_VISIBLE_FINGER1_3_R, //59 
 MNET_RHAND_IN_2DX_ENDSITE_FINGER1_3_R, //60 
 MNET_RHAND_IN_2DY_ENDSITE_FINGER1_3_R, //61 
 MNET_RHAND_IN_VISIBLE_ENDSITE_FINGER1_3_R, //62 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_0, //63 
 MNET_RHAND_IN_RHANDY_FINGER5_1_RY_ANGLE, //64 
 MNET_RHAND_IN_RHANDY_FINGER5_2_RY_ANGLE, //65 
 MNET_RHAND_IN_RHANDY_FINGER5_3_RY_ANGLE, //66 
 MNET_RHAND_IN_RHANDY_FINGER4_1_RY_ANGLE, //67 
 MNET_RHAND_IN_RHANDY_FINGER4_2_RY_ANGLE, //68 
 MNET_RHAND_IN_RHANDY_FINGER4_3_RY_ANGLE, //69 
 MNET_RHAND_IN_RHANDY_FINGER3_1_RY_ANGLE, //70 
 MNET_RHAND_IN_RHANDY_FINGER3_2_RY_ANGLE, //71 
 MNET_RHAND_IN_RHANDY_FINGER3_3_RY_ANGLE, //72 
 MNET_RHAND_IN_RHANDY_FINGER2_1_RY_ANGLE, //73 
 MNET_RHAND_IN_RHANDY_FINGER2_2_RY_ANGLE, //74 
 MNET_RHAND_IN_RHANDY_FINGER2_3_RY_ANGLE, //75 
 MNET_RHAND_IN_RHANDY_RTHUMBY_ANGLE, //76 
 MNET_RHAND_IN_RHANDY_FINGER1_2_RY_ANGLE, //77 
 MNET_RHAND_IN_RHANDY_FINGER1_3_RY_ANGLE, //78 
 MNET_RHAND_IN_FINGER5_1_RY_RHANDY_ANGLE, //79 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_1, //80 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER5_2_RY_ANGLE, //81 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER5_3_RY_ANGLE, //82 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER4_1_RY_ANGLE, //83 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER4_2_RY_ANGLE, //84 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER4_3_RY_ANGLE, //85 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER3_1_RY_ANGLE, //86 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER3_2_RY_ANGLE, //87 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER3_3_RY_ANGLE, //88 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER2_1_RY_ANGLE, //89 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER2_2_RY_ANGLE, //90 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER2_3_RY_ANGLE, //91 
 MNET_RHAND_IN_FINGER5_1_RY_RTHUMBY_ANGLE, //92 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER1_2_RY_ANGLE, //93 
 MNET_RHAND_IN_FINGER5_1_RY_FINGER1_3_RY_ANGLE, //94 
 MNET_RHAND_IN_FINGER5_2_RY_RHANDY_ANGLE, //95 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER5_1_RY_ANGLE, //96 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_2, //97 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER5_3_RY_ANGLE, //98 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER4_1_RY_ANGLE, //99 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER4_2_RY_ANGLE, //100 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER4_3_RY_ANGLE, //101 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER3_1_RY_ANGLE, //102 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER3_2_RY_ANGLE, //103 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER3_3_RY_ANGLE, //104 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER2_1_RY_ANGLE, //105 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER2_2_RY_ANGLE, //106 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER2_3_RY_ANGLE, //107 
 MNET_RHAND_IN_FINGER5_2_RY_RTHUMBY_ANGLE, //108 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER1_2_RY_ANGLE, //109 
 MNET_RHAND_IN_FINGER5_2_RY_FINGER1_3_RY_ANGLE, //110 
 MNET_RHAND_IN_FINGER5_3_RY_RHANDY_ANGLE, //111 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER5_1_RY_ANGLE, //112 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER5_2_RY_ANGLE, //113 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_3, //114 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER4_1_RY_ANGLE, //115 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER4_2_RY_ANGLE, //116 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER4_3_RY_ANGLE, //117 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER3_1_RY_ANGLE, //118 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER3_2_RY_ANGLE, //119 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER3_3_RY_ANGLE, //120 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER2_1_RY_ANGLE, //121 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER2_2_RY_ANGLE, //122 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER2_3_RY_ANGLE, //123 
 MNET_RHAND_IN_FINGER5_3_RY_RTHUMBY_ANGLE, //124 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER1_2_RY_ANGLE, //125 
 MNET_RHAND_IN_FINGER5_3_RY_FINGER1_3_RY_ANGLE, //126 
 MNET_RHAND_IN_FINGER4_1_RY_RHANDY_ANGLE, //127 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER5_1_RY_ANGLE, //128 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER5_2_RY_ANGLE, //129 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER5_3_RY_ANGLE, //130 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_4, //131 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER4_2_RY_ANGLE, //132 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER4_3_RY_ANGLE, //133 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER3_1_RY_ANGLE, //134 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER3_2_RY_ANGLE, //135 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER3_3_RY_ANGLE, //136 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER2_1_RY_ANGLE, //137 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER2_2_RY_ANGLE, //138 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER2_3_RY_ANGLE, //139 
 MNET_RHAND_IN_FINGER4_1_RY_RTHUMBY_ANGLE, //140 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER1_2_RY_ANGLE, //141 
 MNET_RHAND_IN_FINGER4_1_RY_FINGER1_3_RY_ANGLE, //142 
 MNET_RHAND_IN_FINGER4_2_RY_RHANDY_ANGLE, //143 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER5_1_RY_ANGLE, //144 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER5_2_RY_ANGLE, //145 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER5_3_RY_ANGLE, //146 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER4_1_RY_ANGLE, //147 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_5, //148 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER4_3_RY_ANGLE, //149 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER3_1_RY_ANGLE, //150 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER3_2_RY_ANGLE, //151 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER3_3_RY_ANGLE, //152 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER2_1_RY_ANGLE, //153 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER2_2_RY_ANGLE, //154 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER2_3_RY_ANGLE, //155 
 MNET_RHAND_IN_FINGER4_2_RY_RTHUMBY_ANGLE, //156 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER1_2_RY_ANGLE, //157 
 MNET_RHAND_IN_FINGER4_2_RY_FINGER1_3_RY_ANGLE, //158 
 MNET_RHAND_IN_FINGER4_3_RY_RHANDY_ANGLE, //159 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER5_1_RY_ANGLE, //160 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER5_2_RY_ANGLE, //161 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER5_3_RY_ANGLE, //162 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER4_1_RY_ANGLE, //163 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER4_2_RY_ANGLE, //164 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_6, //165 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER3_1_RY_ANGLE, //166 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER3_2_RY_ANGLE, //167 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER3_3_RY_ANGLE, //168 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER2_1_RY_ANGLE, //169 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER2_2_RY_ANGLE, //170 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER2_3_RY_ANGLE, //171 
 MNET_RHAND_IN_FINGER4_3_RY_RTHUMBY_ANGLE, //172 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER1_2_RY_ANGLE, //173 
 MNET_RHAND_IN_FINGER4_3_RY_FINGER1_3_RY_ANGLE, //174 
 MNET_RHAND_IN_FINGER3_1_RY_RHANDY_ANGLE, //175 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER5_1_RY_ANGLE, //176 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER5_2_RY_ANGLE, //177 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER5_3_RY_ANGLE, //178 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER4_1_RY_ANGLE, //179 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER4_2_RY_ANGLE, //180 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER4_3_RY_ANGLE, //181 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_7, //182 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER3_2_RY_ANGLE, //183 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER3_3_RY_ANGLE, //184 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER2_1_RY_ANGLE, //185 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER2_2_RY_ANGLE, //186 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER2_3_RY_ANGLE, //187 
 MNET_RHAND_IN_FINGER3_1_RY_RTHUMBY_ANGLE, //188 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER1_2_RY_ANGLE, //189 
 MNET_RHAND_IN_FINGER3_1_RY_FINGER1_3_RY_ANGLE, //190 
 MNET_RHAND_IN_FINGER3_2_RY_RHANDY_ANGLE, //191 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER5_1_RY_ANGLE, //192 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER5_2_RY_ANGLE, //193 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER5_3_RY_ANGLE, //194 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER4_1_RY_ANGLE, //195 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER4_2_RY_ANGLE, //196 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER4_3_RY_ANGLE, //197 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER3_1_RY_ANGLE, //198 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_8, //199 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER3_3_RY_ANGLE, //200 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER2_1_RY_ANGLE, //201 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER2_2_RY_ANGLE, //202 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER2_3_RY_ANGLE, //203 
 MNET_RHAND_IN_FINGER3_2_RY_RTHUMBY_ANGLE, //204 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER1_2_RY_ANGLE, //205 
 MNET_RHAND_IN_FINGER3_2_RY_FINGER1_3_RY_ANGLE, //206 
 MNET_RHAND_IN_FINGER3_3_RY_RHANDY_ANGLE, //207 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER5_1_RY_ANGLE, //208 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER5_2_RY_ANGLE, //209 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER5_3_RY_ANGLE, //210 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER4_1_RY_ANGLE, //211 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER4_2_RY_ANGLE, //212 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER4_3_RY_ANGLE, //213 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER3_1_RY_ANGLE, //214 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER3_2_RY_ANGLE, //215 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_9, //216 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER2_1_RY_ANGLE, //217 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER2_2_RY_ANGLE, //218 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER2_3_RY_ANGLE, //219 
 MNET_RHAND_IN_FINGER3_3_RY_RTHUMBY_ANGLE, //220 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER1_2_RY_ANGLE, //221 
 MNET_RHAND_IN_FINGER3_3_RY_FINGER1_3_RY_ANGLE, //222 
 MNET_RHAND_IN_FINGER2_1_RY_RHANDY_ANGLE, //223 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER5_1_RY_ANGLE, //224 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER5_2_RY_ANGLE, //225 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER5_3_RY_ANGLE, //226 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER4_1_RY_ANGLE, //227 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER4_2_RY_ANGLE, //228 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER4_3_RY_ANGLE, //229 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER3_1_RY_ANGLE, //230 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER3_2_RY_ANGLE, //231 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER3_3_RY_ANGLE, //232 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_10, //233 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER2_2_RY_ANGLE, //234 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER2_3_RY_ANGLE, //235 
 MNET_RHAND_IN_FINGER2_1_RY_RTHUMBY_ANGLE, //236 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER1_2_RY_ANGLE, //237 
 MNET_RHAND_IN_FINGER2_1_RY_FINGER1_3_RY_ANGLE, //238 
 MNET_RHAND_IN_FINGER2_2_RY_RHANDY_ANGLE, //239 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER5_1_RY_ANGLE, //240 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER5_2_RY_ANGLE, //241 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER5_3_RY_ANGLE, //242 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER4_1_RY_ANGLE, //243 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER4_2_RY_ANGLE, //244 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER4_3_RY_ANGLE, //245 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER3_1_RY_ANGLE, //246 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER3_2_RY_ANGLE, //247 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER3_3_RY_ANGLE, //248 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER2_1_RY_ANGLE, //249 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_11, //250 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER2_3_RY_ANGLE, //251 
 MNET_RHAND_IN_FINGER2_2_RY_RTHUMBY_ANGLE, //252 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER1_2_RY_ANGLE, //253 
 MNET_RHAND_IN_FINGER2_2_RY_FINGER1_3_RY_ANGLE, //254 
 MNET_RHAND_IN_FINGER2_3_RY_RHANDY_ANGLE, //255 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER5_1_RY_ANGLE, //256 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER5_2_RY_ANGLE, //257 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER5_3_RY_ANGLE, //258 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER4_1_RY_ANGLE, //259 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER4_2_RY_ANGLE, //260 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER4_3_RY_ANGLE, //261 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER3_1_RY_ANGLE, //262 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER3_2_RY_ANGLE, //263 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER3_3_RY_ANGLE, //264 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER2_1_RY_ANGLE, //265 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER2_2_RY_ANGLE, //266 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_12, //267 
 MNET_RHAND_IN_FINGER2_3_RY_RTHUMBY_ANGLE, //268 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER1_2_RY_ANGLE, //269 
 MNET_RHAND_IN_FINGER2_3_RY_FINGER1_3_RY_ANGLE, //270 
 MNET_RHAND_IN_RTHUMBY_RHANDY_ANGLE, //271 
 MNET_RHAND_IN_RTHUMBY_FINGER5_1_RY_ANGLE, //272 
 MNET_RHAND_IN_RTHUMBY_FINGER5_2_RY_ANGLE, //273 
 MNET_RHAND_IN_RTHUMBY_FINGER5_3_RY_ANGLE, //274 
 MNET_RHAND_IN_RTHUMBY_FINGER4_1_RY_ANGLE, //275 
 MNET_RHAND_IN_RTHUMBY_FINGER4_2_RY_ANGLE, //276 
 MNET_RHAND_IN_RTHUMBY_FINGER4_3_RY_ANGLE, //277 
 MNET_RHAND_IN_RTHUMBY_FINGER3_1_RY_ANGLE, //278 
 MNET_RHAND_IN_RTHUMBY_FINGER3_2_RY_ANGLE, //279 
 MNET_RHAND_IN_RTHUMBY_FINGER3_3_RY_ANGLE, //280 
 MNET_RHAND_IN_RTHUMBY_FINGER2_1_RY_ANGLE, //281 
 MNET_RHAND_IN_RTHUMBY_FINGER2_2_RY_ANGLE, //282 
 MNET_RHAND_IN_RTHUMBY_FINGER2_3_RY_ANGLE, //283 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_13, //284 
 MNET_RHAND_IN_RTHUMBY_FINGER1_2_RY_ANGLE, //285 
 MNET_RHAND_IN_RTHUMBY_FINGER1_3_RY_ANGLE, //286 
 MNET_RHAND_IN_FINGER1_2_RY_RHANDY_ANGLE, //287 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER5_1_RY_ANGLE, //288 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER5_2_RY_ANGLE, //289 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER5_3_RY_ANGLE, //290 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER4_1_RY_ANGLE, //291 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER4_2_RY_ANGLE, //292 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER4_3_RY_ANGLE, //293 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER3_1_RY_ANGLE, //294 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER3_2_RY_ANGLE, //295 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER3_3_RY_ANGLE, //296 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER2_1_RY_ANGLE, //297 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER2_2_RY_ANGLE, //298 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER2_3_RY_ANGLE, //299 
 MNET_RHAND_IN_FINGER1_2_RY_RTHUMBY_ANGLE, //300 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_14, //301 
 MNET_RHAND_IN_FINGER1_2_RY_FINGER1_3_RY_ANGLE, //302 
 MNET_RHAND_IN_FINGER1_3_RY_RHANDY_ANGLE, //303 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER5_1_RY_ANGLE, //304 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER5_2_RY_ANGLE, //305 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER5_3_RY_ANGLE, //306 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER4_1_RY_ANGLE, //307 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER4_2_RY_ANGLE, //308 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER4_3_RY_ANGLE, //309 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER3_1_RY_ANGLE, //310 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER3_2_RY_ANGLE, //311 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER3_3_RY_ANGLE, //312 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER2_1_RY_ANGLE, //313 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER2_2_RY_ANGLE, //314 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER2_3_RY_ANGLE, //315 
 MNET_RHAND_IN_FINGER1_3_RY_RTHUMBY_ANGLE, //316 
 MNET_RHAND_IN_FINGER1_3_RY_FINGER1_2_RY_ANGLE, //317 
 MNET_RHAND_IN_ANGLEUSEDFOR2DROTATION_15, //318 
 MNET_RHAND_IN_NUMBER
};

/** @brief Programmer friendly enumerator of expected outputs
 TODO: CAREFULL!*/
enum mocapNET_Output_rhand_enum 
{
 MOCAPNET_RHAND_OUTPUT_RHAND_XPOSITION = 0, //0 
 MOCAPNET_RHAND_OUTPUT_RHAND_YPOSITION, //1 
 MOCAPNET_RHAND_OUTPUT_RHAND_ZPOSITION, //2 
 MOCAPNET_RHAND_OUTPUT_RHAND_WROTATION, //3 
 MOCAPNET_RHAND_OUTPUT_RHAND_XROTATION, //4 
 MOCAPNET_RHAND_OUTPUT_RHAND_YROTATION, //5 
 MOCAPNET_RHAND_OUTPUT_RHAND_ZROTATION, //6 
 MOCAPNET_RHAND_OUTPUT_FINGER2_1_R_ZROTATION, //7 
 MOCAPNET_RHAND_OUTPUT_FINGER2_1_R_XROTATION, //8 
 MOCAPNET_RHAND_OUTPUT_FINGER2_1_R_YROTATION, //9 
 MOCAPNET_RHAND_OUTPUT_FINGER2_2_R_ZROTATION, //10 
 MOCAPNET_RHAND_OUTPUT_FINGER2_2_R_XROTATION, //11 
 MOCAPNET_RHAND_OUTPUT_FINGER2_2_R_YROTATION, //12 
 MOCAPNET_RHAND_OUTPUT_FINGER2_3_R_ZROTATION, //13 
 MOCAPNET_RHAND_OUTPUT_FINGER2_3_R_XROTATION, //14 
 MOCAPNET_RHAND_OUTPUT_FINGER2_3_R_YROTATION, //15 
 MOCAPNET_RHAND_OUTPUT_FINGER3_1_R_ZROTATION, //16 
 MOCAPNET_RHAND_OUTPUT_FINGER3_1_R_XROTATION, //17 
 MOCAPNET_RHAND_OUTPUT_FINGER3_1_R_YROTATION, //18 
 MOCAPNET_RHAND_OUTPUT_FINGER3_2_R_ZROTATION, //19 
 MOCAPNET_RHAND_OUTPUT_FINGER3_2_R_XROTATION, //20 
 MOCAPNET_RHAND_OUTPUT_FINGER3_2_R_YROTATION, //21 
 MOCAPNET_RHAND_OUTPUT_FINGER3_3_R_ZROTATION, //22 
 MOCAPNET_RHAND_OUTPUT_FINGER3_3_R_XROTATION, //23 
 MOCAPNET_RHAND_OUTPUT_FINGER3_3_R_YROTATION, //24 
 MOCAPNET_RHAND_OUTPUT_FINGER4_1_R_ZROTATION, //25 
 MOCAPNET_RHAND_OUTPUT_FINGER4_1_R_XROTATION, //26 
 MOCAPNET_RHAND_OUTPUT_FINGER4_1_R_YROTATION, //27 
 MOCAPNET_RHAND_OUTPUT_FINGER4_2_R_ZROTATION, //28 
 MOCAPNET_RHAND_OUTPUT_FINGER4_2_R_XROTATION, //29 
 MOCAPNET_RHAND_OUTPUT_FINGER4_2_R_YROTATION, //30 
 MOCAPNET_RHAND_OUTPUT_FINGER4_3_R_ZROTATION, //31 
 MOCAPNET_RHAND_OUTPUT_FINGER4_3_R_XROTATION, //32 
 MOCAPNET_RHAND_OUTPUT_FINGER4_3_R_YROTATION, //33 
 MOCAPNET_RHAND_OUTPUT_FINGER5_1_R_ZROTATION, //34 
 MOCAPNET_RHAND_OUTPUT_FINGER5_1_R_XROTATION, //35 
 MOCAPNET_RHAND_OUTPUT_FINGER5_1_R_YROTATION, //36 
 MOCAPNET_RHAND_OUTPUT_FINGER5_2_R_ZROTATION, //37 
 MOCAPNET_RHAND_OUTPUT_FINGER5_2_R_XROTATION, //38 
 MOCAPNET_RHAND_OUTPUT_FINGER5_2_R_YROTATION, //39 
 MOCAPNET_RHAND_OUTPUT_FINGER5_3_R_ZROTATION, //40 
 MOCAPNET_RHAND_OUTPUT_FINGER5_3_R_XROTATION, //41 
 MOCAPNET_RHAND_OUTPUT_FINGER5_3_R_YROTATION, //42 
 MOCAPNET_RHAND_OUTPUT_RTHUMBBASE_ZROTATION, //43 
 MOCAPNET_RHAND_OUTPUT_RTHUMBBASE_XROTATION, //44 
 MOCAPNET_RHAND_OUTPUT_RTHUMBBASE_YROTATION, //45 
 MOCAPNET_RHAND_OUTPUT_RTHUMB_ZROTATION, //46 
 MOCAPNET_RHAND_OUTPUT_RTHUMB_XROTATION, //47 
 MOCAPNET_RHAND_OUTPUT_RTHUMB_YROTATION, //48 
 MOCAPNET_RHAND_OUTPUT_FINGER1_2_R_ZROTATION, //49 
 MOCAPNET_RHAND_OUTPUT_FINGER1_2_R_XROTATION, //50 
 MOCAPNET_RHAND_OUTPUT_FINGER1_2_R_YROTATION, //51 
 MOCAPNET_RHAND_OUTPUT_FINGER1_3_R_ZROTATION, //52 
 MOCAPNET_RHAND_OUTPUT_FINGER1_3_R_XROTATION, //53 
 MOCAPNET_RHAND_OUTPUT_FINGER1_3_R_YROTATION, //54 
 MOCAPNET_RHAND_OUTPUT_NUMBER
};

/** @brief Programmer friendly enumerator of NSDM elments*/
enum mocapNET_NSDM_rhand_enum 
{
 MNET_NSDM_RHAND_RHAND = 0, //0 
 MNET_NSDM_RHAND_FINGER5_1_R, //1 
 MNET_NSDM_RHAND_FINGER5_2_R, //2 
 MNET_NSDM_RHAND_FINGER5_3_R, //3 
 MNET_NSDM_RHAND_FINGER4_1_R, //4 
 MNET_NSDM_RHAND_FINGER4_2_R, //5 
 MNET_NSDM_RHAND_FINGER4_3_R, //6 
 MNET_NSDM_RHAND_FINGER3_1_R, //7 
 MNET_NSDM_RHAND_FINGER3_2_R, //8 
 MNET_NSDM_RHAND_FINGER3_3_R, //9 
 MNET_NSDM_RHAND_FINGER2_1_R, //10 
 MNET_NSDM_RHAND_FINGER2_2_R, //11 
 MNET_NSDM_RHAND_FINGER2_3_R, //12 
 MNET_NSDM_RHAND_RTHUMB, //13 
 MNET_NSDM_RHAND_FINGER1_2_R, //14 
 MNET_NSDM_RHAND_FINGER1_3_R, //15 
 MNET_NSDM_RHAND_NUMBER
};

/** @brief This is a lookup table to immediately resolve referred Joints*/
static const int mocapNET_ResolveJoint_rhand[] = 
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
static const int mocapNET_ResolveSecondTargetJoint_rhand[] = 
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
static const int mocapNET_ArtificialJoint_rhand[] = 
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

/** @brief These are X offsets for artificial joints of type 1 ( see mocapNET_ArtificialJoint_rhand )*/
static const float mocapNET_ArtificialJointXOffset_rhand[] = 
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

/** @brief These are Y offsets for artificial joints of type 1 ( see mocapNET_ArtificialJoint_rhand )*/
static const float mocapNET_ArtificialJointYOffset_rhand[] = 
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
static const int mocapNET_ScalingStart_rhand[] = 
{
 0, //0 
 0, //1 
 0, //2 
 0, //3 
 0, //4 
 0//end of array
};

/** @brief These are 2D Joints that are used as ending points for scaling vectors*/
static const int mocapNET_ScalingEnd_rhand[] = 
{
 13, //0 
 9, //1 
 5, //2 
 1, //3 
 17, //4 
 0//end of array
};

/** @brief These is a 2D Joints that is used as alignment for the skeleton*/
static const int mocapNET_AlignmentStart_rhand[] = 
{
 0, //0 
 0//end of array
};

/** @brief These is a 2D Joints that is used as alignment for the skeleton*/
static const int mocapNET_AlignmentEnd_rhand[] = 
{
 5, //0 
 0//end of array
};

/** @brief This function can be used to debug NSDM input and find in a user friendly what is missing..!*/
static int rhandCountMissingNSDMElements(std::vector<float> mocapNETInput,int verbose)
{
    unsigned int numberOfZeros=0;
    for (int i=0; i<mocapNETInput.size(); i++)
        {
            if (mocapNETInput[i]==0)
                {
                   if (verbose) { fprintf(stderr,"rhand NSDM %s missing\n",mocapNET_rhand[i]); }
                    ++numberOfZeros;
                }
        }
    return numberOfZeros;
}

/** @brief Another debug function to make sure the joint order is correctly resolved..!*/
static void rhandDebugNSDMJointOrder(char ** labels)
{ 
    for (int i=0; i<MNET_NSDM_RHAND_NUMBER; i++)
        {
            fprintf(stderr,"NSDM rhand %s => skeletonSerialized %s\n ",mocapNET_rhand[i],labels[i]);       
        }
}

/** @brief This function returns the euclidean distance between two input 2D joints and zero if either of them is invalid*/
static float getJoint2DDistance_RHAND(std::vector<float> in,int jointA,int jointB)
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

/** @brief This function returns a vector of NSDM values ready for use with the MocapNET rhand network */
static std::vector<float> rhandCreateNDSM(std::vector<float> in,float alignmentAngle2D,int havePositionalElements,int haveAngularElements,int doNormalization,int eNSRM)
 {
    std::vector<float> result;
    for (int i=0; i<MNET_NSDM_RHAND_NUMBER; i++)
    {
     //------------------------------------
     unsigned int iJointID=mocapNET_ResolveJoint_rhand[i];
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
         for (int j=0; j<MNET_NSDM_RHAND_NUMBER; j++)
                      {
                          if (havePositionalElements) { result.push_back(0.0); result.push_back(0.0); } 
                          if (haveAngularElements)    { result.push_back(0.0); } 
                      }
         invalidPointI=1;
       } else 
       {  
     for (int j=0; j<MNET_NSDM_RHAND_NUMBER; j++)
     {
       jJointID=mocapNET_ResolveJoint_rhand[j];
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
             unsigned int iJointID=mocapNET_ScalingStart_rhand[i];
             unsigned int jJointID=mocapNET_ScalingEnd_rhand[i];
             float distance = getJoint2DDistance_RHAND(in,iJointID,jJointID);
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


             unsigned int iJointID=mocapNET_AlignmentStart_rhand[0];
             unsigned int jJointID=mocapNET_AlignmentEnd_rhand[0];
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
unsigned int firstJointID=mocapNET_ResolveJoint_rhand[0];
for (unsigned int i=0; i<MNET_NSDM_RHAND_NUMBER; i++)
{
for (unsigned int j=0; j<MNET_NSDM_RHAND_NUMBER; j++)
{
if (i==j)
{
  result[elementID]=alignmentAngle2D;
  //Rest diagonal elements will encode euclidean distances from base joint
  //if richDiagonal_rhand flag is set.. 
  if ( (i>0) && (richDiagonal_rhand) )
      {  
        unsigned int jJointID=mocapNET_ResolveJoint_rhand[j];  
        result[elementID]=getJoint2DDistance_RHAND(in,firstJointID,jJointID); 
      }
}
elementID+=1;
}
}
}

 return result;
 }

/** @file generated_lowerbody.hpp
 *  @brief A Description the lowerbody input of a Tensorflow network required for MocapNET
 *  @author Ammar Qammaz (AmmarkoV)
 *  Automatically generated using :
 *     python3 exportCPPCodeFromJSONConfiguration.py --front lowerbody --config dataset/lowerbody_configuration.json
 *     please note that since the names of the labels are both affected by the dataset/lowerbody_configuration.json configuration
 *     as well as the ground truth, if you have made any weird additions you might consider running the ./createRandomizedDataset.sh and ./createTestDataset.sh scripts
 */

#pragma once

#include <iostream>
#include <math.h>
#include <vector>
#include "../tools.hpp"

/** @brief This is an array of names for all uncompressed 2D inputs expected. */
static const unsigned int mocapNET_InputLength_WithoutNSDM_lowerbody = 36;

/** @brief An array of strings that contains the label for each expected input. */
static const char * mocapNET_lowerbody[] =
{
    "2DX_hip", //0
    "2DY_hip", //1
    "visible_hip", //2
    "2DX_neck", //3
    "2DY_neck", //4
    "visible_neck", //5
    "2DX_rhip", //6
    "2DY_rhip", //7
    "visible_rhip", //8
    "2DX_rknee", //9
    "2DY_rknee", //10
    "visible_rknee", //11
    "2DX_rfoot", //12
    "2DY_rfoot", //13
    "visible_rfoot", //14
    "2DX_EndSite_toe1-2.r", //15
    "2DY_EndSite_toe1-2.r", //16
    "visible_EndSite_toe1-2.r", //17
    "2DX_EndSite_toe5-3.r", //18
    "2DY_EndSite_toe5-3.r", //19
    "visible_EndSite_toe5-3.r", //20
    "2DX_lhip", //21
    "2DY_lhip", //22
    "visible_lhip", //23
    "2DX_lknee", //24
    "2DY_lknee", //25
    "visible_lknee", //26
    "2DX_lfoot", //27
    "2DY_lfoot", //28
    "visible_lfoot", //29
    "2DX_EndSite_toe1-2.l", //30
    "2DY_EndSite_toe1-2.l", //31
    "visible_EndSite_toe1-2.l", //32
    "2DX_EndSite_toe5-3.l", //33
    "2DY_EndSite_toe5-3.l", //34
    "visible_EndSite_toe5-3.l", //35
//This is where regular input ends and the NSDM data kicks in..
    "hipY-hipY-Angle", //36
    "hipY-rhipY-Angle", //37
    "hipY-halfway_rhip_and_rkneeY-Angle", //38
    "hipY-rkneeY-Angle", //39
    "hipY-halfway_rknee_and_rfootY-Angle", //40
    "hipY-rfootY-Angle", //41
    "hipY-EndSite_toe1-2.rY-Angle", //42
    "hipY-virtual_hip_x_plus0_3_y_0Y-Angle", //43
    "hipY-virtual_hip_x_minus_0_3_y_0Y-Angle", //44
    "hipY-lhipY-Angle", //45
    "hipY-halfway_lhip_and_lkneeY-Angle", //46
    "hipY-lkneeY-Angle", //47
    "hipY-halfway_lknee_and_lfootY-Angle", //48
    "hipY-lfootY-Angle", //49
    "hipY-EndSite_toe1-2.lY-Angle", //50
    "hipY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //51
    "hipY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //52
    "rhipY-hipY-Angle", //53
    "rhipY-rhipY-Angle", //54
    "rhipY-halfway_rhip_and_rkneeY-Angle", //55
    "rhipY-rkneeY-Angle", //56
    "rhipY-halfway_rknee_and_rfootY-Angle", //57
    "rhipY-rfootY-Angle", //58
    "rhipY-EndSite_toe1-2.rY-Angle", //59
    "rhipY-virtual_hip_x_plus0_3_y_0Y-Angle", //60
    "rhipY-virtual_hip_x_minus_0_3_y_0Y-Angle", //61
    "rhipY-lhipY-Angle", //62
    "rhipY-halfway_lhip_and_lkneeY-Angle", //63
    "rhipY-lkneeY-Angle", //64
    "rhipY-halfway_lknee_and_lfootY-Angle", //65
    "rhipY-lfootY-Angle", //66
    "rhipY-EndSite_toe1-2.lY-Angle", //67
    "rhipY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //68
    "rhipY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //69
    "halfway_rhip_and_rkneeY-hipY-Angle", //70
    "halfway_rhip_and_rkneeY-rhipY-Angle", //71
    "halfway_rhip_and_rkneeY-halfway_rhip_and_rkneeY-Angle", //72
    "halfway_rhip_and_rkneeY-rkneeY-Angle", //73
    "halfway_rhip_and_rkneeY-halfway_rknee_and_rfootY-Angle", //74
    "halfway_rhip_and_rkneeY-rfootY-Angle", //75
    "halfway_rhip_and_rkneeY-EndSite_toe1-2.rY-Angle", //76
    "halfway_rhip_and_rkneeY-virtual_hip_x_plus0_3_y_0Y-Angle", //77
    "halfway_rhip_and_rkneeY-virtual_hip_x_minus_0_3_y_0Y-Angle", //78
    "halfway_rhip_and_rkneeY-lhipY-Angle", //79
    "halfway_rhip_and_rkneeY-halfway_lhip_and_lkneeY-Angle", //80
    "halfway_rhip_and_rkneeY-lkneeY-Angle", //81
    "halfway_rhip_and_rkneeY-halfway_lknee_and_lfootY-Angle", //82
    "halfway_rhip_and_rkneeY-lfootY-Angle", //83
    "halfway_rhip_and_rkneeY-EndSite_toe1-2.lY-Angle", //84
    "halfway_rhip_and_rkneeY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //85
    "halfway_rhip_and_rkneeY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //86
    "rkneeY-hipY-Angle", //87
    "rkneeY-rhipY-Angle", //88
    "rkneeY-halfway_rhip_and_rkneeY-Angle", //89
    "rkneeY-rkneeY-Angle", //90
    "rkneeY-halfway_rknee_and_rfootY-Angle", //91
    "rkneeY-rfootY-Angle", //92
    "rkneeY-EndSite_toe1-2.rY-Angle", //93
    "rkneeY-virtual_hip_x_plus0_3_y_0Y-Angle", //94
    "rkneeY-virtual_hip_x_minus_0_3_y_0Y-Angle", //95
    "rkneeY-lhipY-Angle", //96
    "rkneeY-halfway_lhip_and_lkneeY-Angle", //97
    "rkneeY-lkneeY-Angle", //98
    "rkneeY-halfway_lknee_and_lfootY-Angle", //99
    "rkneeY-lfootY-Angle", //100
    "rkneeY-EndSite_toe1-2.lY-Angle", //101
    "rkneeY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //102
    "rkneeY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //103
    "halfway_rknee_and_rfootY-hipY-Angle", //104
    "halfway_rknee_and_rfootY-rhipY-Angle", //105
    "halfway_rknee_and_rfootY-halfway_rhip_and_rkneeY-Angle", //106
    "halfway_rknee_and_rfootY-rkneeY-Angle", //107
    "halfway_rknee_and_rfootY-halfway_rknee_and_rfootY-Angle", //108
    "halfway_rknee_and_rfootY-rfootY-Angle", //109
    "halfway_rknee_and_rfootY-EndSite_toe1-2.rY-Angle", //110
    "halfway_rknee_and_rfootY-virtual_hip_x_plus0_3_y_0Y-Angle", //111
    "halfway_rknee_and_rfootY-virtual_hip_x_minus_0_3_y_0Y-Angle", //112
    "halfway_rknee_and_rfootY-lhipY-Angle", //113
    "halfway_rknee_and_rfootY-halfway_lhip_and_lkneeY-Angle", //114
    "halfway_rknee_and_rfootY-lkneeY-Angle", //115
    "halfway_rknee_and_rfootY-halfway_lknee_and_lfootY-Angle", //116
    "halfway_rknee_and_rfootY-lfootY-Angle", //117
    "halfway_rknee_and_rfootY-EndSite_toe1-2.lY-Angle", //118
    "halfway_rknee_and_rfootY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //119
    "halfway_rknee_and_rfootY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //120
    "rfootY-hipY-Angle", //121
    "rfootY-rhipY-Angle", //122
    "rfootY-halfway_rhip_and_rkneeY-Angle", //123
    "rfootY-rkneeY-Angle", //124
    "rfootY-halfway_rknee_and_rfootY-Angle", //125
    "rfootY-rfootY-Angle", //126
    "rfootY-EndSite_toe1-2.rY-Angle", //127
    "rfootY-virtual_hip_x_plus0_3_y_0Y-Angle", //128
    "rfootY-virtual_hip_x_minus_0_3_y_0Y-Angle", //129
    "rfootY-lhipY-Angle", //130
    "rfootY-halfway_lhip_and_lkneeY-Angle", //131
    "rfootY-lkneeY-Angle", //132
    "rfootY-halfway_lknee_and_lfootY-Angle", //133
    "rfootY-lfootY-Angle", //134
    "rfootY-EndSite_toe1-2.lY-Angle", //135
    "rfootY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //136
    "rfootY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //137
    "EndSite_toe1-2.rY-hipY-Angle", //138
    "EndSite_toe1-2.rY-rhipY-Angle", //139
    "EndSite_toe1-2.rY-halfway_rhip_and_rkneeY-Angle", //140
    "EndSite_toe1-2.rY-rkneeY-Angle", //141
    "EndSite_toe1-2.rY-halfway_rknee_and_rfootY-Angle", //142
    "EndSite_toe1-2.rY-rfootY-Angle", //143
    "EndSite_toe1-2.rY-EndSite_toe1-2.rY-Angle", //144
    "EndSite_toe1-2.rY-virtual_hip_x_plus0_3_y_0Y-Angle", //145
    "EndSite_toe1-2.rY-virtual_hip_x_minus_0_3_y_0Y-Angle", //146
    "EndSite_toe1-2.rY-lhipY-Angle", //147
    "EndSite_toe1-2.rY-halfway_lhip_and_lkneeY-Angle", //148
    "EndSite_toe1-2.rY-lkneeY-Angle", //149
    "EndSite_toe1-2.rY-halfway_lknee_and_lfootY-Angle", //150
    "EndSite_toe1-2.rY-lfootY-Angle", //151
    "EndSite_toe1-2.rY-EndSite_toe1-2.lY-Angle", //152
    "EndSite_toe1-2.rY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //153
    "EndSite_toe1-2.rY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //154
    "virtual_hip_x_plus0_3_y_0Y-hipY-Angle", //155
    "virtual_hip_x_plus0_3_y_0Y-rhipY-Angle", //156
    "virtual_hip_x_plus0_3_y_0Y-halfway_rhip_and_rkneeY-Angle", //157
    "virtual_hip_x_plus0_3_y_0Y-rkneeY-Angle", //158
    "virtual_hip_x_plus0_3_y_0Y-halfway_rknee_and_rfootY-Angle", //159
    "virtual_hip_x_plus0_3_y_0Y-rfootY-Angle", //160
    "virtual_hip_x_plus0_3_y_0Y-EndSite_toe1-2.rY-Angle", //161
    "virtual_hip_x_plus0_3_y_0Y-virtual_hip_x_plus0_3_y_0Y-Angle", //162
    "virtual_hip_x_plus0_3_y_0Y-virtual_hip_x_minus_0_3_y_0Y-Angle", //163
    "virtual_hip_x_plus0_3_y_0Y-lhipY-Angle", //164
    "virtual_hip_x_plus0_3_y_0Y-halfway_lhip_and_lkneeY-Angle", //165
    "virtual_hip_x_plus0_3_y_0Y-lkneeY-Angle", //166
    "virtual_hip_x_plus0_3_y_0Y-halfway_lknee_and_lfootY-Angle", //167
    "virtual_hip_x_plus0_3_y_0Y-lfootY-Angle", //168
    "virtual_hip_x_plus0_3_y_0Y-EndSite_toe1-2.lY-Angle", //169
    "virtual_hip_x_plus0_3_y_0Y-virtual_hip_x_0_0_y_plus0_15Y-Angle", //170
    "virtual_hip_x_plus0_3_y_0Y-virtual_hip_x_0_0_y_plus0_3Y-Angle", //171
    "virtual_hip_x_minus_0_3_y_0Y-hipY-Angle", //172
    "virtual_hip_x_minus_0_3_y_0Y-rhipY-Angle", //173
    "virtual_hip_x_minus_0_3_y_0Y-halfway_rhip_and_rkneeY-Angle", //174
    "virtual_hip_x_minus_0_3_y_0Y-rkneeY-Angle", //175
    "virtual_hip_x_minus_0_3_y_0Y-halfway_rknee_and_rfootY-Angle", //176
    "virtual_hip_x_minus_0_3_y_0Y-rfootY-Angle", //177
    "virtual_hip_x_minus_0_3_y_0Y-EndSite_toe1-2.rY-Angle", //178
    "virtual_hip_x_minus_0_3_y_0Y-virtual_hip_x_plus0_3_y_0Y-Angle", //179
    "virtual_hip_x_minus_0_3_y_0Y-virtual_hip_x_minus_0_3_y_0Y-Angle", //180
    "virtual_hip_x_minus_0_3_y_0Y-lhipY-Angle", //181
    "virtual_hip_x_minus_0_3_y_0Y-halfway_lhip_and_lkneeY-Angle", //182
    "virtual_hip_x_minus_0_3_y_0Y-lkneeY-Angle", //183
    "virtual_hip_x_minus_0_3_y_0Y-halfway_lknee_and_lfootY-Angle", //184
    "virtual_hip_x_minus_0_3_y_0Y-lfootY-Angle", //185
    "virtual_hip_x_minus_0_3_y_0Y-EndSite_toe1-2.lY-Angle", //186
    "virtual_hip_x_minus_0_3_y_0Y-virtual_hip_x_0_0_y_plus0_15Y-Angle", //187
    "virtual_hip_x_minus_0_3_y_0Y-virtual_hip_x_0_0_y_plus0_3Y-Angle", //188
    "lhipY-hipY-Angle", //189
    "lhipY-rhipY-Angle", //190
    "lhipY-halfway_rhip_and_rkneeY-Angle", //191
    "lhipY-rkneeY-Angle", //192
    "lhipY-halfway_rknee_and_rfootY-Angle", //193
    "lhipY-rfootY-Angle", //194
    "lhipY-EndSite_toe1-2.rY-Angle", //195
    "lhipY-virtual_hip_x_plus0_3_y_0Y-Angle", //196
    "lhipY-virtual_hip_x_minus_0_3_y_0Y-Angle", //197
    "lhipY-lhipY-Angle", //198
    "lhipY-halfway_lhip_and_lkneeY-Angle", //199
    "lhipY-lkneeY-Angle", //200
    "lhipY-halfway_lknee_and_lfootY-Angle", //201
    "lhipY-lfootY-Angle", //202
    "lhipY-EndSite_toe1-2.lY-Angle", //203
    "lhipY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //204
    "lhipY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //205
    "halfway_lhip_and_lkneeY-hipY-Angle", //206
    "halfway_lhip_and_lkneeY-rhipY-Angle", //207
    "halfway_lhip_and_lkneeY-halfway_rhip_and_rkneeY-Angle", //208
    "halfway_lhip_and_lkneeY-rkneeY-Angle", //209
    "halfway_lhip_and_lkneeY-halfway_rknee_and_rfootY-Angle", //210
    "halfway_lhip_and_lkneeY-rfootY-Angle", //211
    "halfway_lhip_and_lkneeY-EndSite_toe1-2.rY-Angle", //212
    "halfway_lhip_and_lkneeY-virtual_hip_x_plus0_3_y_0Y-Angle", //213
    "halfway_lhip_and_lkneeY-virtual_hip_x_minus_0_3_y_0Y-Angle", //214
    "halfway_lhip_and_lkneeY-lhipY-Angle", //215
    "halfway_lhip_and_lkneeY-halfway_lhip_and_lkneeY-Angle", //216
    "halfway_lhip_and_lkneeY-lkneeY-Angle", //217
    "halfway_lhip_and_lkneeY-halfway_lknee_and_lfootY-Angle", //218
    "halfway_lhip_and_lkneeY-lfootY-Angle", //219
    "halfway_lhip_and_lkneeY-EndSite_toe1-2.lY-Angle", //220
    "halfway_lhip_and_lkneeY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //221
    "halfway_lhip_and_lkneeY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //222
    "lkneeY-hipY-Angle", //223
    "lkneeY-rhipY-Angle", //224
    "lkneeY-halfway_rhip_and_rkneeY-Angle", //225
    "lkneeY-rkneeY-Angle", //226
    "lkneeY-halfway_rknee_and_rfootY-Angle", //227
    "lkneeY-rfootY-Angle", //228
    "lkneeY-EndSite_toe1-2.rY-Angle", //229
    "lkneeY-virtual_hip_x_plus0_3_y_0Y-Angle", //230
    "lkneeY-virtual_hip_x_minus_0_3_y_0Y-Angle", //231
    "lkneeY-lhipY-Angle", //232
    "lkneeY-halfway_lhip_and_lkneeY-Angle", //233
    "lkneeY-lkneeY-Angle", //234
    "lkneeY-halfway_lknee_and_lfootY-Angle", //235
    "lkneeY-lfootY-Angle", //236
    "lkneeY-EndSite_toe1-2.lY-Angle", //237
    "lkneeY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //238
    "lkneeY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //239
    "halfway_lknee_and_lfootY-hipY-Angle", //240
    "halfway_lknee_and_lfootY-rhipY-Angle", //241
    "halfway_lknee_and_lfootY-halfway_rhip_and_rkneeY-Angle", //242
    "halfway_lknee_and_lfootY-rkneeY-Angle", //243
    "halfway_lknee_and_lfootY-halfway_rknee_and_rfootY-Angle", //244
    "halfway_lknee_and_lfootY-rfootY-Angle", //245
    "halfway_lknee_and_lfootY-EndSite_toe1-2.rY-Angle", //246
    "halfway_lknee_and_lfootY-virtual_hip_x_plus0_3_y_0Y-Angle", //247
    "halfway_lknee_and_lfootY-virtual_hip_x_minus_0_3_y_0Y-Angle", //248
    "halfway_lknee_and_lfootY-lhipY-Angle", //249
    "halfway_lknee_and_lfootY-halfway_lhip_and_lkneeY-Angle", //250
    "halfway_lknee_and_lfootY-lkneeY-Angle", //251
    "halfway_lknee_and_lfootY-halfway_lknee_and_lfootY-Angle", //252
    "halfway_lknee_and_lfootY-lfootY-Angle", //253
    "halfway_lknee_and_lfootY-EndSite_toe1-2.lY-Angle", //254
    "halfway_lknee_and_lfootY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //255
    "halfway_lknee_and_lfootY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //256
    "lfootY-hipY-Angle", //257
    "lfootY-rhipY-Angle", //258
    "lfootY-halfway_rhip_and_rkneeY-Angle", //259
    "lfootY-rkneeY-Angle", //260
    "lfootY-halfway_rknee_and_rfootY-Angle", //261
    "lfootY-rfootY-Angle", //262
    "lfootY-EndSite_toe1-2.rY-Angle", //263
    "lfootY-virtual_hip_x_plus0_3_y_0Y-Angle", //264
    "lfootY-virtual_hip_x_minus_0_3_y_0Y-Angle", //265
    "lfootY-lhipY-Angle", //266
    "lfootY-halfway_lhip_and_lkneeY-Angle", //267
    "lfootY-lkneeY-Angle", //268
    "lfootY-halfway_lknee_and_lfootY-Angle", //269
    "lfootY-lfootY-Angle", //270
    "lfootY-EndSite_toe1-2.lY-Angle", //271
    "lfootY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //272
    "lfootY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //273
    "EndSite_toe1-2.lY-hipY-Angle", //274
    "EndSite_toe1-2.lY-rhipY-Angle", //275
    "EndSite_toe1-2.lY-halfway_rhip_and_rkneeY-Angle", //276
    "EndSite_toe1-2.lY-rkneeY-Angle", //277
    "EndSite_toe1-2.lY-halfway_rknee_and_rfootY-Angle", //278
    "EndSite_toe1-2.lY-rfootY-Angle", //279
    "EndSite_toe1-2.lY-EndSite_toe1-2.rY-Angle", //280
    "EndSite_toe1-2.lY-virtual_hip_x_plus0_3_y_0Y-Angle", //281
    "EndSite_toe1-2.lY-virtual_hip_x_minus_0_3_y_0Y-Angle", //282
    "EndSite_toe1-2.lY-lhipY-Angle", //283
    "EndSite_toe1-2.lY-halfway_lhip_and_lkneeY-Angle", //284
    "EndSite_toe1-2.lY-lkneeY-Angle", //285
    "EndSite_toe1-2.lY-halfway_lknee_and_lfootY-Angle", //286
    "EndSite_toe1-2.lY-lfootY-Angle", //287
    "EndSite_toe1-2.lY-EndSite_toe1-2.lY-Angle", //288
    "EndSite_toe1-2.lY-virtual_hip_x_0_0_y_plus0_15Y-Angle", //289
    "EndSite_toe1-2.lY-virtual_hip_x_0_0_y_plus0_3Y-Angle", //290
    "virtual_hip_x_0_0_y_plus0_15Y-hipY-Angle", //291
    "virtual_hip_x_0_0_y_plus0_15Y-rhipY-Angle", //292
    "virtual_hip_x_0_0_y_plus0_15Y-halfway_rhip_and_rkneeY-Angle", //293
    "virtual_hip_x_0_0_y_plus0_15Y-rkneeY-Angle", //294
    "virtual_hip_x_0_0_y_plus0_15Y-halfway_rknee_and_rfootY-Angle", //295
    "virtual_hip_x_0_0_y_plus0_15Y-rfootY-Angle", //296
    "virtual_hip_x_0_0_y_plus0_15Y-EndSite_toe1-2.rY-Angle", //297
    "virtual_hip_x_0_0_y_plus0_15Y-virtual_hip_x_plus0_3_y_0Y-Angle", //298
    "virtual_hip_x_0_0_y_plus0_15Y-virtual_hip_x_minus_0_3_y_0Y-Angle", //299
    "virtual_hip_x_0_0_y_plus0_15Y-lhipY-Angle", //300
    "virtual_hip_x_0_0_y_plus0_15Y-halfway_lhip_and_lkneeY-Angle", //301
    "virtual_hip_x_0_0_y_plus0_15Y-lkneeY-Angle", //302
    "virtual_hip_x_0_0_y_plus0_15Y-halfway_lknee_and_lfootY-Angle", //303
    "virtual_hip_x_0_0_y_plus0_15Y-lfootY-Angle", //304
    "virtual_hip_x_0_0_y_plus0_15Y-EndSite_toe1-2.lY-Angle", //305
    "virtual_hip_x_0_0_y_plus0_15Y-virtual_hip_x_0_0_y_plus0_15Y-Angle", //306
    "virtual_hip_x_0_0_y_plus0_15Y-virtual_hip_x_0_0_y_plus0_3Y-Angle", //307
    "virtual_hip_x_0_0_y_plus0_3Y-hipY-Angle", //308
    "virtual_hip_x_0_0_y_plus0_3Y-rhipY-Angle", //309
    "virtual_hip_x_0_0_y_plus0_3Y-halfway_rhip_and_rkneeY-Angle", //310
    "virtual_hip_x_0_0_y_plus0_3Y-rkneeY-Angle", //311
    "virtual_hip_x_0_0_y_plus0_3Y-halfway_rknee_and_rfootY-Angle", //312
    "virtual_hip_x_0_0_y_plus0_3Y-rfootY-Angle", //313
    "virtual_hip_x_0_0_y_plus0_3Y-EndSite_toe1-2.rY-Angle", //314
    "virtual_hip_x_0_0_y_plus0_3Y-virtual_hip_x_plus0_3_y_0Y-Angle", //315
    "virtual_hip_x_0_0_y_plus0_3Y-virtual_hip_x_minus_0_3_y_0Y-Angle", //316
    "virtual_hip_x_0_0_y_plus0_3Y-lhipY-Angle", //317
    "virtual_hip_x_0_0_y_plus0_3Y-halfway_lhip_and_lkneeY-Angle", //318
    "virtual_hip_x_0_0_y_plus0_3Y-lkneeY-Angle", //319
    "virtual_hip_x_0_0_y_plus0_3Y-halfway_lknee_and_lfootY-Angle", //320
    "virtual_hip_x_0_0_y_plus0_3Y-lfootY-Angle", //321
    "virtual_hip_x_0_0_y_plus0_3Y-EndSite_toe1-2.lY-Angle", //322
    "virtual_hip_x_0_0_y_plus0_3Y-virtual_hip_x_0_0_y_plus0_15Y-Angle", //323
    "virtual_hip_x_0_0_y_plus0_3Y-virtual_hip_x_0_0_y_plus0_3Y-Angle", //324
    "end"
};
/** @brief Programmer friendly enumerator of expected inputs*/
enum mocapNET_lowerbody_enum
{
    MNET_LOWERBODY_IN_2DX_HIP = 0, //0
    MNET_LOWERBODY_IN_2DY_HIP, //1
    MNET_LOWERBODY_IN_VISIBLE_HIP, //2
    MNET_LOWERBODY_IN_2DX_NECK, //3
    MNET_LOWERBODY_IN_2DY_NECK, //4
    MNET_LOWERBODY_IN_VISIBLE_NECK, //5
    MNET_LOWERBODY_IN_2DX_RHIP, //6
    MNET_LOWERBODY_IN_2DY_RHIP, //7
    MNET_LOWERBODY_IN_VISIBLE_RHIP, //8
    MNET_LOWERBODY_IN_2DX_RKNEE, //9
    MNET_LOWERBODY_IN_2DY_RKNEE, //10
    MNET_LOWERBODY_IN_VISIBLE_RKNEE, //11
    MNET_LOWERBODY_IN_2DX_RFOOT, //12
    MNET_LOWERBODY_IN_2DY_RFOOT, //13
    MNET_LOWERBODY_IN_VISIBLE_RFOOT, //14
    MNET_LOWERBODY_IN_2DX_ENDSITE_TOE1_2_R, //15
    MNET_LOWERBODY_IN_2DY_ENDSITE_TOE1_2_R, //16
    MNET_LOWERBODY_IN_VISIBLE_ENDSITE_TOE1_2_R, //17
    MNET_LOWERBODY_IN_2DX_ENDSITE_TOE5_3_R, //18
    MNET_LOWERBODY_IN_2DY_ENDSITE_TOE5_3_R, //19
    MNET_LOWERBODY_IN_VISIBLE_ENDSITE_TOE5_3_R, //20
    MNET_LOWERBODY_IN_2DX_LHIP, //21
    MNET_LOWERBODY_IN_2DY_LHIP, //22
    MNET_LOWERBODY_IN_VISIBLE_LHIP, //23
    MNET_LOWERBODY_IN_2DX_LKNEE, //24
    MNET_LOWERBODY_IN_2DY_LKNEE, //25
    MNET_LOWERBODY_IN_VISIBLE_LKNEE, //26
    MNET_LOWERBODY_IN_2DX_LFOOT, //27
    MNET_LOWERBODY_IN_2DY_LFOOT, //28
    MNET_LOWERBODY_IN_VISIBLE_LFOOT, //29
    MNET_LOWERBODY_IN_2DX_ENDSITE_TOE1_2_L, //30
    MNET_LOWERBODY_IN_2DY_ENDSITE_TOE1_2_L, //31
    MNET_LOWERBODY_IN_VISIBLE_ENDSITE_TOE1_2_L, //32
    MNET_LOWERBODY_IN_2DX_ENDSITE_TOE5_3_L, //33
    MNET_LOWERBODY_IN_2DY_ENDSITE_TOE5_3_L, //34
    MNET_LOWERBODY_IN_VISIBLE_ENDSITE_TOE5_3_L, //35
    MNET_LOWERBODY_IN_HIPY_HIPY_ANGLE, //36
    MNET_LOWERBODY_IN_HIPY_RHIPY_ANGLE, //37
    MNET_LOWERBODY_IN_HIPY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //38
    MNET_LOWERBODY_IN_HIPY_RKNEEY_ANGLE, //39
    MNET_LOWERBODY_IN_HIPY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //40
    MNET_LOWERBODY_IN_HIPY_RFOOTY_ANGLE, //41
    MNET_LOWERBODY_IN_HIPY_ENDSITE_TOE1_2_RY_ANGLE, //42
    MNET_LOWERBODY_IN_HIPY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //43
    MNET_LOWERBODY_IN_HIPY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //44
    MNET_LOWERBODY_IN_HIPY_LHIPY_ANGLE, //45
    MNET_LOWERBODY_IN_HIPY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //46
    MNET_LOWERBODY_IN_HIPY_LKNEEY_ANGLE, //47
    MNET_LOWERBODY_IN_HIPY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //48
    MNET_LOWERBODY_IN_HIPY_LFOOTY_ANGLE, //49
    MNET_LOWERBODY_IN_HIPY_ENDSITE_TOE1_2_LY_ANGLE, //50
    MNET_LOWERBODY_IN_HIPY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //51
    MNET_LOWERBODY_IN_HIPY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //52
    MNET_LOWERBODY_IN_RHIPY_HIPY_ANGLE, //53
    MNET_LOWERBODY_IN_RHIPY_RHIPY_ANGLE, //54
    MNET_LOWERBODY_IN_RHIPY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //55
    MNET_LOWERBODY_IN_RHIPY_RKNEEY_ANGLE, //56
    MNET_LOWERBODY_IN_RHIPY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //57
    MNET_LOWERBODY_IN_RHIPY_RFOOTY_ANGLE, //58
    MNET_LOWERBODY_IN_RHIPY_ENDSITE_TOE1_2_RY_ANGLE, //59
    MNET_LOWERBODY_IN_RHIPY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //60
    MNET_LOWERBODY_IN_RHIPY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //61
    MNET_LOWERBODY_IN_RHIPY_LHIPY_ANGLE, //62
    MNET_LOWERBODY_IN_RHIPY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //63
    MNET_LOWERBODY_IN_RHIPY_LKNEEY_ANGLE, //64
    MNET_LOWERBODY_IN_RHIPY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //65
    MNET_LOWERBODY_IN_RHIPY_LFOOTY_ANGLE, //66
    MNET_LOWERBODY_IN_RHIPY_ENDSITE_TOE1_2_LY_ANGLE, //67
    MNET_LOWERBODY_IN_RHIPY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //68
    MNET_LOWERBODY_IN_RHIPY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //69
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_HIPY_ANGLE, //70
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_RHIPY_ANGLE, //71
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //72
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_RKNEEY_ANGLE, //73
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //74
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_RFOOTY_ANGLE, //75
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_ENDSITE_TOE1_2_RY_ANGLE, //76
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //77
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //78
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_LHIPY_ANGLE, //79
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //80
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_LKNEEY_ANGLE, //81
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //82
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_LFOOTY_ANGLE, //83
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_ENDSITE_TOE1_2_LY_ANGLE, //84
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //85
    MNET_LOWERBODY_IN_HALFWAY_RHIP_AND_RKNEEY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //86
    MNET_LOWERBODY_IN_RKNEEY_HIPY_ANGLE, //87
    MNET_LOWERBODY_IN_RKNEEY_RHIPY_ANGLE, //88
    MNET_LOWERBODY_IN_RKNEEY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //89
    MNET_LOWERBODY_IN_RKNEEY_RKNEEY_ANGLE, //90
    MNET_LOWERBODY_IN_RKNEEY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //91
    MNET_LOWERBODY_IN_RKNEEY_RFOOTY_ANGLE, //92
    MNET_LOWERBODY_IN_RKNEEY_ENDSITE_TOE1_2_RY_ANGLE, //93
    MNET_LOWERBODY_IN_RKNEEY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //94
    MNET_LOWERBODY_IN_RKNEEY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //95
    MNET_LOWERBODY_IN_RKNEEY_LHIPY_ANGLE, //96
    MNET_LOWERBODY_IN_RKNEEY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //97
    MNET_LOWERBODY_IN_RKNEEY_LKNEEY_ANGLE, //98
    MNET_LOWERBODY_IN_RKNEEY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //99
    MNET_LOWERBODY_IN_RKNEEY_LFOOTY_ANGLE, //100
    MNET_LOWERBODY_IN_RKNEEY_ENDSITE_TOE1_2_LY_ANGLE, //101
    MNET_LOWERBODY_IN_RKNEEY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //102
    MNET_LOWERBODY_IN_RKNEEY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //103
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_HIPY_ANGLE, //104
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_RHIPY_ANGLE, //105
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //106
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_RKNEEY_ANGLE, //107
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //108
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_RFOOTY_ANGLE, //109
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_ENDSITE_TOE1_2_RY_ANGLE, //110
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //111
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //112
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_LHIPY_ANGLE, //113
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //114
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_LKNEEY_ANGLE, //115
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //116
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_LFOOTY_ANGLE, //117
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_ENDSITE_TOE1_2_LY_ANGLE, //118
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //119
    MNET_LOWERBODY_IN_HALFWAY_RKNEE_AND_RFOOTY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //120
    MNET_LOWERBODY_IN_RFOOTY_HIPY_ANGLE, //121
    MNET_LOWERBODY_IN_RFOOTY_RHIPY_ANGLE, //122
    MNET_LOWERBODY_IN_RFOOTY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //123
    MNET_LOWERBODY_IN_RFOOTY_RKNEEY_ANGLE, //124
    MNET_LOWERBODY_IN_RFOOTY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //125
    MNET_LOWERBODY_IN_RFOOTY_RFOOTY_ANGLE, //126
    MNET_LOWERBODY_IN_RFOOTY_ENDSITE_TOE1_2_RY_ANGLE, //127
    MNET_LOWERBODY_IN_RFOOTY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //128
    MNET_LOWERBODY_IN_RFOOTY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //129
    MNET_LOWERBODY_IN_RFOOTY_LHIPY_ANGLE, //130
    MNET_LOWERBODY_IN_RFOOTY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //131
    MNET_LOWERBODY_IN_RFOOTY_LKNEEY_ANGLE, //132
    MNET_LOWERBODY_IN_RFOOTY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //133
    MNET_LOWERBODY_IN_RFOOTY_LFOOTY_ANGLE, //134
    MNET_LOWERBODY_IN_RFOOTY_ENDSITE_TOE1_2_LY_ANGLE, //135
    MNET_LOWERBODY_IN_RFOOTY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //136
    MNET_LOWERBODY_IN_RFOOTY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //137
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_HIPY_ANGLE, //138
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_RHIPY_ANGLE, //139
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //140
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_RKNEEY_ANGLE, //141
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //142
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_RFOOTY_ANGLE, //143
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_ENDSITE_TOE1_2_RY_ANGLE, //144
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //145
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //146
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_LHIPY_ANGLE, //147
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //148
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_LKNEEY_ANGLE, //149
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //150
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_LFOOTY_ANGLE, //151
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_ENDSITE_TOE1_2_LY_ANGLE, //152
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //153
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_RY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //154
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_HIPY_ANGLE, //155
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_RHIPY_ANGLE, //156
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //157
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_RKNEEY_ANGLE, //158
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //159
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_RFOOTY_ANGLE, //160
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ENDSITE_TOE1_2_RY_ANGLE, //161
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //162
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //163
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_LHIPY_ANGLE, //164
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //165
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_LKNEEY_ANGLE, //166
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //167
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_LFOOTY_ANGLE, //168
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ENDSITE_TOE1_2_LY_ANGLE, //169
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //170
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //171
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_HIPY_ANGLE, //172
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_RHIPY_ANGLE, //173
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //174
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_RKNEEY_ANGLE, //175
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //176
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_RFOOTY_ANGLE, //177
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ENDSITE_TOE1_2_RY_ANGLE, //178
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //179
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //180
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_LHIPY_ANGLE, //181
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //182
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_LKNEEY_ANGLE, //183
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //184
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_LFOOTY_ANGLE, //185
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ENDSITE_TOE1_2_LY_ANGLE, //186
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //187
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //188
    MNET_LOWERBODY_IN_LHIPY_HIPY_ANGLE, //189
    MNET_LOWERBODY_IN_LHIPY_RHIPY_ANGLE, //190
    MNET_LOWERBODY_IN_LHIPY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //191
    MNET_LOWERBODY_IN_LHIPY_RKNEEY_ANGLE, //192
    MNET_LOWERBODY_IN_LHIPY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //193
    MNET_LOWERBODY_IN_LHIPY_RFOOTY_ANGLE, //194
    MNET_LOWERBODY_IN_LHIPY_ENDSITE_TOE1_2_RY_ANGLE, //195
    MNET_LOWERBODY_IN_LHIPY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //196
    MNET_LOWERBODY_IN_LHIPY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //197
    MNET_LOWERBODY_IN_LHIPY_LHIPY_ANGLE, //198
    MNET_LOWERBODY_IN_LHIPY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //199
    MNET_LOWERBODY_IN_LHIPY_LKNEEY_ANGLE, //200
    MNET_LOWERBODY_IN_LHIPY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //201
    MNET_LOWERBODY_IN_LHIPY_LFOOTY_ANGLE, //202
    MNET_LOWERBODY_IN_LHIPY_ENDSITE_TOE1_2_LY_ANGLE, //203
    MNET_LOWERBODY_IN_LHIPY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //204
    MNET_LOWERBODY_IN_LHIPY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //205
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_HIPY_ANGLE, //206
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_RHIPY_ANGLE, //207
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //208
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_RKNEEY_ANGLE, //209
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //210
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_RFOOTY_ANGLE, //211
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_ENDSITE_TOE1_2_RY_ANGLE, //212
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //213
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //214
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_LHIPY_ANGLE, //215
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //216
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_LKNEEY_ANGLE, //217
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //218
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_LFOOTY_ANGLE, //219
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_ENDSITE_TOE1_2_LY_ANGLE, //220
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //221
    MNET_LOWERBODY_IN_HALFWAY_LHIP_AND_LKNEEY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //222
    MNET_LOWERBODY_IN_LKNEEY_HIPY_ANGLE, //223
    MNET_LOWERBODY_IN_LKNEEY_RHIPY_ANGLE, //224
    MNET_LOWERBODY_IN_LKNEEY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //225
    MNET_LOWERBODY_IN_LKNEEY_RKNEEY_ANGLE, //226
    MNET_LOWERBODY_IN_LKNEEY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //227
    MNET_LOWERBODY_IN_LKNEEY_RFOOTY_ANGLE, //228
    MNET_LOWERBODY_IN_LKNEEY_ENDSITE_TOE1_2_RY_ANGLE, //229
    MNET_LOWERBODY_IN_LKNEEY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //230
    MNET_LOWERBODY_IN_LKNEEY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //231
    MNET_LOWERBODY_IN_LKNEEY_LHIPY_ANGLE, //232
    MNET_LOWERBODY_IN_LKNEEY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //233
    MNET_LOWERBODY_IN_LKNEEY_LKNEEY_ANGLE, //234
    MNET_LOWERBODY_IN_LKNEEY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //235
    MNET_LOWERBODY_IN_LKNEEY_LFOOTY_ANGLE, //236
    MNET_LOWERBODY_IN_LKNEEY_ENDSITE_TOE1_2_LY_ANGLE, //237
    MNET_LOWERBODY_IN_LKNEEY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //238
    MNET_LOWERBODY_IN_LKNEEY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //239
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_HIPY_ANGLE, //240
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_RHIPY_ANGLE, //241
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //242
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_RKNEEY_ANGLE, //243
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //244
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_RFOOTY_ANGLE, //245
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_ENDSITE_TOE1_2_RY_ANGLE, //246
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //247
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //248
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_LHIPY_ANGLE, //249
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //250
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_LKNEEY_ANGLE, //251
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //252
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_LFOOTY_ANGLE, //253
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_ENDSITE_TOE1_2_LY_ANGLE, //254
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //255
    MNET_LOWERBODY_IN_HALFWAY_LKNEE_AND_LFOOTY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //256
    MNET_LOWERBODY_IN_LFOOTY_HIPY_ANGLE, //257
    MNET_LOWERBODY_IN_LFOOTY_RHIPY_ANGLE, //258
    MNET_LOWERBODY_IN_LFOOTY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //259
    MNET_LOWERBODY_IN_LFOOTY_RKNEEY_ANGLE, //260
    MNET_LOWERBODY_IN_LFOOTY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //261
    MNET_LOWERBODY_IN_LFOOTY_RFOOTY_ANGLE, //262
    MNET_LOWERBODY_IN_LFOOTY_ENDSITE_TOE1_2_RY_ANGLE, //263
    MNET_LOWERBODY_IN_LFOOTY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //264
    MNET_LOWERBODY_IN_LFOOTY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //265
    MNET_LOWERBODY_IN_LFOOTY_LHIPY_ANGLE, //266
    MNET_LOWERBODY_IN_LFOOTY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //267
    MNET_LOWERBODY_IN_LFOOTY_LKNEEY_ANGLE, //268
    MNET_LOWERBODY_IN_LFOOTY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //269
    MNET_LOWERBODY_IN_LFOOTY_LFOOTY_ANGLE, //270
    MNET_LOWERBODY_IN_LFOOTY_ENDSITE_TOE1_2_LY_ANGLE, //271
    MNET_LOWERBODY_IN_LFOOTY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //272
    MNET_LOWERBODY_IN_LFOOTY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //273
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_HIPY_ANGLE, //274
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_RHIPY_ANGLE, //275
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //276
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_RKNEEY_ANGLE, //277
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //278
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_RFOOTY_ANGLE, //279
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_ENDSITE_TOE1_2_RY_ANGLE, //280
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //281
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //282
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_LHIPY_ANGLE, //283
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //284
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_LKNEEY_ANGLE, //285
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //286
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_LFOOTY_ANGLE, //287
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_ENDSITE_TOE1_2_LY_ANGLE, //288
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //289
    MNET_LOWERBODY_IN_ENDSITE_TOE1_2_LY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //290
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_HIPY_ANGLE, //291
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_RHIPY_ANGLE, //292
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //293
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_RKNEEY_ANGLE, //294
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //295
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_RFOOTY_ANGLE, //296
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ENDSITE_TOE1_2_RY_ANGLE, //297
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //298
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //299
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_LHIPY_ANGLE, //300
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //301
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_LKNEEY_ANGLE, //302
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //303
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_LFOOTY_ANGLE, //304
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ENDSITE_TOE1_2_LY_ANGLE, //305
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //306
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //307
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_HIPY_ANGLE, //308
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_RHIPY_ANGLE, //309
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_HALFWAY_RHIP_AND_RKNEEY_ANGLE, //310
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_RKNEEY_ANGLE, //311
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_HALFWAY_RKNEE_AND_RFOOTY_ANGLE, //312
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_RFOOTY_ANGLE, //313
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ENDSITE_TOE1_2_RY_ANGLE, //314
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ANGLE, //315
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ANGLE, //316
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_LHIPY_ANGLE, //317
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_HALFWAY_LHIP_AND_LKNEEY_ANGLE, //318
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_LKNEEY_ANGLE, //319
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_HALFWAY_LKNEE_AND_LFOOTY_ANGLE, //320
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_LFOOTY_ANGLE, //321
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ENDSITE_TOE1_2_LY_ANGLE, //322
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_VIRTUAL_HIP_X_0_0_Y_PLUS0_15Y_ANGLE, //323
    MNET_LOWERBODY_IN_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_VIRTUAL_HIP_X_0_0_Y_PLUS0_3Y_ANGLE, //324
    MNET_LOWERBODY_IN_NUMBER
};

/** @brief Programmer friendly enumerator of expected outputs*/
enum mocapNET_Output_lowerbody_enum
{
    MOCAPNET_LOWERBODY_OUTPUT_HIP_XPOSITION = 0, //0
    MOCAPNET_LOWERBODY_OUTPUT_HIP_YPOSITION, //1
    MOCAPNET_LOWERBODY_OUTPUT_HIP_ZPOSITION, //2
    MOCAPNET_LOWERBODY_OUTPUT_HIP_ZROTATION, //3
    MOCAPNET_LOWERBODY_OUTPUT_HIP_YROTATION, //4
    MOCAPNET_LOWERBODY_OUTPUT_HIP_XROTATION, //5
    MOCAPNET_LOWERBODY_OUTPUT_ABDOMEN_ZROTATION, //6
    MOCAPNET_LOWERBODY_OUTPUT_ABDOMEN_XROTATION, //7
    MOCAPNET_LOWERBODY_OUTPUT_ABDOMEN_YROTATION, //8
    MOCAPNET_LOWERBODY_OUTPUT_CHEST_ZROTATION, //9
    MOCAPNET_LOWERBODY_OUTPUT_CHEST_XROTATION, //10
    MOCAPNET_LOWERBODY_OUTPUT_CHEST_YROTATION, //11
    MOCAPNET_LOWERBODY_OUTPUT_NECK_ZROTATION, //12
    MOCAPNET_LOWERBODY_OUTPUT_NECK_XROTATION, //13
    MOCAPNET_LOWERBODY_OUTPUT_NECK_YROTATION, //14
    MOCAPNET_LOWERBODY_OUTPUT_RHIP_ZROTATION, //15
    MOCAPNET_LOWERBODY_OUTPUT_RHIP_XROTATION, //16
    MOCAPNET_LOWERBODY_OUTPUT_RHIP_YROTATION, //17
    MOCAPNET_LOWERBODY_OUTPUT_RKNEE_ZROTATION, //18
    MOCAPNET_LOWERBODY_OUTPUT_RKNEE_XROTATION, //19
    MOCAPNET_LOWERBODY_OUTPUT_RKNEE_YROTATION, //20
    MOCAPNET_LOWERBODY_OUTPUT_RFOOT_ZROTATION, //21
    MOCAPNET_LOWERBODY_OUTPUT_RFOOT_XROTATION, //22
    MOCAPNET_LOWERBODY_OUTPUT_RFOOT_YROTATION, //23
    MOCAPNET_LOWERBODY_OUTPUT_TOE1_2_R_ZROTATION, //24
    MOCAPNET_LOWERBODY_OUTPUT_TOE1_2_R_XROTATION, //25
    MOCAPNET_LOWERBODY_OUTPUT_TOE1_2_R_YROTATION, //26
    MOCAPNET_LOWERBODY_OUTPUT_TOE5_3_R_ZROTATION, //27
    MOCAPNET_LOWERBODY_OUTPUT_TOE5_3_R_XROTATION, //28
    MOCAPNET_LOWERBODY_OUTPUT_TOE5_3_R_YROTATION, //29
    MOCAPNET_LOWERBODY_OUTPUT_LHIP_ZROTATION, //30
    MOCAPNET_LOWERBODY_OUTPUT_LHIP_XROTATION, //31
    MOCAPNET_LOWERBODY_OUTPUT_LHIP_YROTATION, //32
    MOCAPNET_LOWERBODY_OUTPUT_LKNEE_ZROTATION, //33
    MOCAPNET_LOWERBODY_OUTPUT_LKNEE_XROTATION, //34
    MOCAPNET_LOWERBODY_OUTPUT_LKNEE_YROTATION, //35
    MOCAPNET_LOWERBODY_OUTPUT_LFOOT_ZROTATION, //36
    MOCAPNET_LOWERBODY_OUTPUT_LFOOT_XROTATION, //37
    MOCAPNET_LOWERBODY_OUTPUT_LFOOT_YROTATION, //38
    MOCAPNET_LOWERBODY_OUTPUT_TOE1_2_L_ZROTATION, //39
    MOCAPNET_LOWERBODY_OUTPUT_TOE1_2_L_XROTATION, //40
    MOCAPNET_LOWERBODY_OUTPUT_TOE1_2_L_YROTATION, //41
    MOCAPNET_LOWERBODY_OUTPUT_TOE5_3_L_ZROTATION, //42
    MOCAPNET_LOWERBODY_OUTPUT_TOE5_3_L_XROTATION, //43
    MOCAPNET_LOWERBODY_OUTPUT_TOE5_3_L_YROTATION, //44
    MOCAPNET_LOWERBODY_OUTPUT_NUMBER
};

/** @brief Programmer friendly enumerator of NSDM elments*/
enum mocapNET_NSDM_lowerbody_enum
{
    MNET_NSDM_LOWERBODY_HIP = 0, //0
    MNET_NSDM_LOWERBODY_RHIP, //1
    MNET_NSDM_LOWERBODY_VIRTUAL_HALFWAY_BETWEEN_RHIP_AND_RKNEE, //2
    MNET_NSDM_LOWERBODY_RKNEE, //3
    MNET_NSDM_LOWERBODY_VIRTUAL_HALFWAY_BETWEEN_RKNEE_AND_RFOOT, //4
    MNET_NSDM_LOWERBODY_RFOOT, //5
    MNET_NSDM_LOWERBODY_ENDSITE_TOE1_2_R, //6
    MNET_NSDM_LOWERBODY_VIRTUAL_HIP_X_PLUS0_3_Y_0, //7
    MNET_NSDM_LOWERBODY_VIRTUAL_HIP_X_MINUS_0_3_Y_0, //8
    MNET_NSDM_LOWERBODY_LHIP, //9
    MNET_NSDM_LOWERBODY_VIRTUAL_HALFWAY_BETWEEN_LHIP_AND_LKNEE, //10
    MNET_NSDM_LOWERBODY_LKNEE, //11
    MNET_NSDM_LOWERBODY_VIRTUAL_HALFWAY_BETWEEN_LKNEE_AND_LFOOT, //12
    MNET_NSDM_LOWERBODY_LFOOT, //13
    MNET_NSDM_LOWERBODY_ENDSITE_TOE1_2_L, //14
    MNET_NSDM_LOWERBODY_VIRTUAL_HIP_X_0_0_Y_PLUS0_15, //15
    MNET_NSDM_LOWERBODY_VIRTUAL_HIP_X_0_0_Y_PLUS0_3, //16
    MNET_NSDM_LOWERBODY_NUMBER
};

/** @brief This is a lookup table to immediately resolve referred Joints*/
static const int mocapNET_ResolveJoint_lowerbody[] =
{
    0, //0
    2, //1
    2, //2
    3, //3
    3, //4
    4, //5
    5, //6
    0, //7
    0, //8
    7, //9
    7, //10
    8, //11
    8, //12
    9, //13
    10, //14
    0, //15
    0, //16
    0//end of array
};

/** @brief This is a lookup table to immediately resolve referred Joints of second targets*/
static const int mocapNET_ResolveSecondTargetJoint_lowerbody[] =
{
    0, //0
    0, //1
    3, //2
    0, //3
    4, //4
    0, //5
    0, //6
    0, //7
    0, //8
    0, //9
    8, //10
    0, //11
    9, //12
    0, //13
    0, //14
    0, //15
    0, //16
    0//end of array
};

/** @brief This is the configuration of NSDM elements :
 *  A value of 0 is a normal 2D point
 *  A value of 1 is a 2D point plus some offset
 *  A value of 2 is a virtual point between two 2D points */
static const int mocapNET_ArtificialJoint_lowerbody[] =
{
    0, //0
    0, //1
    2, //2
    0, //3
    2, //4
    0, //5
    0, //6
    1, //7
    1, //8
    0, //9
    2, //10
    0, //11
    2, //12
    0, //13
    0, //14
    1, //15
    1, //16
    0//end of array
};

/** @brief These are X offsets for artificial joints of type 1 ( see mocapNET_ArtificialJoint_lowerbody )*/
static const float mocapNET_ArtificialJointXOffset_lowerbody[] =
{
    0, //0
    0, //1
    0, //2
    0, //3
    0, //4
    0, //5
    0, //6
    0.3, //7
    -0.3, //8
    0, //9
    0, //10
    0, //11
    0, //12
    0, //13
    0, //14
    0.0, //15
    0.0, //16
    0//end of array
};

/** @brief These are Y offsets for artificial joints of type 1 ( see mocapNET_ArtificialJoint_lowerbody )*/
static const float mocapNET_ArtificialJointYOffset_lowerbody[] =
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
    0.15, //15
    0.3, //16
    0//end of array
};

/** @brief These are 2D Joints that are used as starting points for scaling vectors*/
static const int mocapNET_ScalingStart_lowerbody[] =
{
    0, //0
    0, //1
    0//end of array
};

/** @brief These are 2D Joints that are used as ending points for scaling vectors*/
static const int mocapNET_ScalingEnd_lowerbody[] =
{
    2, //0
    7, //1
    0//end of array
};

/** @brief These is a 2D Joints that is used as alignment for the skeleton*/
static const int mocapNET_AlignmentStart_lowerbody[] =
{
    3, //0
    0//end of array
};

/** @brief These is a 2D Joints that is used as alignment for the skeleton*/
static const int mocapNET_AlignmentEnd_lowerbody[] =
{
    2, //0
    0//end of array
};

/** @brief This function can be used to debug NSDM input and find in a user friendly what is missing..!*/
static int lowerbodyCountMissingNSDMElements(std::vector<float> mocapNETInput,int verbose)
{
    unsigned int numberOfZeros=0;
    for (int i=0; i<mocapNETInput.size(); i++)
    {
        if (mocapNETInput[i]==0)
        {
            if (verbose) {
                fprintf(stderr,"lowerbody NSDM %s missing\n",mocapNET_lowerbody[i]);
            }
            ++numberOfZeros;
        }
    }
    return numberOfZeros;
}

/** @brief Another debug function to make sure the joint order is correctly resolved..!*/
static void lowerbodyDebugNSDMJointOrder(char ** labels)
{
    for (int i=0; i<MNET_NSDM_LOWERBODY_NUMBER; i++)
    {
        fprintf(stderr,"NSDM lowerbody %s => skeletonSerialized %s\n ",mocapNET_lowerbody[i],labels[i]);
    }
}

/** @brief This function returns the euclidean distance between two input 2D joints and zero if either of them is invalid*/
static float getJoint2DDistance_LOWERBODY(std::vector<float> in,int jointA,int jointB)
{
    float aX=in[jointA*3+0];
    float aY=in[jointA*3+1];
    float bX=in[jointB*3+0];
    float bY=in[jointB*3+1];
    if ( ((aX==0) && (aY==0)) || ((bX==0) && (bY==0)) ) {
        return 0.0;
    }


    float xDistance=(float) bX-aX;
    float yDistance=(float) bY-aY;
    return sqrt( (xDistance*xDistance) + (yDistance*yDistance) );
}

/** @brief This function returns a vector of NSDM values ready for use with the MocapNET lowerbody network */
static std::vector<float> lowerbodyCreateNDSM(std::vector<float> in,int havePositionalElements,int haveAngularElements,int doNormalization)
{
    std::vector<float> result;
    int secondTargetJointID;
    float sIX,sIY,sJX,sJY;
    for (int i=0; i<MNET_NSDM_LOWERBODY_NUMBER; i++)
    {
        //------------------------------------
        unsigned int iJointID=mocapNET_ResolveJoint_lowerbody[i];
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
            for (int j=0; j<MNET_NSDM_LOWERBODY_NUMBER; j++)
            {
                if (havePositionalElements) {
                    result.push_back(0.0);
                    result.push_back(0.0);
                }
                if (haveAngularElements)    {
                    result.push_back(0.0);
                }
            }
            invalidPointI=1;
        } else
        {
            switch (mocapNET_ArtificialJoint_lowerbody[i])
            {
            case 1 :
                iX+=mocapNET_ArtificialJointXOffset_lowerbody[i];
                iY+=mocapNET_ArtificialJointYOffset_lowerbody[i];
                break;

            case 2 :
                secondTargetJointID=mocapNET_ResolveSecondTargetJoint_lowerbody[i];
                sIX=in[secondTargetJointID*3+0];
                sIY=in[secondTargetJointID*3+1];
                invalidPointI=((sIX==0) && (sIY==0));
                if (!invalidPointI)
                {
                    iX=(float) (iX+sIX)/2;
                    iY=(float) (iY+sIY)/2;
                }
                break;

            default :
                //Normal NSDM element ( not virtual )
                break;
            };
            for (int j=0; j<MNET_NSDM_LOWERBODY_NUMBER; j++)
            {
                jJointID=mocapNET_ResolveJoint_lowerbody[j];
                jX=in[jJointID*3+0];
                jY=in[jJointID*3+1];
                jVisibility=in[jJointID*3+2];
                invalidPointJ=0;

                if ((jX==0) && (jY==0))
                {
                    if (havePositionalElements) {
                        result.push_back(0.0);
                        result.push_back(0.0);
                    }
                    if (haveAngularElements)    {
                        result.push_back(0.0);
                    }
                    invalidPointJ=1;

                }
                else
                {
                    switch (mocapNET_ArtificialJoint_lowerbody[j])
                    {
                    case 1 :
                        jX+=mocapNET_ArtificialJointXOffset_lowerbody[j];
                        jY+=mocapNET_ArtificialJointYOffset_lowerbody[j];
                        break;

                    case 2 :
                        secondTargetJointID=mocapNET_ResolveSecondTargetJoint_lowerbody[j];
                        sJX=in[secondTargetJointID*3+0];
                        sJY=in[secondTargetJointID*3+1];
                        invalidPointJ=((sJX==0) && (sJY==0));
                        if (!invalidPointJ)
                        {
                            jX=(float) (jX+sJX)/2;
                            jY=(float) (jY+sJY)/2;
                        }
                        break;

                    default :
                        //Normal NSDM element ( not virtual )
                        break;
                    };
                    //---------------------------------
                    if (invalidPointJ||invalidPointI)
                    {
                        if (havePositionalElements) {
                            result.push_back(0.0);
                            result.push_back(0.0);
                        }
                        if (haveAngularElements)    {
                            result.push_back(0.0);
                        }
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
            unsigned int numberOfNSDMScalingRules=2;
            if (numberOfNSDMScalingRules>0)
            {
                unsigned int numberOfDistanceSamples=0;
                float sumOfDistanceSamples=0.0;
                for ( int i=0; i<numberOfNSDMScalingRules; i++)
                {
                    unsigned int iJointID=mocapNET_ScalingStart_lowerbody[i];
                    unsigned int jJointID=mocapNET_ScalingEnd_lowerbody[i];
                    float distance = getJoint2DDistance_LOWERBODY(in,iJointID,jJointID);
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
                {
                    scaleDistance=(float) sumOfDistanceSamples/numberOfDistanceSamples;
                }
//-------------------------------------------------------------------------------------------------
                if (scaleDistance!=1.0)
                {
                    for (int i=0; i<result.size(); i++)
                    {
                        result[i]=result[i]/scaleDistance;
                    }
                }
            } //If there are normalization rules..
//-------------------------------------------------------------------------------------------------
            for (int i=0; i<result.size(); i++)
            {
                if (result[i]!=0.0)
                {
                    result[i]=0.5+result[i];
                }
            }
//-------------------------------------------------------------------------------------------------
        }



//-------------------------------------------------------------------------------------------------
        if ( (haveAngularElements) && (!havePositionalElements) )
        {
            float minValue=result[0], maxValue=result[0];
            for (int i=0; i<result.size(); i++)
            {
                if (result[i]<minValue) {
                    minValue=result[i];
                }
                if (result[i]>maxValue) {
                    maxValue=result[i];
                }
            }
            fprintf(stderr,"Original Min Value %0.2f, Max Value %0.2f \n",minValue,maxValue);

 
                unsigned int iJointID=mocapNET_AlignmentStart_lowerbody[0];
                unsigned int jJointID=mocapNET_AlignmentEnd_lowerbody[0];
                float aX=in[iJointID*3+0];
                float aY=in[iJointID*3+1];
                float bX=in[jJointID*3+0];
                float bY=in[jJointID*3+1];
                float alignmentAngle=getAngleToAlignToZero_tools(aX,aY,bX,bY);
                for (int i=0; i<result.size(); i++)
                {
                    if (result[i]!=0.0)
                    {
                        result[i]= alignmentAngle + result[i];
                    }
                }
                
            minValue=result[0], maxValue=result[0];
            for (int i=0; i<result.size(); i++)
            {
                if (result[i]<minValue) {
                    minValue=result[i];
                }
                if (result[i]>maxValue) {
                    maxValue=result[i];
                }
            }
            fprintf(stderr,"Aligned Min Value %0.2f, Max Value %0.2f \n",minValue,maxValue);


        }
//-------------------------------------------------------------------------------------------------


    } //If normalization is enabled..
    return result;
}

/** @file generated_body.hpp
 *  @brief A Description the body input of a Tensorflow network required for MocapNET
 *  @author Ammar Qammaz (AmmarkoV)
 *  Automatically generated using :
 *     python3 exportCPPCodeFromJSONConfiguration.py --front body --config dataset/body_configuration.json
 *     please note that since the names of the labels are both affected by the dataset/body_configuration.json configuration
 *     as well as the ground truth, if you have made any weird additions you might consider running the ./createRandomizedDataset.sh and ./createTestDataset.sh scripts
 */

#pragma once

#include <iostream>
#include <math.h>
#include <vector>
#include "../tools.hpp"

/** @brief This is an array of names for all uncompressed 2D inputs expected. */
static const unsigned int mocapNET_InputLength_WithoutNSDM_body = 63;

/** @brief An array of strings that contains the label for each expected input. */
static const char * mocapNET_body[] =
{
    "2DX_hip", //0
    "2DY_hip", //1
    "visible_hip", //2
    "2DX_neck", //3
    "2DY_neck", //4
    "visible_neck", //5
    "2DX_head", //6
    "2DY_head", //7
    "visible_head", //8
    "2DX_EndSite_eye.l", //9
    "2DY_EndSite_eye.l", //10
    "visible_EndSite_eye.l", //11
    "2DX_EndSite_eye.r", //12
    "2DY_EndSite_eye.r", //13
    "visible_EndSite_eye.r", //14
    "2DX_rshoulder", //15
    "2DY_rshoulder", //16
    "visible_rshoulder", //17
    "2DX_relbow", //18
    "2DY_relbow", //19
    "visible_relbow", //20
    "2DX_rhand", //21
    "2DY_rhand", //22
    "visible_rhand", //23
    "2DX_lshoulder", //24
    "2DY_lshoulder", //25
    "visible_lshoulder", //26
    "2DX_lelbow", //27
    "2DY_lelbow", //28
    "visible_lelbow", //29
    "2DX_lhand", //30
    "2DY_lhand", //31
    "visible_lhand", //32
    "2DX_rhip", //33
    "2DY_rhip", //34
    "visible_rhip", //35
    "2DX_rknee", //36
    "2DY_rknee", //37
    "visible_rknee", //38
    "2DX_rfoot", //39
    "2DY_rfoot", //40
    "visible_rfoot", //41
    "2DX_EndSite_toe1-2.r", //42
    "2DY_EndSite_toe1-2.r", //43
    "visible_EndSite_toe1-2.r", //44
    "2DX_EndSite_toe5-3.r", //45
    "2DY_EndSite_toe5-3.r", //46
    "visible_EndSite_toe5-3.r", //47
    "2DX_lhip", //48
    "2DY_lhip", //49
    "visible_lhip", //50
    "2DX_lknee", //51
    "2DY_lknee", //52
    "visible_lknee", //53
    "2DX_lfoot", //54
    "2DY_lfoot", //55
    "visible_lfoot", //56
    "2DX_EndSite_toe1-2.l", //57
    "2DY_EndSite_toe1-2.l", //58
    "visible_EndSite_toe1-2.l", //59
    "2DX_EndSite_toe5-3.l", //60
    "2DY_EndSite_toe5-3.l", //61
    "visible_EndSite_toe5-3.l", //62
//This is where regular input ends and the NSDM data kicks in..
    "hipX-hipX", //63
    "hipY-hipY", //64
    "hipX-EndSite_eye.rX", //65
    "hipY-EndSite_eye.rY", //66
    "hipX-EndSite_eye.lX", //67
    "hipY-EndSite_eye.lY", //68
    "hipX-neckX", //69
    "hipY-neckY", //70
    "hipX-virtual_hip_x_plus0_3_y_0X", //71
    "hipY-virtual_hip_x_plus0_3_y_0Y", //72
    "hipX-rshoulderX", //73
    "hipY-rshoulderY", //74
    "hipX-relbowX", //75
    "hipY-relbowY", //76
    "hipX-rhandX", //77
    "hipY-rhandY", //78
    "hipX-virtual_hip_x_minus_0_3_y_0X", //79
    "hipY-virtual_hip_x_minus_0_3_y_0Y", //80
    "hipX-lshoulderX", //81
    "hipY-lshoulderY", //82
    "hipX-lelbowX", //83
    "hipY-lelbowY", //84
    "hipX-lhandX", //85
    "hipY-lhandY", //86
    "hipX-rhipX", //87
    "hipY-rhipY", //88
    "hipX-rkneeX", //89
    "hipY-rkneeY", //90
    "hipX-rfootX", //91
    "hipY-rfootY", //92
    "hipX-lhipX", //93
    "hipY-lhipY", //94
    "hipX-lkneeX", //95
    "hipY-lkneeY", //96
    "hipX-lfootX", //97
    "hipY-lfootY", //98
    "EndSite_eye.rX-hipX", //99
    "EndSite_eye.rY-hipY", //100
    "EndSite_eye.rX-EndSite_eye.rX", //101
    "EndSite_eye.rY-EndSite_eye.rY", //102
    "EndSite_eye.rX-EndSite_eye.lX", //103
    "EndSite_eye.rY-EndSite_eye.lY", //104
    "EndSite_eye.rX-neckX", //105
    "EndSite_eye.rY-neckY", //106
    "EndSite_eye.rX-virtual_hip_x_plus0_3_y_0X", //107
    "EndSite_eye.rY-virtual_hip_x_plus0_3_y_0Y", //108
    "EndSite_eye.rX-rshoulderX", //109
    "EndSite_eye.rY-rshoulderY", //110
    "EndSite_eye.rX-relbowX", //111
    "EndSite_eye.rY-relbowY", //112
    "EndSite_eye.rX-rhandX", //113
    "EndSite_eye.rY-rhandY", //114
    "EndSite_eye.rX-virtual_hip_x_minus_0_3_y_0X", //115
    "EndSite_eye.rY-virtual_hip_x_minus_0_3_y_0Y", //116
    "EndSite_eye.rX-lshoulderX", //117
    "EndSite_eye.rY-lshoulderY", //118
    "EndSite_eye.rX-lelbowX", //119
    "EndSite_eye.rY-lelbowY", //120
    "EndSite_eye.rX-lhandX", //121
    "EndSite_eye.rY-lhandY", //122
    "EndSite_eye.rX-rhipX", //123
    "EndSite_eye.rY-rhipY", //124
    "EndSite_eye.rX-rkneeX", //125
    "EndSite_eye.rY-rkneeY", //126
    "EndSite_eye.rX-rfootX", //127
    "EndSite_eye.rY-rfootY", //128
    "EndSite_eye.rX-lhipX", //129
    "EndSite_eye.rY-lhipY", //130
    "EndSite_eye.rX-lkneeX", //131
    "EndSite_eye.rY-lkneeY", //132
    "EndSite_eye.rX-lfootX", //133
    "EndSite_eye.rY-lfootY", //134
    "EndSite_eye.lX-hipX", //135
    "EndSite_eye.lY-hipY", //136
    "EndSite_eye.lX-EndSite_eye.rX", //137
    "EndSite_eye.lY-EndSite_eye.rY", //138
    "EndSite_eye.lX-EndSite_eye.lX", //139
    "EndSite_eye.lY-EndSite_eye.lY", //140
    "EndSite_eye.lX-neckX", //141
    "EndSite_eye.lY-neckY", //142
    "EndSite_eye.lX-virtual_hip_x_plus0_3_y_0X", //143
    "EndSite_eye.lY-virtual_hip_x_plus0_3_y_0Y", //144
    "EndSite_eye.lX-rshoulderX", //145
    "EndSite_eye.lY-rshoulderY", //146
    "EndSite_eye.lX-relbowX", //147
    "EndSite_eye.lY-relbowY", //148
    "EndSite_eye.lX-rhandX", //149
    "EndSite_eye.lY-rhandY", //150
    "EndSite_eye.lX-virtual_hip_x_minus_0_3_y_0X", //151
    "EndSite_eye.lY-virtual_hip_x_minus_0_3_y_0Y", //152
    "EndSite_eye.lX-lshoulderX", //153
    "EndSite_eye.lY-lshoulderY", //154
    "EndSite_eye.lX-lelbowX", //155
    "EndSite_eye.lY-lelbowY", //156
    "EndSite_eye.lX-lhandX", //157
    "EndSite_eye.lY-lhandY", //158
    "EndSite_eye.lX-rhipX", //159
    "EndSite_eye.lY-rhipY", //160
    "EndSite_eye.lX-rkneeX", //161
    "EndSite_eye.lY-rkneeY", //162
    "EndSite_eye.lX-rfootX", //163
    "EndSite_eye.lY-rfootY", //164
    "EndSite_eye.lX-lhipX", //165
    "EndSite_eye.lY-lhipY", //166
    "EndSite_eye.lX-lkneeX", //167
    "EndSite_eye.lY-lkneeY", //168
    "EndSite_eye.lX-lfootX", //169
    "EndSite_eye.lY-lfootY", //170
    "neckX-hipX", //171
    "neckY-hipY", //172
    "neckX-EndSite_eye.rX", //173
    "neckY-EndSite_eye.rY", //174
    "neckX-EndSite_eye.lX", //175
    "neckY-EndSite_eye.lY", //176
    "neckX-neckX", //177
    "neckY-neckY", //178
    "neckX-virtual_hip_x_plus0_3_y_0X", //179
    "neckY-virtual_hip_x_plus0_3_y_0Y", //180
    "neckX-rshoulderX", //181
    "neckY-rshoulderY", //182
    "neckX-relbowX", //183
    "neckY-relbowY", //184
    "neckX-rhandX", //185
    "neckY-rhandY", //186
    "neckX-virtual_hip_x_minus_0_3_y_0X", //187
    "neckY-virtual_hip_x_minus_0_3_y_0Y", //188
    "neckX-lshoulderX", //189
    "neckY-lshoulderY", //190
    "neckX-lelbowX", //191
    "neckY-lelbowY", //192
    "neckX-lhandX", //193
    "neckY-lhandY", //194
    "neckX-rhipX", //195
    "neckY-rhipY", //196
    "neckX-rkneeX", //197
    "neckY-rkneeY", //198
    "neckX-rfootX", //199
    "neckY-rfootY", //200
    "neckX-lhipX", //201
    "neckY-lhipY", //202
    "neckX-lkneeX", //203
    "neckY-lkneeY", //204
    "neckX-lfootX", //205
    "neckY-lfootY", //206
    "virtual_hip_x_plus0_3_y_0X-hipX", //207
    "virtual_hip_x_plus0_3_y_0Y-hipY", //208
    "virtual_hip_x_plus0_3_y_0X-EndSite_eye.rX", //209
    "virtual_hip_x_plus0_3_y_0Y-EndSite_eye.rY", //210
    "virtual_hip_x_plus0_3_y_0X-EndSite_eye.lX", //211
    "virtual_hip_x_plus0_3_y_0Y-EndSite_eye.lY", //212
    "virtual_hip_x_plus0_3_y_0X-neckX", //213
    "virtual_hip_x_plus0_3_y_0Y-neckY", //214
    "virtual_hip_x_plus0_3_y_0X-virtual_hip_x_plus0_3_y_0X", //215
    "virtual_hip_x_plus0_3_y_0Y-virtual_hip_x_plus0_3_y_0Y", //216
    "virtual_hip_x_plus0_3_y_0X-rshoulderX", //217
    "virtual_hip_x_plus0_3_y_0Y-rshoulderY", //218
    "virtual_hip_x_plus0_3_y_0X-relbowX", //219
    "virtual_hip_x_plus0_3_y_0Y-relbowY", //220
    "virtual_hip_x_plus0_3_y_0X-rhandX", //221
    "virtual_hip_x_plus0_3_y_0Y-rhandY", //222
    "virtual_hip_x_plus0_3_y_0X-virtual_hip_x_minus_0_3_y_0X", //223
    "virtual_hip_x_plus0_3_y_0Y-virtual_hip_x_minus_0_3_y_0Y", //224
    "virtual_hip_x_plus0_3_y_0X-lshoulderX", //225
    "virtual_hip_x_plus0_3_y_0Y-lshoulderY", //226
    "virtual_hip_x_plus0_3_y_0X-lelbowX", //227
    "virtual_hip_x_plus0_3_y_0Y-lelbowY", //228
    "virtual_hip_x_plus0_3_y_0X-lhandX", //229
    "virtual_hip_x_plus0_3_y_0Y-lhandY", //230
    "virtual_hip_x_plus0_3_y_0X-rhipX", //231
    "virtual_hip_x_plus0_3_y_0Y-rhipY", //232
    "virtual_hip_x_plus0_3_y_0X-rkneeX", //233
    "virtual_hip_x_plus0_3_y_0Y-rkneeY", //234
    "virtual_hip_x_plus0_3_y_0X-rfootX", //235
    "virtual_hip_x_plus0_3_y_0Y-rfootY", //236
    "virtual_hip_x_plus0_3_y_0X-lhipX", //237
    "virtual_hip_x_plus0_3_y_0Y-lhipY", //238
    "virtual_hip_x_plus0_3_y_0X-lkneeX", //239
    "virtual_hip_x_plus0_3_y_0Y-lkneeY", //240
    "virtual_hip_x_plus0_3_y_0X-lfootX", //241
    "virtual_hip_x_plus0_3_y_0Y-lfootY", //242
    "rshoulderX-hipX", //243
    "rshoulderY-hipY", //244
    "rshoulderX-EndSite_eye.rX", //245
    "rshoulderY-EndSite_eye.rY", //246
    "rshoulderX-EndSite_eye.lX", //247
    "rshoulderY-EndSite_eye.lY", //248
    "rshoulderX-neckX", //249
    "rshoulderY-neckY", //250
    "rshoulderX-virtual_hip_x_plus0_3_y_0X", //251
    "rshoulderY-virtual_hip_x_plus0_3_y_0Y", //252
    "rshoulderX-rshoulderX", //253
    "rshoulderY-rshoulderY", //254
    "rshoulderX-relbowX", //255
    "rshoulderY-relbowY", //256
    "rshoulderX-rhandX", //257
    "rshoulderY-rhandY", //258
    "rshoulderX-virtual_hip_x_minus_0_3_y_0X", //259
    "rshoulderY-virtual_hip_x_minus_0_3_y_0Y", //260
    "rshoulderX-lshoulderX", //261
    "rshoulderY-lshoulderY", //262
    "rshoulderX-lelbowX", //263
    "rshoulderY-lelbowY", //264
    "rshoulderX-lhandX", //265
    "rshoulderY-lhandY", //266
    "rshoulderX-rhipX", //267
    "rshoulderY-rhipY", //268
    "rshoulderX-rkneeX", //269
    "rshoulderY-rkneeY", //270
    "rshoulderX-rfootX", //271
    "rshoulderY-rfootY", //272
    "rshoulderX-lhipX", //273
    "rshoulderY-lhipY", //274
    "rshoulderX-lkneeX", //275
    "rshoulderY-lkneeY", //276
    "rshoulderX-lfootX", //277
    "rshoulderY-lfootY", //278
    "relbowX-hipX", //279
    "relbowY-hipY", //280
    "relbowX-EndSite_eye.rX", //281
    "relbowY-EndSite_eye.rY", //282
    "relbowX-EndSite_eye.lX", //283
    "relbowY-EndSite_eye.lY", //284
    "relbowX-neckX", //285
    "relbowY-neckY", //286
    "relbowX-virtual_hip_x_plus0_3_y_0X", //287
    "relbowY-virtual_hip_x_plus0_3_y_0Y", //288
    "relbowX-rshoulderX", //289
    "relbowY-rshoulderY", //290
    "relbowX-relbowX", //291
    "relbowY-relbowY", //292
    "relbowX-rhandX", //293
    "relbowY-rhandY", //294
    "relbowX-virtual_hip_x_minus_0_3_y_0X", //295
    "relbowY-virtual_hip_x_minus_0_3_y_0Y", //296
    "relbowX-lshoulderX", //297
    "relbowY-lshoulderY", //298
    "relbowX-lelbowX", //299
    "relbowY-lelbowY", //300
    "relbowX-lhandX", //301
    "relbowY-lhandY", //302
    "relbowX-rhipX", //303
    "relbowY-rhipY", //304
    "relbowX-rkneeX", //305
    "relbowY-rkneeY", //306
    "relbowX-rfootX", //307
    "relbowY-rfootY", //308
    "relbowX-lhipX", //309
    "relbowY-lhipY", //310
    "relbowX-lkneeX", //311
    "relbowY-lkneeY", //312
    "relbowX-lfootX", //313
    "relbowY-lfootY", //314
    "rhandX-hipX", //315
    "rhandY-hipY", //316
    "rhandX-EndSite_eye.rX", //317
    "rhandY-EndSite_eye.rY", //318
    "rhandX-EndSite_eye.lX", //319
    "rhandY-EndSite_eye.lY", //320
    "rhandX-neckX", //321
    "rhandY-neckY", //322
    "rhandX-virtual_hip_x_plus0_3_y_0X", //323
    "rhandY-virtual_hip_x_plus0_3_y_0Y", //324
    "rhandX-rshoulderX", //325
    "rhandY-rshoulderY", //326
    "rhandX-relbowX", //327
    "rhandY-relbowY", //328
    "rhandX-rhandX", //329
    "rhandY-rhandY", //330
    "rhandX-virtual_hip_x_minus_0_3_y_0X", //331
    "rhandY-virtual_hip_x_minus_0_3_y_0Y", //332
    "rhandX-lshoulderX", //333
    "rhandY-lshoulderY", //334
    "rhandX-lelbowX", //335
    "rhandY-lelbowY", //336
    "rhandX-lhandX", //337
    "rhandY-lhandY", //338
    "rhandX-rhipX", //339
    "rhandY-rhipY", //340
    "rhandX-rkneeX", //341
    "rhandY-rkneeY", //342
    "rhandX-rfootX", //343
    "rhandY-rfootY", //344
    "rhandX-lhipX", //345
    "rhandY-lhipY", //346
    "rhandX-lkneeX", //347
    "rhandY-lkneeY", //348
    "rhandX-lfootX", //349
    "rhandY-lfootY", //350
    "virtual_hip_x_minus_0_3_y_0X-hipX", //351
    "virtual_hip_x_minus_0_3_y_0Y-hipY", //352
    "virtual_hip_x_minus_0_3_y_0X-EndSite_eye.rX", //353
    "virtual_hip_x_minus_0_3_y_0Y-EndSite_eye.rY", //354
    "virtual_hip_x_minus_0_3_y_0X-EndSite_eye.lX", //355
    "virtual_hip_x_minus_0_3_y_0Y-EndSite_eye.lY", //356
    "virtual_hip_x_minus_0_3_y_0X-neckX", //357
    "virtual_hip_x_minus_0_3_y_0Y-neckY", //358
    "virtual_hip_x_minus_0_3_y_0X-virtual_hip_x_plus0_3_y_0X", //359
    "virtual_hip_x_minus_0_3_y_0Y-virtual_hip_x_plus0_3_y_0Y", //360
    "virtual_hip_x_minus_0_3_y_0X-rshoulderX", //361
    "virtual_hip_x_minus_0_3_y_0Y-rshoulderY", //362
    "virtual_hip_x_minus_0_3_y_0X-relbowX", //363
    "virtual_hip_x_minus_0_3_y_0Y-relbowY", //364
    "virtual_hip_x_minus_0_3_y_0X-rhandX", //365
    "virtual_hip_x_minus_0_3_y_0Y-rhandY", //366
    "virtual_hip_x_minus_0_3_y_0X-virtual_hip_x_minus_0_3_y_0X", //367
    "virtual_hip_x_minus_0_3_y_0Y-virtual_hip_x_minus_0_3_y_0Y", //368
    "virtual_hip_x_minus_0_3_y_0X-lshoulderX", //369
    "virtual_hip_x_minus_0_3_y_0Y-lshoulderY", //370
    "virtual_hip_x_minus_0_3_y_0X-lelbowX", //371
    "virtual_hip_x_minus_0_3_y_0Y-lelbowY", //372
    "virtual_hip_x_minus_0_3_y_0X-lhandX", //373
    "virtual_hip_x_minus_0_3_y_0Y-lhandY", //374
    "virtual_hip_x_minus_0_3_y_0X-rhipX", //375
    "virtual_hip_x_minus_0_3_y_0Y-rhipY", //376
    "virtual_hip_x_minus_0_3_y_0X-rkneeX", //377
    "virtual_hip_x_minus_0_3_y_0Y-rkneeY", //378
    "virtual_hip_x_minus_0_3_y_0X-rfootX", //379
    "virtual_hip_x_minus_0_3_y_0Y-rfootY", //380
    "virtual_hip_x_minus_0_3_y_0X-lhipX", //381
    "virtual_hip_x_minus_0_3_y_0Y-lhipY", //382
    "virtual_hip_x_minus_0_3_y_0X-lkneeX", //383
    "virtual_hip_x_minus_0_3_y_0Y-lkneeY", //384
    "virtual_hip_x_minus_0_3_y_0X-lfootX", //385
    "virtual_hip_x_minus_0_3_y_0Y-lfootY", //386
    "lshoulderX-hipX", //387
    "lshoulderY-hipY", //388
    "lshoulderX-EndSite_eye.rX", //389
    "lshoulderY-EndSite_eye.rY", //390
    "lshoulderX-EndSite_eye.lX", //391
    "lshoulderY-EndSite_eye.lY", //392
    "lshoulderX-neckX", //393
    "lshoulderY-neckY", //394
    "lshoulderX-virtual_hip_x_plus0_3_y_0X", //395
    "lshoulderY-virtual_hip_x_plus0_3_y_0Y", //396
    "lshoulderX-rshoulderX", //397
    "lshoulderY-rshoulderY", //398
    "lshoulderX-relbowX", //399
    "lshoulderY-relbowY", //400
    "lshoulderX-rhandX", //401
    "lshoulderY-rhandY", //402
    "lshoulderX-virtual_hip_x_minus_0_3_y_0X", //403
    "lshoulderY-virtual_hip_x_minus_0_3_y_0Y", //404
    "lshoulderX-lshoulderX", //405
    "lshoulderY-lshoulderY", //406
    "lshoulderX-lelbowX", //407
    "lshoulderY-lelbowY", //408
    "lshoulderX-lhandX", //409
    "lshoulderY-lhandY", //410
    "lshoulderX-rhipX", //411
    "lshoulderY-rhipY", //412
    "lshoulderX-rkneeX", //413
    "lshoulderY-rkneeY", //414
    "lshoulderX-rfootX", //415
    "lshoulderY-rfootY", //416
    "lshoulderX-lhipX", //417
    "lshoulderY-lhipY", //418
    "lshoulderX-lkneeX", //419
    "lshoulderY-lkneeY", //420
    "lshoulderX-lfootX", //421
    "lshoulderY-lfootY", //422
    "lelbowX-hipX", //423
    "lelbowY-hipY", //424
    "lelbowX-EndSite_eye.rX", //425
    "lelbowY-EndSite_eye.rY", //426
    "lelbowX-EndSite_eye.lX", //427
    "lelbowY-EndSite_eye.lY", //428
    "lelbowX-neckX", //429
    "lelbowY-neckY", //430
    "lelbowX-virtual_hip_x_plus0_3_y_0X", //431
    "lelbowY-virtual_hip_x_plus0_3_y_0Y", //432
    "lelbowX-rshoulderX", //433
    "lelbowY-rshoulderY", //434
    "lelbowX-relbowX", //435
    "lelbowY-relbowY", //436
    "lelbowX-rhandX", //437
    "lelbowY-rhandY", //438
    "lelbowX-virtual_hip_x_minus_0_3_y_0X", //439
    "lelbowY-virtual_hip_x_minus_0_3_y_0Y", //440
    "lelbowX-lshoulderX", //441
    "lelbowY-lshoulderY", //442
    "lelbowX-lelbowX", //443
    "lelbowY-lelbowY", //444
    "lelbowX-lhandX", //445
    "lelbowY-lhandY", //446
    "lelbowX-rhipX", //447
    "lelbowY-rhipY", //448
    "lelbowX-rkneeX", //449
    "lelbowY-rkneeY", //450
    "lelbowX-rfootX", //451
    "lelbowY-rfootY", //452
    "lelbowX-lhipX", //453
    "lelbowY-lhipY", //454
    "lelbowX-lkneeX", //455
    "lelbowY-lkneeY", //456
    "lelbowX-lfootX", //457
    "lelbowY-lfootY", //458
    "lhandX-hipX", //459
    "lhandY-hipY", //460
    "lhandX-EndSite_eye.rX", //461
    "lhandY-EndSite_eye.rY", //462
    "lhandX-EndSite_eye.lX", //463
    "lhandY-EndSite_eye.lY", //464
    "lhandX-neckX", //465
    "lhandY-neckY", //466
    "lhandX-virtual_hip_x_plus0_3_y_0X", //467
    "lhandY-virtual_hip_x_plus0_3_y_0Y", //468
    "lhandX-rshoulderX", //469
    "lhandY-rshoulderY", //470
    "lhandX-relbowX", //471
    "lhandY-relbowY", //472
    "lhandX-rhandX", //473
    "lhandY-rhandY", //474
    "lhandX-virtual_hip_x_minus_0_3_y_0X", //475
    "lhandY-virtual_hip_x_minus_0_3_y_0Y", //476
    "lhandX-lshoulderX", //477
    "lhandY-lshoulderY", //478
    "lhandX-lelbowX", //479
    "lhandY-lelbowY", //480
    "lhandX-lhandX", //481
    "lhandY-lhandY", //482
    "lhandX-rhipX", //483
    "lhandY-rhipY", //484
    "lhandX-rkneeX", //485
    "lhandY-rkneeY", //486
    "lhandX-rfootX", //487
    "lhandY-rfootY", //488
    "lhandX-lhipX", //489
    "lhandY-lhipY", //490
    "lhandX-lkneeX", //491
    "lhandY-lkneeY", //492
    "lhandX-lfootX", //493
    "lhandY-lfootY", //494
    "rhipX-hipX", //495
    "rhipY-hipY", //496
    "rhipX-EndSite_eye.rX", //497
    "rhipY-EndSite_eye.rY", //498
    "rhipX-EndSite_eye.lX", //499
    "rhipY-EndSite_eye.lY", //500
    "rhipX-neckX", //501
    "rhipY-neckY", //502
    "rhipX-virtual_hip_x_plus0_3_y_0X", //503
    "rhipY-virtual_hip_x_plus0_3_y_0Y", //504
    "rhipX-rshoulderX", //505
    "rhipY-rshoulderY", //506
    "rhipX-relbowX", //507
    "rhipY-relbowY", //508
    "rhipX-rhandX", //509
    "rhipY-rhandY", //510
    "rhipX-virtual_hip_x_minus_0_3_y_0X", //511
    "rhipY-virtual_hip_x_minus_0_3_y_0Y", //512
    "rhipX-lshoulderX", //513
    "rhipY-lshoulderY", //514
    "rhipX-lelbowX", //515
    "rhipY-lelbowY", //516
    "rhipX-lhandX", //517
    "rhipY-lhandY", //518
    "rhipX-rhipX", //519
    "rhipY-rhipY", //520
    "rhipX-rkneeX", //521
    "rhipY-rkneeY", //522
    "rhipX-rfootX", //523
    "rhipY-rfootY", //524
    "rhipX-lhipX", //525
    "rhipY-lhipY", //526
    "rhipX-lkneeX", //527
    "rhipY-lkneeY", //528
    "rhipX-lfootX", //529
    "rhipY-lfootY", //530
    "rkneeX-hipX", //531
    "rkneeY-hipY", //532
    "rkneeX-EndSite_eye.rX", //533
    "rkneeY-EndSite_eye.rY", //534
    "rkneeX-EndSite_eye.lX", //535
    "rkneeY-EndSite_eye.lY", //536
    "rkneeX-neckX", //537
    "rkneeY-neckY", //538
    "rkneeX-virtual_hip_x_plus0_3_y_0X", //539
    "rkneeY-virtual_hip_x_plus0_3_y_0Y", //540
    "rkneeX-rshoulderX", //541
    "rkneeY-rshoulderY", //542
    "rkneeX-relbowX", //543
    "rkneeY-relbowY", //544
    "rkneeX-rhandX", //545
    "rkneeY-rhandY", //546
    "rkneeX-virtual_hip_x_minus_0_3_y_0X", //547
    "rkneeY-virtual_hip_x_minus_0_3_y_0Y", //548
    "rkneeX-lshoulderX", //549
    "rkneeY-lshoulderY", //550
    "rkneeX-lelbowX", //551
    "rkneeY-lelbowY", //552
    "rkneeX-lhandX", //553
    "rkneeY-lhandY", //554
    "rkneeX-rhipX", //555
    "rkneeY-rhipY", //556
    "rkneeX-rkneeX", //557
    "rkneeY-rkneeY", //558
    "rkneeX-rfootX", //559
    "rkneeY-rfootY", //560
    "rkneeX-lhipX", //561
    "rkneeY-lhipY", //562
    "rkneeX-lkneeX", //563
    "rkneeY-lkneeY", //564
    "rkneeX-lfootX", //565
    "rkneeY-lfootY", //566
    "rfootX-hipX", //567
    "rfootY-hipY", //568
    "rfootX-EndSite_eye.rX", //569
    "rfootY-EndSite_eye.rY", //570
    "rfootX-EndSite_eye.lX", //571
    "rfootY-EndSite_eye.lY", //572
    "rfootX-neckX", //573
    "rfootY-neckY", //574
    "rfootX-virtual_hip_x_plus0_3_y_0X", //575
    "rfootY-virtual_hip_x_plus0_3_y_0Y", //576
    "rfootX-rshoulderX", //577
    "rfootY-rshoulderY", //578
    "rfootX-relbowX", //579
    "rfootY-relbowY", //580
    "rfootX-rhandX", //581
    "rfootY-rhandY", //582
    "rfootX-virtual_hip_x_minus_0_3_y_0X", //583
    "rfootY-virtual_hip_x_minus_0_3_y_0Y", //584
    "rfootX-lshoulderX", //585
    "rfootY-lshoulderY", //586
    "rfootX-lelbowX", //587
    "rfootY-lelbowY", //588
    "rfootX-lhandX", //589
    "rfootY-lhandY", //590
    "rfootX-rhipX", //591
    "rfootY-rhipY", //592
    "rfootX-rkneeX", //593
    "rfootY-rkneeY", //594
    "rfootX-rfootX", //595
    "rfootY-rfootY", //596
    "rfootX-lhipX", //597
    "rfootY-lhipY", //598
    "rfootX-lkneeX", //599
    "rfootY-lkneeY", //600
    "rfootX-lfootX", //601
    "rfootY-lfootY", //602
    "lhipX-hipX", //603
    "lhipY-hipY", //604
    "lhipX-EndSite_eye.rX", //605
    "lhipY-EndSite_eye.rY", //606
    "lhipX-EndSite_eye.lX", //607
    "lhipY-EndSite_eye.lY", //608
    "lhipX-neckX", //609
    "lhipY-neckY", //610
    "lhipX-virtual_hip_x_plus0_3_y_0X", //611
    "lhipY-virtual_hip_x_plus0_3_y_0Y", //612
    "lhipX-rshoulderX", //613
    "lhipY-rshoulderY", //614
    "lhipX-relbowX", //615
    "lhipY-relbowY", //616
    "lhipX-rhandX", //617
    "lhipY-rhandY", //618
    "lhipX-virtual_hip_x_minus_0_3_y_0X", //619
    "lhipY-virtual_hip_x_minus_0_3_y_0Y", //620
    "lhipX-lshoulderX", //621
    "lhipY-lshoulderY", //622
    "lhipX-lelbowX", //623
    "lhipY-lelbowY", //624
    "lhipX-lhandX", //625
    "lhipY-lhandY", //626
    "lhipX-rhipX", //627
    "lhipY-rhipY", //628
    "lhipX-rkneeX", //629
    "lhipY-rkneeY", //630
    "lhipX-rfootX", //631
    "lhipY-rfootY", //632
    "lhipX-lhipX", //633
    "lhipY-lhipY", //634
    "lhipX-lkneeX", //635
    "lhipY-lkneeY", //636
    "lhipX-lfootX", //637
    "lhipY-lfootY", //638
    "lkneeX-hipX", //639
    "lkneeY-hipY", //640
    "lkneeX-EndSite_eye.rX", //641
    "lkneeY-EndSite_eye.rY", //642
    "lkneeX-EndSite_eye.lX", //643
    "lkneeY-EndSite_eye.lY", //644
    "lkneeX-neckX", //645
    "lkneeY-neckY", //646
    "lkneeX-virtual_hip_x_plus0_3_y_0X", //647
    "lkneeY-virtual_hip_x_plus0_3_y_0Y", //648
    "lkneeX-rshoulderX", //649
    "lkneeY-rshoulderY", //650
    "lkneeX-relbowX", //651
    "lkneeY-relbowY", //652
    "lkneeX-rhandX", //653
    "lkneeY-rhandY", //654
    "lkneeX-virtual_hip_x_minus_0_3_y_0X", //655
    "lkneeY-virtual_hip_x_minus_0_3_y_0Y", //656
    "lkneeX-lshoulderX", //657
    "lkneeY-lshoulderY", //658
    "lkneeX-lelbowX", //659
    "lkneeY-lelbowY", //660
    "lkneeX-lhandX", //661
    "lkneeY-lhandY", //662
    "lkneeX-rhipX", //663
    "lkneeY-rhipY", //664
    "lkneeX-rkneeX", //665
    "lkneeY-rkneeY", //666
    "lkneeX-rfootX", //667
    "lkneeY-rfootY", //668
    "lkneeX-lhipX", //669
    "lkneeY-lhipY", //670
    "lkneeX-lkneeX", //671
    "lkneeY-lkneeY", //672
    "lkneeX-lfootX", //673
    "lkneeY-lfootY", //674
    "lfootX-hipX", //675
    "lfootY-hipY", //676
    "lfootX-EndSite_eye.rX", //677
    "lfootY-EndSite_eye.rY", //678
    "lfootX-EndSite_eye.lX", //679
    "lfootY-EndSite_eye.lY", //680
    "lfootX-neckX", //681
    "lfootY-neckY", //682
    "lfootX-virtual_hip_x_plus0_3_y_0X", //683
    "lfootY-virtual_hip_x_plus0_3_y_0Y", //684
    "lfootX-rshoulderX", //685
    "lfootY-rshoulderY", //686
    "lfootX-relbowX", //687
    "lfootY-relbowY", //688
    "lfootX-rhandX", //689
    "lfootY-rhandY", //690
    "lfootX-virtual_hip_x_minus_0_3_y_0X", //691
    "lfootY-virtual_hip_x_minus_0_3_y_0Y", //692
    "lfootX-lshoulderX", //693
    "lfootY-lshoulderY", //694
    "lfootX-lelbowX", //695
    "lfootY-lelbowY", //696
    "lfootX-lhandX", //697
    "lfootY-lhandY", //698
    "lfootX-rhipX", //699
    "lfootY-rhipY", //700
    "lfootX-rkneeX", //701
    "lfootY-rkneeY", //702
    "lfootX-rfootX", //703
    "lfootY-rfootY", //704
    "lfootX-lhipX", //705
    "lfootY-lhipY", //706
    "lfootX-lkneeX", //707
    "lfootY-lkneeY", //708
    "lfootX-lfootX", //709
    "lfootY-lfootY", //710
    "end"
};
/** @brief Programmer friendly enumerator of expected inputs*/
enum mocapNET_body_enum
{
    MNET_BODY_IN_2DX_HIP = 0, //0
    MNET_BODY_IN_2DY_HIP, //1
    MNET_BODY_IN_VISIBLE_HIP, //2
    MNET_BODY_IN_2DX_NECK, //3
    MNET_BODY_IN_2DY_NECK, //4
    MNET_BODY_IN_VISIBLE_NECK, //5
    MNET_BODY_IN_2DX_HEAD, //6
    MNET_BODY_IN_2DY_HEAD, //7
    MNET_BODY_IN_VISIBLE_HEAD, //8
    MNET_BODY_IN_2DX_ENDSITE_EYE_L, //9
    MNET_BODY_IN_2DY_ENDSITE_EYE_L, //10
    MNET_BODY_IN_VISIBLE_ENDSITE_EYE_L, //11
    MNET_BODY_IN_2DX_ENDSITE_EYE_R, //12
    MNET_BODY_IN_2DY_ENDSITE_EYE_R, //13
    MNET_BODY_IN_VISIBLE_ENDSITE_EYE_R, //14
    MNET_BODY_IN_2DX_RSHOULDER, //15
    MNET_BODY_IN_2DY_RSHOULDER, //16
    MNET_BODY_IN_VISIBLE_RSHOULDER, //17
    MNET_BODY_IN_2DX_RELBOW, //18
    MNET_BODY_IN_2DY_RELBOW, //19
    MNET_BODY_IN_VISIBLE_RELBOW, //20
    MNET_BODY_IN_2DX_RHAND, //21
    MNET_BODY_IN_2DY_RHAND, //22
    MNET_BODY_IN_VISIBLE_RHAND, //23
    MNET_BODY_IN_2DX_LSHOULDER, //24
    MNET_BODY_IN_2DY_LSHOULDER, //25
    MNET_BODY_IN_VISIBLE_LSHOULDER, //26
    MNET_BODY_IN_2DX_LELBOW, //27
    MNET_BODY_IN_2DY_LELBOW, //28
    MNET_BODY_IN_VISIBLE_LELBOW, //29
    MNET_BODY_IN_2DX_LHAND, //30
    MNET_BODY_IN_2DY_LHAND, //31
    MNET_BODY_IN_VISIBLE_LHAND, //32
    MNET_BODY_IN_2DX_RHIP, //33
    MNET_BODY_IN_2DY_RHIP, //34
    MNET_BODY_IN_VISIBLE_RHIP, //35
    MNET_BODY_IN_2DX_RKNEE, //36
    MNET_BODY_IN_2DY_RKNEE, //37
    MNET_BODY_IN_VISIBLE_RKNEE, //38
    MNET_BODY_IN_2DX_RFOOT, //39
    MNET_BODY_IN_2DY_RFOOT, //40
    MNET_BODY_IN_VISIBLE_RFOOT, //41
    MNET_BODY_IN_2DX_ENDSITE_TOE1_2_R, //42
    MNET_BODY_IN_2DY_ENDSITE_TOE1_2_R, //43
    MNET_BODY_IN_VISIBLE_ENDSITE_TOE1_2_R, //44
    MNET_BODY_IN_2DX_ENDSITE_TOE5_3_R, //45
    MNET_BODY_IN_2DY_ENDSITE_TOE5_3_R, //46
    MNET_BODY_IN_VISIBLE_ENDSITE_TOE5_3_R, //47
    MNET_BODY_IN_2DX_LHIP, //48
    MNET_BODY_IN_2DY_LHIP, //49
    MNET_BODY_IN_VISIBLE_LHIP, //50
    MNET_BODY_IN_2DX_LKNEE, //51
    MNET_BODY_IN_2DY_LKNEE, //52
    MNET_BODY_IN_VISIBLE_LKNEE, //53
    MNET_BODY_IN_2DX_LFOOT, //54
    MNET_BODY_IN_2DY_LFOOT, //55
    MNET_BODY_IN_VISIBLE_LFOOT, //56
    MNET_BODY_IN_2DX_ENDSITE_TOE1_2_L, //57
    MNET_BODY_IN_2DY_ENDSITE_TOE1_2_L, //58
    MNET_BODY_IN_VISIBLE_ENDSITE_TOE1_2_L, //59
    MNET_BODY_IN_2DX_ENDSITE_TOE5_3_L, //60
    MNET_BODY_IN_2DY_ENDSITE_TOE5_3_L, //61
    MNET_BODY_IN_VISIBLE_ENDSITE_TOE5_3_L, //62
    MNET_BODY_IN_HIPX_HIPX, //63
    MNET_BODY_IN_HIPY_HIPY, //64
    MNET_BODY_IN_HIPX_ENDSITE_EYE_RX, //65
    MNET_BODY_IN_HIPY_ENDSITE_EYE_RY, //66
    MNET_BODY_IN_HIPX_ENDSITE_EYE_LX, //67
    MNET_BODY_IN_HIPY_ENDSITE_EYE_LY, //68
    MNET_BODY_IN_HIPX_NECKX, //69
    MNET_BODY_IN_HIPY_NECKY, //70
    MNET_BODY_IN_HIPX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //71
    MNET_BODY_IN_HIPY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //72
    MNET_BODY_IN_HIPX_RSHOULDERX, //73
    MNET_BODY_IN_HIPY_RSHOULDERY, //74
    MNET_BODY_IN_HIPX_RELBOWX, //75
    MNET_BODY_IN_HIPY_RELBOWY, //76
    MNET_BODY_IN_HIPX_RHANDX, //77
    MNET_BODY_IN_HIPY_RHANDY, //78
    MNET_BODY_IN_HIPX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //79
    MNET_BODY_IN_HIPY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //80
    MNET_BODY_IN_HIPX_LSHOULDERX, //81
    MNET_BODY_IN_HIPY_LSHOULDERY, //82
    MNET_BODY_IN_HIPX_LELBOWX, //83
    MNET_BODY_IN_HIPY_LELBOWY, //84
    MNET_BODY_IN_HIPX_LHANDX, //85
    MNET_BODY_IN_HIPY_LHANDY, //86
    MNET_BODY_IN_HIPX_RHIPX, //87
    MNET_BODY_IN_HIPY_RHIPY, //88
    MNET_BODY_IN_HIPX_RKNEEX, //89
    MNET_BODY_IN_HIPY_RKNEEY, //90
    MNET_BODY_IN_HIPX_RFOOTX, //91
    MNET_BODY_IN_HIPY_RFOOTY, //92
    MNET_BODY_IN_HIPX_LHIPX, //93
    MNET_BODY_IN_HIPY_LHIPY, //94
    MNET_BODY_IN_HIPX_LKNEEX, //95
    MNET_BODY_IN_HIPY_LKNEEY, //96
    MNET_BODY_IN_HIPX_LFOOTX, //97
    MNET_BODY_IN_HIPY_LFOOTY, //98
    MNET_BODY_IN_ENDSITE_EYE_RX_HIPX, //99
    MNET_BODY_IN_ENDSITE_EYE_RY_HIPY, //100
    MNET_BODY_IN_ENDSITE_EYE_RX_ENDSITE_EYE_RX, //101
    MNET_BODY_IN_ENDSITE_EYE_RY_ENDSITE_EYE_RY, //102
    MNET_BODY_IN_ENDSITE_EYE_RX_ENDSITE_EYE_LX, //103
    MNET_BODY_IN_ENDSITE_EYE_RY_ENDSITE_EYE_LY, //104
    MNET_BODY_IN_ENDSITE_EYE_RX_NECKX, //105
    MNET_BODY_IN_ENDSITE_EYE_RY_NECKY, //106
    MNET_BODY_IN_ENDSITE_EYE_RX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //107
    MNET_BODY_IN_ENDSITE_EYE_RY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //108
    MNET_BODY_IN_ENDSITE_EYE_RX_RSHOULDERX, //109
    MNET_BODY_IN_ENDSITE_EYE_RY_RSHOULDERY, //110
    MNET_BODY_IN_ENDSITE_EYE_RX_RELBOWX, //111
    MNET_BODY_IN_ENDSITE_EYE_RY_RELBOWY, //112
    MNET_BODY_IN_ENDSITE_EYE_RX_RHANDX, //113
    MNET_BODY_IN_ENDSITE_EYE_RY_RHANDY, //114
    MNET_BODY_IN_ENDSITE_EYE_RX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //115
    MNET_BODY_IN_ENDSITE_EYE_RY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //116
    MNET_BODY_IN_ENDSITE_EYE_RX_LSHOULDERX, //117
    MNET_BODY_IN_ENDSITE_EYE_RY_LSHOULDERY, //118
    MNET_BODY_IN_ENDSITE_EYE_RX_LELBOWX, //119
    MNET_BODY_IN_ENDSITE_EYE_RY_LELBOWY, //120
    MNET_BODY_IN_ENDSITE_EYE_RX_LHANDX, //121
    MNET_BODY_IN_ENDSITE_EYE_RY_LHANDY, //122
    MNET_BODY_IN_ENDSITE_EYE_RX_RHIPX, //123
    MNET_BODY_IN_ENDSITE_EYE_RY_RHIPY, //124
    MNET_BODY_IN_ENDSITE_EYE_RX_RKNEEX, //125
    MNET_BODY_IN_ENDSITE_EYE_RY_RKNEEY, //126
    MNET_BODY_IN_ENDSITE_EYE_RX_RFOOTX, //127
    MNET_BODY_IN_ENDSITE_EYE_RY_RFOOTY, //128
    MNET_BODY_IN_ENDSITE_EYE_RX_LHIPX, //129
    MNET_BODY_IN_ENDSITE_EYE_RY_LHIPY, //130
    MNET_BODY_IN_ENDSITE_EYE_RX_LKNEEX, //131
    MNET_BODY_IN_ENDSITE_EYE_RY_LKNEEY, //132
    MNET_BODY_IN_ENDSITE_EYE_RX_LFOOTX, //133
    MNET_BODY_IN_ENDSITE_EYE_RY_LFOOTY, //134
    MNET_BODY_IN_ENDSITE_EYE_LX_HIPX, //135
    MNET_BODY_IN_ENDSITE_EYE_LY_HIPY, //136
    MNET_BODY_IN_ENDSITE_EYE_LX_ENDSITE_EYE_RX, //137
    MNET_BODY_IN_ENDSITE_EYE_LY_ENDSITE_EYE_RY, //138
    MNET_BODY_IN_ENDSITE_EYE_LX_ENDSITE_EYE_LX, //139
    MNET_BODY_IN_ENDSITE_EYE_LY_ENDSITE_EYE_LY, //140
    MNET_BODY_IN_ENDSITE_EYE_LX_NECKX, //141
    MNET_BODY_IN_ENDSITE_EYE_LY_NECKY, //142
    MNET_BODY_IN_ENDSITE_EYE_LX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //143
    MNET_BODY_IN_ENDSITE_EYE_LY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //144
    MNET_BODY_IN_ENDSITE_EYE_LX_RSHOULDERX, //145
    MNET_BODY_IN_ENDSITE_EYE_LY_RSHOULDERY, //146
    MNET_BODY_IN_ENDSITE_EYE_LX_RELBOWX, //147
    MNET_BODY_IN_ENDSITE_EYE_LY_RELBOWY, //148
    MNET_BODY_IN_ENDSITE_EYE_LX_RHANDX, //149
    MNET_BODY_IN_ENDSITE_EYE_LY_RHANDY, //150
    MNET_BODY_IN_ENDSITE_EYE_LX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //151
    MNET_BODY_IN_ENDSITE_EYE_LY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //152
    MNET_BODY_IN_ENDSITE_EYE_LX_LSHOULDERX, //153
    MNET_BODY_IN_ENDSITE_EYE_LY_LSHOULDERY, //154
    MNET_BODY_IN_ENDSITE_EYE_LX_LELBOWX, //155
    MNET_BODY_IN_ENDSITE_EYE_LY_LELBOWY, //156
    MNET_BODY_IN_ENDSITE_EYE_LX_LHANDX, //157
    MNET_BODY_IN_ENDSITE_EYE_LY_LHANDY, //158
    MNET_BODY_IN_ENDSITE_EYE_LX_RHIPX, //159
    MNET_BODY_IN_ENDSITE_EYE_LY_RHIPY, //160
    MNET_BODY_IN_ENDSITE_EYE_LX_RKNEEX, //161
    MNET_BODY_IN_ENDSITE_EYE_LY_RKNEEY, //162
    MNET_BODY_IN_ENDSITE_EYE_LX_RFOOTX, //163
    MNET_BODY_IN_ENDSITE_EYE_LY_RFOOTY, //164
    MNET_BODY_IN_ENDSITE_EYE_LX_LHIPX, //165
    MNET_BODY_IN_ENDSITE_EYE_LY_LHIPY, //166
    MNET_BODY_IN_ENDSITE_EYE_LX_LKNEEX, //167
    MNET_BODY_IN_ENDSITE_EYE_LY_LKNEEY, //168
    MNET_BODY_IN_ENDSITE_EYE_LX_LFOOTX, //169
    MNET_BODY_IN_ENDSITE_EYE_LY_LFOOTY, //170
    MNET_BODY_IN_NECKX_HIPX, //171
    MNET_BODY_IN_NECKY_HIPY, //172
    MNET_BODY_IN_NECKX_ENDSITE_EYE_RX, //173
    MNET_BODY_IN_NECKY_ENDSITE_EYE_RY, //174
    MNET_BODY_IN_NECKX_ENDSITE_EYE_LX, //175
    MNET_BODY_IN_NECKY_ENDSITE_EYE_LY, //176
    MNET_BODY_IN_NECKX_NECKX, //177
    MNET_BODY_IN_NECKY_NECKY, //178
    MNET_BODY_IN_NECKX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //179
    MNET_BODY_IN_NECKY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //180
    MNET_BODY_IN_NECKX_RSHOULDERX, //181
    MNET_BODY_IN_NECKY_RSHOULDERY, //182
    MNET_BODY_IN_NECKX_RELBOWX, //183
    MNET_BODY_IN_NECKY_RELBOWY, //184
    MNET_BODY_IN_NECKX_RHANDX, //185
    MNET_BODY_IN_NECKY_RHANDY, //186
    MNET_BODY_IN_NECKX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //187
    MNET_BODY_IN_NECKY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //188
    MNET_BODY_IN_NECKX_LSHOULDERX, //189
    MNET_BODY_IN_NECKY_LSHOULDERY, //190
    MNET_BODY_IN_NECKX_LELBOWX, //191
    MNET_BODY_IN_NECKY_LELBOWY, //192
    MNET_BODY_IN_NECKX_LHANDX, //193
    MNET_BODY_IN_NECKY_LHANDY, //194
    MNET_BODY_IN_NECKX_RHIPX, //195
    MNET_BODY_IN_NECKY_RHIPY, //196
    MNET_BODY_IN_NECKX_RKNEEX, //197
    MNET_BODY_IN_NECKY_RKNEEY, //198
    MNET_BODY_IN_NECKX_RFOOTX, //199
    MNET_BODY_IN_NECKY_RFOOTY, //200
    MNET_BODY_IN_NECKX_LHIPX, //201
    MNET_BODY_IN_NECKY_LHIPY, //202
    MNET_BODY_IN_NECKX_LKNEEX, //203
    MNET_BODY_IN_NECKY_LKNEEY, //204
    MNET_BODY_IN_NECKX_LFOOTX, //205
    MNET_BODY_IN_NECKY_LFOOTY, //206
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_HIPX, //207
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_HIPY, //208
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_ENDSITE_EYE_RX, //209
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ENDSITE_EYE_RY, //210
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_ENDSITE_EYE_LX, //211
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_ENDSITE_EYE_LY, //212
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_NECKX, //213
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_NECKY, //214
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //215
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //216
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_RSHOULDERX, //217
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_RSHOULDERY, //218
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_RELBOWX, //219
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_RELBOWY, //220
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_RHANDX, //221
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_RHANDY, //222
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //223
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //224
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_LSHOULDERX, //225
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_LSHOULDERY, //226
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_LELBOWX, //227
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_LELBOWY, //228
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_LHANDX, //229
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_LHANDY, //230
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_RHIPX, //231
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_RHIPY, //232
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_RKNEEX, //233
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_RKNEEY, //234
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_RFOOTX, //235
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_RFOOTY, //236
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_LHIPX, //237
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_LHIPY, //238
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_LKNEEX, //239
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_LKNEEY, //240
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0X_LFOOTX, //241
    MNET_BODY_IN_VIRTUAL_HIP_X_PLUS0_3_Y_0Y_LFOOTY, //242
    MNET_BODY_IN_RSHOULDERX_HIPX, //243
    MNET_BODY_IN_RSHOULDERY_HIPY, //244
    MNET_BODY_IN_RSHOULDERX_ENDSITE_EYE_RX, //245
    MNET_BODY_IN_RSHOULDERY_ENDSITE_EYE_RY, //246
    MNET_BODY_IN_RSHOULDERX_ENDSITE_EYE_LX, //247
    MNET_BODY_IN_RSHOULDERY_ENDSITE_EYE_LY, //248
    MNET_BODY_IN_RSHOULDERX_NECKX, //249
    MNET_BODY_IN_RSHOULDERY_NECKY, //250
    MNET_BODY_IN_RSHOULDERX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //251
    MNET_BODY_IN_RSHOULDERY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //252
    MNET_BODY_IN_RSHOULDERX_RSHOULDERX, //253
    MNET_BODY_IN_RSHOULDERY_RSHOULDERY, //254
    MNET_BODY_IN_RSHOULDERX_RELBOWX, //255
    MNET_BODY_IN_RSHOULDERY_RELBOWY, //256
    MNET_BODY_IN_RSHOULDERX_RHANDX, //257
    MNET_BODY_IN_RSHOULDERY_RHANDY, //258
    MNET_BODY_IN_RSHOULDERX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //259
    MNET_BODY_IN_RSHOULDERY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //260
    MNET_BODY_IN_RSHOULDERX_LSHOULDERX, //261
    MNET_BODY_IN_RSHOULDERY_LSHOULDERY, //262
    MNET_BODY_IN_RSHOULDERX_LELBOWX, //263
    MNET_BODY_IN_RSHOULDERY_LELBOWY, //264
    MNET_BODY_IN_RSHOULDERX_LHANDX, //265
    MNET_BODY_IN_RSHOULDERY_LHANDY, //266
    MNET_BODY_IN_RSHOULDERX_RHIPX, //267
    MNET_BODY_IN_RSHOULDERY_RHIPY, //268
    MNET_BODY_IN_RSHOULDERX_RKNEEX, //269
    MNET_BODY_IN_RSHOULDERY_RKNEEY, //270
    MNET_BODY_IN_RSHOULDERX_RFOOTX, //271
    MNET_BODY_IN_RSHOULDERY_RFOOTY, //272
    MNET_BODY_IN_RSHOULDERX_LHIPX, //273
    MNET_BODY_IN_RSHOULDERY_LHIPY, //274
    MNET_BODY_IN_RSHOULDERX_LKNEEX, //275
    MNET_BODY_IN_RSHOULDERY_LKNEEY, //276
    MNET_BODY_IN_RSHOULDERX_LFOOTX, //277
    MNET_BODY_IN_RSHOULDERY_LFOOTY, //278
    MNET_BODY_IN_RELBOWX_HIPX, //279
    MNET_BODY_IN_RELBOWY_HIPY, //280
    MNET_BODY_IN_RELBOWX_ENDSITE_EYE_RX, //281
    MNET_BODY_IN_RELBOWY_ENDSITE_EYE_RY, //282
    MNET_BODY_IN_RELBOWX_ENDSITE_EYE_LX, //283
    MNET_BODY_IN_RELBOWY_ENDSITE_EYE_LY, //284
    MNET_BODY_IN_RELBOWX_NECKX, //285
    MNET_BODY_IN_RELBOWY_NECKY, //286
    MNET_BODY_IN_RELBOWX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //287
    MNET_BODY_IN_RELBOWY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //288
    MNET_BODY_IN_RELBOWX_RSHOULDERX, //289
    MNET_BODY_IN_RELBOWY_RSHOULDERY, //290
    MNET_BODY_IN_RELBOWX_RELBOWX, //291
    MNET_BODY_IN_RELBOWY_RELBOWY, //292
    MNET_BODY_IN_RELBOWX_RHANDX, //293
    MNET_BODY_IN_RELBOWY_RHANDY, //294
    MNET_BODY_IN_RELBOWX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //295
    MNET_BODY_IN_RELBOWY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //296
    MNET_BODY_IN_RELBOWX_LSHOULDERX, //297
    MNET_BODY_IN_RELBOWY_LSHOULDERY, //298
    MNET_BODY_IN_RELBOWX_LELBOWX, //299
    MNET_BODY_IN_RELBOWY_LELBOWY, //300
    MNET_BODY_IN_RELBOWX_LHANDX, //301
    MNET_BODY_IN_RELBOWY_LHANDY, //302
    MNET_BODY_IN_RELBOWX_RHIPX, //303
    MNET_BODY_IN_RELBOWY_RHIPY, //304
    MNET_BODY_IN_RELBOWX_RKNEEX, //305
    MNET_BODY_IN_RELBOWY_RKNEEY, //306
    MNET_BODY_IN_RELBOWX_RFOOTX, //307
    MNET_BODY_IN_RELBOWY_RFOOTY, //308
    MNET_BODY_IN_RELBOWX_LHIPX, //309
    MNET_BODY_IN_RELBOWY_LHIPY, //310
    MNET_BODY_IN_RELBOWX_LKNEEX, //311
    MNET_BODY_IN_RELBOWY_LKNEEY, //312
    MNET_BODY_IN_RELBOWX_LFOOTX, //313
    MNET_BODY_IN_RELBOWY_LFOOTY, //314
    MNET_BODY_IN_RHANDX_HIPX, //315
    MNET_BODY_IN_RHANDY_HIPY, //316
    MNET_BODY_IN_RHANDX_ENDSITE_EYE_RX, //317
    MNET_BODY_IN_RHANDY_ENDSITE_EYE_RY, //318
    MNET_BODY_IN_RHANDX_ENDSITE_EYE_LX, //319
    MNET_BODY_IN_RHANDY_ENDSITE_EYE_LY, //320
    MNET_BODY_IN_RHANDX_NECKX, //321
    MNET_BODY_IN_RHANDY_NECKY, //322
    MNET_BODY_IN_RHANDX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //323
    MNET_BODY_IN_RHANDY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //324
    MNET_BODY_IN_RHANDX_RSHOULDERX, //325
    MNET_BODY_IN_RHANDY_RSHOULDERY, //326
    MNET_BODY_IN_RHANDX_RELBOWX, //327
    MNET_BODY_IN_RHANDY_RELBOWY, //328
    MNET_BODY_IN_RHANDX_RHANDX, //329
    MNET_BODY_IN_RHANDY_RHANDY, //330
    MNET_BODY_IN_RHANDX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //331
    MNET_BODY_IN_RHANDY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //332
    MNET_BODY_IN_RHANDX_LSHOULDERX, //333
    MNET_BODY_IN_RHANDY_LSHOULDERY, //334
    MNET_BODY_IN_RHANDX_LELBOWX, //335
    MNET_BODY_IN_RHANDY_LELBOWY, //336
    MNET_BODY_IN_RHANDX_LHANDX, //337
    MNET_BODY_IN_RHANDY_LHANDY, //338
    MNET_BODY_IN_RHANDX_RHIPX, //339
    MNET_BODY_IN_RHANDY_RHIPY, //340
    MNET_BODY_IN_RHANDX_RKNEEX, //341
    MNET_BODY_IN_RHANDY_RKNEEY, //342
    MNET_BODY_IN_RHANDX_RFOOTX, //343
    MNET_BODY_IN_RHANDY_RFOOTY, //344
    MNET_BODY_IN_RHANDX_LHIPX, //345
    MNET_BODY_IN_RHANDY_LHIPY, //346
    MNET_BODY_IN_RHANDX_LKNEEX, //347
    MNET_BODY_IN_RHANDY_LKNEEY, //348
    MNET_BODY_IN_RHANDX_LFOOTX, //349
    MNET_BODY_IN_RHANDY_LFOOTY, //350
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_HIPX, //351
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_HIPY, //352
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_ENDSITE_EYE_RX, //353
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ENDSITE_EYE_RY, //354
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_ENDSITE_EYE_LX, //355
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_ENDSITE_EYE_LY, //356
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_NECKX, //357
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_NECKY, //358
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //359
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //360
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_RSHOULDERX, //361
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_RSHOULDERY, //362
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_RELBOWX, //363
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_RELBOWY, //364
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_RHANDX, //365
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_RHANDY, //366
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //367
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //368
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_LSHOULDERX, //369
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_LSHOULDERY, //370
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_LELBOWX, //371
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_LELBOWY, //372
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_LHANDX, //373
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_LHANDY, //374
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_RHIPX, //375
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_RHIPY, //376
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_RKNEEX, //377
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_RKNEEY, //378
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_RFOOTX, //379
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_RFOOTY, //380
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_LHIPX, //381
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_LHIPY, //382
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_LKNEEX, //383
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_LKNEEY, //384
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0X_LFOOTX, //385
    MNET_BODY_IN_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y_LFOOTY, //386
    MNET_BODY_IN_LSHOULDERX_HIPX, //387
    MNET_BODY_IN_LSHOULDERY_HIPY, //388
    MNET_BODY_IN_LSHOULDERX_ENDSITE_EYE_RX, //389
    MNET_BODY_IN_LSHOULDERY_ENDSITE_EYE_RY, //390
    MNET_BODY_IN_LSHOULDERX_ENDSITE_EYE_LX, //391
    MNET_BODY_IN_LSHOULDERY_ENDSITE_EYE_LY, //392
    MNET_BODY_IN_LSHOULDERX_NECKX, //393
    MNET_BODY_IN_LSHOULDERY_NECKY, //394
    MNET_BODY_IN_LSHOULDERX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //395
    MNET_BODY_IN_LSHOULDERY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //396
    MNET_BODY_IN_LSHOULDERX_RSHOULDERX, //397
    MNET_BODY_IN_LSHOULDERY_RSHOULDERY, //398
    MNET_BODY_IN_LSHOULDERX_RELBOWX, //399
    MNET_BODY_IN_LSHOULDERY_RELBOWY, //400
    MNET_BODY_IN_LSHOULDERX_RHANDX, //401
    MNET_BODY_IN_LSHOULDERY_RHANDY, //402
    MNET_BODY_IN_LSHOULDERX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //403
    MNET_BODY_IN_LSHOULDERY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //404
    MNET_BODY_IN_LSHOULDERX_LSHOULDERX, //405
    MNET_BODY_IN_LSHOULDERY_LSHOULDERY, //406
    MNET_BODY_IN_LSHOULDERX_LELBOWX, //407
    MNET_BODY_IN_LSHOULDERY_LELBOWY, //408
    MNET_BODY_IN_LSHOULDERX_LHANDX, //409
    MNET_BODY_IN_LSHOULDERY_LHANDY, //410
    MNET_BODY_IN_LSHOULDERX_RHIPX, //411
    MNET_BODY_IN_LSHOULDERY_RHIPY, //412
    MNET_BODY_IN_LSHOULDERX_RKNEEX, //413
    MNET_BODY_IN_LSHOULDERY_RKNEEY, //414
    MNET_BODY_IN_LSHOULDERX_RFOOTX, //415
    MNET_BODY_IN_LSHOULDERY_RFOOTY, //416
    MNET_BODY_IN_LSHOULDERX_LHIPX, //417
    MNET_BODY_IN_LSHOULDERY_LHIPY, //418
    MNET_BODY_IN_LSHOULDERX_LKNEEX, //419
    MNET_BODY_IN_LSHOULDERY_LKNEEY, //420
    MNET_BODY_IN_LSHOULDERX_LFOOTX, //421
    MNET_BODY_IN_LSHOULDERY_LFOOTY, //422
    MNET_BODY_IN_LELBOWX_HIPX, //423
    MNET_BODY_IN_LELBOWY_HIPY, //424
    MNET_BODY_IN_LELBOWX_ENDSITE_EYE_RX, //425
    MNET_BODY_IN_LELBOWY_ENDSITE_EYE_RY, //426
    MNET_BODY_IN_LELBOWX_ENDSITE_EYE_LX, //427
    MNET_BODY_IN_LELBOWY_ENDSITE_EYE_LY, //428
    MNET_BODY_IN_LELBOWX_NECKX, //429
    MNET_BODY_IN_LELBOWY_NECKY, //430
    MNET_BODY_IN_LELBOWX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //431
    MNET_BODY_IN_LELBOWY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //432
    MNET_BODY_IN_LELBOWX_RSHOULDERX, //433
    MNET_BODY_IN_LELBOWY_RSHOULDERY, //434
    MNET_BODY_IN_LELBOWX_RELBOWX, //435
    MNET_BODY_IN_LELBOWY_RELBOWY, //436
    MNET_BODY_IN_LELBOWX_RHANDX, //437
    MNET_BODY_IN_LELBOWY_RHANDY, //438
    MNET_BODY_IN_LELBOWX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //439
    MNET_BODY_IN_LELBOWY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //440
    MNET_BODY_IN_LELBOWX_LSHOULDERX, //441
    MNET_BODY_IN_LELBOWY_LSHOULDERY, //442
    MNET_BODY_IN_LELBOWX_LELBOWX, //443
    MNET_BODY_IN_LELBOWY_LELBOWY, //444
    MNET_BODY_IN_LELBOWX_LHANDX, //445
    MNET_BODY_IN_LELBOWY_LHANDY, //446
    MNET_BODY_IN_LELBOWX_RHIPX, //447
    MNET_BODY_IN_LELBOWY_RHIPY, //448
    MNET_BODY_IN_LELBOWX_RKNEEX, //449
    MNET_BODY_IN_LELBOWY_RKNEEY, //450
    MNET_BODY_IN_LELBOWX_RFOOTX, //451
    MNET_BODY_IN_LELBOWY_RFOOTY, //452
    MNET_BODY_IN_LELBOWX_LHIPX, //453
    MNET_BODY_IN_LELBOWY_LHIPY, //454
    MNET_BODY_IN_LELBOWX_LKNEEX, //455
    MNET_BODY_IN_LELBOWY_LKNEEY, //456
    MNET_BODY_IN_LELBOWX_LFOOTX, //457
    MNET_BODY_IN_LELBOWY_LFOOTY, //458
    MNET_BODY_IN_LHANDX_HIPX, //459
    MNET_BODY_IN_LHANDY_HIPY, //460
    MNET_BODY_IN_LHANDX_ENDSITE_EYE_RX, //461
    MNET_BODY_IN_LHANDY_ENDSITE_EYE_RY, //462
    MNET_BODY_IN_LHANDX_ENDSITE_EYE_LX, //463
    MNET_BODY_IN_LHANDY_ENDSITE_EYE_LY, //464
    MNET_BODY_IN_LHANDX_NECKX, //465
    MNET_BODY_IN_LHANDY_NECKY, //466
    MNET_BODY_IN_LHANDX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //467
    MNET_BODY_IN_LHANDY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //468
    MNET_BODY_IN_LHANDX_RSHOULDERX, //469
    MNET_BODY_IN_LHANDY_RSHOULDERY, //470
    MNET_BODY_IN_LHANDX_RELBOWX, //471
    MNET_BODY_IN_LHANDY_RELBOWY, //472
    MNET_BODY_IN_LHANDX_RHANDX, //473
    MNET_BODY_IN_LHANDY_RHANDY, //474
    MNET_BODY_IN_LHANDX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //475
    MNET_BODY_IN_LHANDY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //476
    MNET_BODY_IN_LHANDX_LSHOULDERX, //477
    MNET_BODY_IN_LHANDY_LSHOULDERY, //478
    MNET_BODY_IN_LHANDX_LELBOWX, //479
    MNET_BODY_IN_LHANDY_LELBOWY, //480
    MNET_BODY_IN_LHANDX_LHANDX, //481
    MNET_BODY_IN_LHANDY_LHANDY, //482
    MNET_BODY_IN_LHANDX_RHIPX, //483
    MNET_BODY_IN_LHANDY_RHIPY, //484
    MNET_BODY_IN_LHANDX_RKNEEX, //485
    MNET_BODY_IN_LHANDY_RKNEEY, //486
    MNET_BODY_IN_LHANDX_RFOOTX, //487
    MNET_BODY_IN_LHANDY_RFOOTY, //488
    MNET_BODY_IN_LHANDX_LHIPX, //489
    MNET_BODY_IN_LHANDY_LHIPY, //490
    MNET_BODY_IN_LHANDX_LKNEEX, //491
    MNET_BODY_IN_LHANDY_LKNEEY, //492
    MNET_BODY_IN_LHANDX_LFOOTX, //493
    MNET_BODY_IN_LHANDY_LFOOTY, //494
    MNET_BODY_IN_RHIPX_HIPX, //495
    MNET_BODY_IN_RHIPY_HIPY, //496
    MNET_BODY_IN_RHIPX_ENDSITE_EYE_RX, //497
    MNET_BODY_IN_RHIPY_ENDSITE_EYE_RY, //498
    MNET_BODY_IN_RHIPX_ENDSITE_EYE_LX, //499
    MNET_BODY_IN_RHIPY_ENDSITE_EYE_LY, //500
    MNET_BODY_IN_RHIPX_NECKX, //501
    MNET_BODY_IN_RHIPY_NECKY, //502
    MNET_BODY_IN_RHIPX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //503
    MNET_BODY_IN_RHIPY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //504
    MNET_BODY_IN_RHIPX_RSHOULDERX, //505
    MNET_BODY_IN_RHIPY_RSHOULDERY, //506
    MNET_BODY_IN_RHIPX_RELBOWX, //507
    MNET_BODY_IN_RHIPY_RELBOWY, //508
    MNET_BODY_IN_RHIPX_RHANDX, //509
    MNET_BODY_IN_RHIPY_RHANDY, //510
    MNET_BODY_IN_RHIPX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //511
    MNET_BODY_IN_RHIPY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //512
    MNET_BODY_IN_RHIPX_LSHOULDERX, //513
    MNET_BODY_IN_RHIPY_LSHOULDERY, //514
    MNET_BODY_IN_RHIPX_LELBOWX, //515
    MNET_BODY_IN_RHIPY_LELBOWY, //516
    MNET_BODY_IN_RHIPX_LHANDX, //517
    MNET_BODY_IN_RHIPY_LHANDY, //518
    MNET_BODY_IN_RHIPX_RHIPX, //519
    MNET_BODY_IN_RHIPY_RHIPY, //520
    MNET_BODY_IN_RHIPX_RKNEEX, //521
    MNET_BODY_IN_RHIPY_RKNEEY, //522
    MNET_BODY_IN_RHIPX_RFOOTX, //523
    MNET_BODY_IN_RHIPY_RFOOTY, //524
    MNET_BODY_IN_RHIPX_LHIPX, //525
    MNET_BODY_IN_RHIPY_LHIPY, //526
    MNET_BODY_IN_RHIPX_LKNEEX, //527
    MNET_BODY_IN_RHIPY_LKNEEY, //528
    MNET_BODY_IN_RHIPX_LFOOTX, //529
    MNET_BODY_IN_RHIPY_LFOOTY, //530
    MNET_BODY_IN_RKNEEX_HIPX, //531
    MNET_BODY_IN_RKNEEY_HIPY, //532
    MNET_BODY_IN_RKNEEX_ENDSITE_EYE_RX, //533
    MNET_BODY_IN_RKNEEY_ENDSITE_EYE_RY, //534
    MNET_BODY_IN_RKNEEX_ENDSITE_EYE_LX, //535
    MNET_BODY_IN_RKNEEY_ENDSITE_EYE_LY, //536
    MNET_BODY_IN_RKNEEX_NECKX, //537
    MNET_BODY_IN_RKNEEY_NECKY, //538
    MNET_BODY_IN_RKNEEX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //539
    MNET_BODY_IN_RKNEEY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //540
    MNET_BODY_IN_RKNEEX_RSHOULDERX, //541
    MNET_BODY_IN_RKNEEY_RSHOULDERY, //542
    MNET_BODY_IN_RKNEEX_RELBOWX, //543
    MNET_BODY_IN_RKNEEY_RELBOWY, //544
    MNET_BODY_IN_RKNEEX_RHANDX, //545
    MNET_BODY_IN_RKNEEY_RHANDY, //546
    MNET_BODY_IN_RKNEEX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //547
    MNET_BODY_IN_RKNEEY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //548
    MNET_BODY_IN_RKNEEX_LSHOULDERX, //549
    MNET_BODY_IN_RKNEEY_LSHOULDERY, //550
    MNET_BODY_IN_RKNEEX_LELBOWX, //551
    MNET_BODY_IN_RKNEEY_LELBOWY, //552
    MNET_BODY_IN_RKNEEX_LHANDX, //553
    MNET_BODY_IN_RKNEEY_LHANDY, //554
    MNET_BODY_IN_RKNEEX_RHIPX, //555
    MNET_BODY_IN_RKNEEY_RHIPY, //556
    MNET_BODY_IN_RKNEEX_RKNEEX, //557
    MNET_BODY_IN_RKNEEY_RKNEEY, //558
    MNET_BODY_IN_RKNEEX_RFOOTX, //559
    MNET_BODY_IN_RKNEEY_RFOOTY, //560
    MNET_BODY_IN_RKNEEX_LHIPX, //561
    MNET_BODY_IN_RKNEEY_LHIPY, //562
    MNET_BODY_IN_RKNEEX_LKNEEX, //563
    MNET_BODY_IN_RKNEEY_LKNEEY, //564
    MNET_BODY_IN_RKNEEX_LFOOTX, //565
    MNET_BODY_IN_RKNEEY_LFOOTY, //566
    MNET_BODY_IN_RFOOTX_HIPX, //567
    MNET_BODY_IN_RFOOTY_HIPY, //568
    MNET_BODY_IN_RFOOTX_ENDSITE_EYE_RX, //569
    MNET_BODY_IN_RFOOTY_ENDSITE_EYE_RY, //570
    MNET_BODY_IN_RFOOTX_ENDSITE_EYE_LX, //571
    MNET_BODY_IN_RFOOTY_ENDSITE_EYE_LY, //572
    MNET_BODY_IN_RFOOTX_NECKX, //573
    MNET_BODY_IN_RFOOTY_NECKY, //574
    MNET_BODY_IN_RFOOTX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //575
    MNET_BODY_IN_RFOOTY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //576
    MNET_BODY_IN_RFOOTX_RSHOULDERX, //577
    MNET_BODY_IN_RFOOTY_RSHOULDERY, //578
    MNET_BODY_IN_RFOOTX_RELBOWX, //579
    MNET_BODY_IN_RFOOTY_RELBOWY, //580
    MNET_BODY_IN_RFOOTX_RHANDX, //581
    MNET_BODY_IN_RFOOTY_RHANDY, //582
    MNET_BODY_IN_RFOOTX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //583
    MNET_BODY_IN_RFOOTY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //584
    MNET_BODY_IN_RFOOTX_LSHOULDERX, //585
    MNET_BODY_IN_RFOOTY_LSHOULDERY, //586
    MNET_BODY_IN_RFOOTX_LELBOWX, //587
    MNET_BODY_IN_RFOOTY_LELBOWY, //588
    MNET_BODY_IN_RFOOTX_LHANDX, //589
    MNET_BODY_IN_RFOOTY_LHANDY, //590
    MNET_BODY_IN_RFOOTX_RHIPX, //591
    MNET_BODY_IN_RFOOTY_RHIPY, //592
    MNET_BODY_IN_RFOOTX_RKNEEX, //593
    MNET_BODY_IN_RFOOTY_RKNEEY, //594
    MNET_BODY_IN_RFOOTX_RFOOTX, //595
    MNET_BODY_IN_RFOOTY_RFOOTY, //596
    MNET_BODY_IN_RFOOTX_LHIPX, //597
    MNET_BODY_IN_RFOOTY_LHIPY, //598
    MNET_BODY_IN_RFOOTX_LKNEEX, //599
    MNET_BODY_IN_RFOOTY_LKNEEY, //600
    MNET_BODY_IN_RFOOTX_LFOOTX, //601
    MNET_BODY_IN_RFOOTY_LFOOTY, //602
    MNET_BODY_IN_LHIPX_HIPX, //603
    MNET_BODY_IN_LHIPY_HIPY, //604
    MNET_BODY_IN_LHIPX_ENDSITE_EYE_RX, //605
    MNET_BODY_IN_LHIPY_ENDSITE_EYE_RY, //606
    MNET_BODY_IN_LHIPX_ENDSITE_EYE_LX, //607
    MNET_BODY_IN_LHIPY_ENDSITE_EYE_LY, //608
    MNET_BODY_IN_LHIPX_NECKX, //609
    MNET_BODY_IN_LHIPY_NECKY, //610
    MNET_BODY_IN_LHIPX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //611
    MNET_BODY_IN_LHIPY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //612
    MNET_BODY_IN_LHIPX_RSHOULDERX, //613
    MNET_BODY_IN_LHIPY_RSHOULDERY, //614
    MNET_BODY_IN_LHIPX_RELBOWX, //615
    MNET_BODY_IN_LHIPY_RELBOWY, //616
    MNET_BODY_IN_LHIPX_RHANDX, //617
    MNET_BODY_IN_LHIPY_RHANDY, //618
    MNET_BODY_IN_LHIPX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //619
    MNET_BODY_IN_LHIPY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //620
    MNET_BODY_IN_LHIPX_LSHOULDERX, //621
    MNET_BODY_IN_LHIPY_LSHOULDERY, //622
    MNET_BODY_IN_LHIPX_LELBOWX, //623
    MNET_BODY_IN_LHIPY_LELBOWY, //624
    MNET_BODY_IN_LHIPX_LHANDX, //625
    MNET_BODY_IN_LHIPY_LHANDY, //626
    MNET_BODY_IN_LHIPX_RHIPX, //627
    MNET_BODY_IN_LHIPY_RHIPY, //628
    MNET_BODY_IN_LHIPX_RKNEEX, //629
    MNET_BODY_IN_LHIPY_RKNEEY, //630
    MNET_BODY_IN_LHIPX_RFOOTX, //631
    MNET_BODY_IN_LHIPY_RFOOTY, //632
    MNET_BODY_IN_LHIPX_LHIPX, //633
    MNET_BODY_IN_LHIPY_LHIPY, //634
    MNET_BODY_IN_LHIPX_LKNEEX, //635
    MNET_BODY_IN_LHIPY_LKNEEY, //636
    MNET_BODY_IN_LHIPX_LFOOTX, //637
    MNET_BODY_IN_LHIPY_LFOOTY, //638
    MNET_BODY_IN_LKNEEX_HIPX, //639
    MNET_BODY_IN_LKNEEY_HIPY, //640
    MNET_BODY_IN_LKNEEX_ENDSITE_EYE_RX, //641
    MNET_BODY_IN_LKNEEY_ENDSITE_EYE_RY, //642
    MNET_BODY_IN_LKNEEX_ENDSITE_EYE_LX, //643
    MNET_BODY_IN_LKNEEY_ENDSITE_EYE_LY, //644
    MNET_BODY_IN_LKNEEX_NECKX, //645
    MNET_BODY_IN_LKNEEY_NECKY, //646
    MNET_BODY_IN_LKNEEX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //647
    MNET_BODY_IN_LKNEEY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //648
    MNET_BODY_IN_LKNEEX_RSHOULDERX, //649
    MNET_BODY_IN_LKNEEY_RSHOULDERY, //650
    MNET_BODY_IN_LKNEEX_RELBOWX, //651
    MNET_BODY_IN_LKNEEY_RELBOWY, //652
    MNET_BODY_IN_LKNEEX_RHANDX, //653
    MNET_BODY_IN_LKNEEY_RHANDY, //654
    MNET_BODY_IN_LKNEEX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //655
    MNET_BODY_IN_LKNEEY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //656
    MNET_BODY_IN_LKNEEX_LSHOULDERX, //657
    MNET_BODY_IN_LKNEEY_LSHOULDERY, //658
    MNET_BODY_IN_LKNEEX_LELBOWX, //659
    MNET_BODY_IN_LKNEEY_LELBOWY, //660
    MNET_BODY_IN_LKNEEX_LHANDX, //661
    MNET_BODY_IN_LKNEEY_LHANDY, //662
    MNET_BODY_IN_LKNEEX_RHIPX, //663
    MNET_BODY_IN_LKNEEY_RHIPY, //664
    MNET_BODY_IN_LKNEEX_RKNEEX, //665
    MNET_BODY_IN_LKNEEY_RKNEEY, //666
    MNET_BODY_IN_LKNEEX_RFOOTX, //667
    MNET_BODY_IN_LKNEEY_RFOOTY, //668
    MNET_BODY_IN_LKNEEX_LHIPX, //669
    MNET_BODY_IN_LKNEEY_LHIPY, //670
    MNET_BODY_IN_LKNEEX_LKNEEX, //671
    MNET_BODY_IN_LKNEEY_LKNEEY, //672
    MNET_BODY_IN_LKNEEX_LFOOTX, //673
    MNET_BODY_IN_LKNEEY_LFOOTY, //674
    MNET_BODY_IN_LFOOTX_HIPX, //675
    MNET_BODY_IN_LFOOTY_HIPY, //676
    MNET_BODY_IN_LFOOTX_ENDSITE_EYE_RX, //677
    MNET_BODY_IN_LFOOTY_ENDSITE_EYE_RY, //678
    MNET_BODY_IN_LFOOTX_ENDSITE_EYE_LX, //679
    MNET_BODY_IN_LFOOTY_ENDSITE_EYE_LY, //680
    MNET_BODY_IN_LFOOTX_NECKX, //681
    MNET_BODY_IN_LFOOTY_NECKY, //682
    MNET_BODY_IN_LFOOTX_VIRTUAL_HIP_X_PLUS0_3_Y_0X, //683
    MNET_BODY_IN_LFOOTY_VIRTUAL_HIP_X_PLUS0_3_Y_0Y, //684
    MNET_BODY_IN_LFOOTX_RSHOULDERX, //685
    MNET_BODY_IN_LFOOTY_RSHOULDERY, //686
    MNET_BODY_IN_LFOOTX_RELBOWX, //687
    MNET_BODY_IN_LFOOTY_RELBOWY, //688
    MNET_BODY_IN_LFOOTX_RHANDX, //689
    MNET_BODY_IN_LFOOTY_RHANDY, //690
    MNET_BODY_IN_LFOOTX_VIRTUAL_HIP_X_MINUS_0_3_Y_0X, //691
    MNET_BODY_IN_LFOOTY_VIRTUAL_HIP_X_MINUS_0_3_Y_0Y, //692
    MNET_BODY_IN_LFOOTX_LSHOULDERX, //693
    MNET_BODY_IN_LFOOTY_LSHOULDERY, //694
    MNET_BODY_IN_LFOOTX_LELBOWX, //695
    MNET_BODY_IN_LFOOTY_LELBOWY, //696
    MNET_BODY_IN_LFOOTX_LHANDX, //697
    MNET_BODY_IN_LFOOTY_LHANDY, //698
    MNET_BODY_IN_LFOOTX_RHIPX, //699
    MNET_BODY_IN_LFOOTY_RHIPY, //700
    MNET_BODY_IN_LFOOTX_RKNEEX, //701
    MNET_BODY_IN_LFOOTY_RKNEEY, //702
    MNET_BODY_IN_LFOOTX_RFOOTX, //703
    MNET_BODY_IN_LFOOTY_RFOOTY, //704
    MNET_BODY_IN_LFOOTX_LHIPX, //705
    MNET_BODY_IN_LFOOTY_LHIPY, //706
    MNET_BODY_IN_LFOOTX_LKNEEX, //707
    MNET_BODY_IN_LFOOTY_LKNEEY, //708
    MNET_BODY_IN_LFOOTX_LFOOTX, //709
    MNET_BODY_IN_LFOOTY_LFOOTY, //710
    MNET_BODY_IN_NUMBER
};

/** @brief Programmer friendly enumerator of expected outputs*/
enum mocapNET_Output_body_enum
{
    MOCAPNET_BODY_OUTPUT_HIP_XPOSITION = 0, //0
    MOCAPNET_BODY_OUTPUT_HIP_YPOSITION, //1
    MOCAPNET_BODY_OUTPUT_HIP_ZPOSITION, //2
    MOCAPNET_BODY_OUTPUT_HIP_ZROTATION, //3
    MOCAPNET_BODY_OUTPUT_HIP_YROTATION, //4
    MOCAPNET_BODY_OUTPUT_HIP_XROTATION, //5
    MOCAPNET_BODY_OUTPUT_ABDOMEN_ZROTATION, //6
    MOCAPNET_BODY_OUTPUT_ABDOMEN_XROTATION, //7
    MOCAPNET_BODY_OUTPUT_ABDOMEN_YROTATION, //8
    MOCAPNET_BODY_OUTPUT_CHEST_ZROTATION, //9
    MOCAPNET_BODY_OUTPUT_CHEST_XROTATION, //10
    MOCAPNET_BODY_OUTPUT_CHEST_YROTATION, //11
    MOCAPNET_BODY_OUTPUT_NECK_ZROTATION, //12
    MOCAPNET_BODY_OUTPUT_NECK_XROTATION, //13
    MOCAPNET_BODY_OUTPUT_NECK_YROTATION, //14
    MOCAPNET_BODY_OUTPUT_HEAD_ZROTATION, //15
    MOCAPNET_BODY_OUTPUT_HEAD_XROTATION, //16
    MOCAPNET_BODY_OUTPUT_HEAD_YROTATION, //17
    MOCAPNET_BODY_OUTPUT_EYE_L_ZROTATION, //18
    MOCAPNET_BODY_OUTPUT_EYE_L_XROTATION, //19
    MOCAPNET_BODY_OUTPUT_EYE_L_YROTATION, //20
    MOCAPNET_BODY_OUTPUT_EYE_R_ZROTATION, //21
    MOCAPNET_BODY_OUTPUT_EYE_R_XROTATION, //22
    MOCAPNET_BODY_OUTPUT_EYE_R_YROTATION, //23
    MOCAPNET_BODY_OUTPUT_RSHOULDER_ZROTATION, //24
    MOCAPNET_BODY_OUTPUT_RSHOULDER_XROTATION, //25
    MOCAPNET_BODY_OUTPUT_RSHOULDER_YROTATION, //26
    MOCAPNET_BODY_OUTPUT_RELBOW_ZROTATION, //27
    MOCAPNET_BODY_OUTPUT_RELBOW_XROTATION, //28
    MOCAPNET_BODY_OUTPUT_RELBOW_YROTATION, //29
    MOCAPNET_BODY_OUTPUT_RHAND_ZROTATION, //30
    MOCAPNET_BODY_OUTPUT_RHAND_XROTATION, //31
    MOCAPNET_BODY_OUTPUT_RHAND_YROTATION, //32
    MOCAPNET_BODY_OUTPUT_LSHOULDER_ZROTATION, //33
    MOCAPNET_BODY_OUTPUT_LSHOULDER_XROTATION, //34
    MOCAPNET_BODY_OUTPUT_LSHOULDER_YROTATION, //35
    MOCAPNET_BODY_OUTPUT_LELBOW_ZROTATION, //36
    MOCAPNET_BODY_OUTPUT_LELBOW_XROTATION, //37
    MOCAPNET_BODY_OUTPUT_LELBOW_YROTATION, //38
    MOCAPNET_BODY_OUTPUT_LHAND_ZROTATION, //39
    MOCAPNET_BODY_OUTPUT_LHAND_XROTATION, //40
    MOCAPNET_BODY_OUTPUT_LHAND_YROTATION, //41
    MOCAPNET_BODY_OUTPUT_RHIP_ZROTATION, //42
    MOCAPNET_BODY_OUTPUT_RHIP_XROTATION, //43
    MOCAPNET_BODY_OUTPUT_RHIP_YROTATION, //44
    MOCAPNET_BODY_OUTPUT_RKNEE_ZROTATION, //45
    MOCAPNET_BODY_OUTPUT_RKNEE_XROTATION, //46
    MOCAPNET_BODY_OUTPUT_RKNEE_YROTATION, //47
    MOCAPNET_BODY_OUTPUT_RFOOT_ZROTATION, //48
    MOCAPNET_BODY_OUTPUT_RFOOT_XROTATION, //49
    MOCAPNET_BODY_OUTPUT_RFOOT_YROTATION, //50
    MOCAPNET_BODY_OUTPUT_TOE1_2_R_ZROTATION, //51
    MOCAPNET_BODY_OUTPUT_TOE1_2_R_XROTATION, //52
    MOCAPNET_BODY_OUTPUT_TOE1_2_R_YROTATION, //53
    MOCAPNET_BODY_OUTPUT_TOE5_3_R_ZROTATION, //54
    MOCAPNET_BODY_OUTPUT_TOE5_3_R_XROTATION, //55
    MOCAPNET_BODY_OUTPUT_TOE5_3_R_YROTATION, //56
    MOCAPNET_BODY_OUTPUT_LHIP_ZROTATION, //57
    MOCAPNET_BODY_OUTPUT_LHIP_XROTATION, //58
    MOCAPNET_BODY_OUTPUT_LHIP_YROTATION, //59
    MOCAPNET_BODY_OUTPUT_LKNEE_ZROTATION, //60
    MOCAPNET_BODY_OUTPUT_LKNEE_XROTATION, //61
    MOCAPNET_BODY_OUTPUT_LKNEE_YROTATION, //62
    MOCAPNET_BODY_OUTPUT_LFOOT_ZROTATION, //63
    MOCAPNET_BODY_OUTPUT_LFOOT_XROTATION, //64
    MOCAPNET_BODY_OUTPUT_LFOOT_YROTATION, //65
    MOCAPNET_BODY_OUTPUT_TOE1_2_L_ZROTATION, //66
    MOCAPNET_BODY_OUTPUT_TOE1_2_L_XROTATION, //67
    MOCAPNET_BODY_OUTPUT_TOE1_2_L_YROTATION, //68
    MOCAPNET_BODY_OUTPUT_TOE5_3_L_ZROTATION, //69
    MOCAPNET_BODY_OUTPUT_TOE5_3_L_XROTATION, //70
    MOCAPNET_BODY_OUTPUT_TOE5_3_L_YROTATION, //71
    MOCAPNET_BODY_OUTPUT_NUMBER
};

/** @brief Programmer friendly enumerator of NSDM elments*/
enum mocapNET_NSDM_body_enum
{
    MNET_NSDM_BODY_HIP = 0, //0
    MNET_NSDM_BODY_ENDSITE_EYE_R, //1
    MNET_NSDM_BODY_ENDSITE_EYE_L, //2
    MNET_NSDM_BODY_NECK, //3
    MNET_NSDM_BODY_VIRTUAL_HIP_X_PLUS0_3_Y_0, //4
    MNET_NSDM_BODY_RSHOULDER, //5
    MNET_NSDM_BODY_RELBOW, //6
    MNET_NSDM_BODY_RHAND, //7
    MNET_NSDM_BODY_VIRTUAL_HIP_X_MINUS_0_3_Y_0, //8
    MNET_NSDM_BODY_LSHOULDER, //9
    MNET_NSDM_BODY_LELBOW, //10
    MNET_NSDM_BODY_LHAND, //11
    MNET_NSDM_BODY_RHIP, //12
    MNET_NSDM_BODY_RKNEE, //13
    MNET_NSDM_BODY_RFOOT, //14
    MNET_NSDM_BODY_LHIP, //15
    MNET_NSDM_BODY_LKNEE, //16
    MNET_NSDM_BODY_LFOOT, //17
    MNET_NSDM_BODY_NUMBER
};

/** @brief This is a lookup table to immediately resolve referred Joints*/
static const int mocapNET_ResolveJoint_body[] =
{
    0, //0
    4, //1
    3, //2
    1, //3
    0, //4
    5, //5
    6, //6
    7, //7
    0, //8
    8, //9
    9, //10
    10, //11
    11, //12
    12, //13
    13, //14
    16, //15
    17, //16
    18, //17
    0//end of array
};

/** @brief This is a lookup table to immediately resolve referred Joints of second targets*/
static const int mocapNET_ResolveSecondTargetJoint_body[] =
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
    0, //16
    0, //17
    0//end of array
};

/** @brief This is the configuration of NSDM elements :
 *  A value of 0 is a normal 2D point
 *  A value of 1 is a 2D point plus some offset
 *  A value of 2 is a virtual point between two 2D points */
static const int mocapNET_ArtificialJoint_body[] =
{
    0, //0
    0, //1
    0, //2
    0, //3
    1, //4
    0, //5
    0, //6
    0, //7
    1, //8
    0, //9
    0, //10
    0, //11
    0, //12
    0, //13
    0, //14
    0, //15
    0, //16
    0, //17
    0//end of array
};

/** @brief These are X offsets for artificial joints of type 1 ( see mocapNET_ArtificialJoint_body )*/
static const float mocapNET_ArtificialJointXOffset_body[] =
{
    0, //0
    0, //1
    0, //2
    0, //3
    0.3, //4
    0, //5
    0, //6
    0, //7
    -0.3, //8
    0, //9
    0, //10
    0, //11
    0, //12
    0, //13
    0, //14
    0, //15
    0, //16
    0, //17
    0//end of array
};

/** @brief These are Y offsets for artificial joints of type 1 ( see mocapNET_ArtificialJoint_body )*/
static const float mocapNET_ArtificialJointYOffset_body[] =
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
    0, //16
    0, //17
    0//end of array
};

/** @brief These are 2D Joints that are used as starting points for scaling vectors*/
static const int mocapNET_ScalingStart_body[] =
{
    0, //0
    0, //1
    0//end of array
};

/** @brief These are 2D Joints that are used as ending points for scaling vectors*/
static const int mocapNET_ScalingEnd_body[] =
{
    5, //0
    8, //1
    0//end of array
};

/** @brief This function can be used to debug NSDM input and find in a user friendly what is missing..!*/
static int bodyCountMissingNSDMElements(std::vector<float> mocapNETInput,int verbose)
{
    unsigned int numberOfZeros=0;
    for (int i=0; i<mocapNETInput.size(); i++)
    {
        if (mocapNETInput[i]==0)
        {
            if (verbose) {
                fprintf(stderr,"body NSDM %s missing\n",mocapNET_body[i]);
            }
            ++numberOfZeros;
        }
    }
    return numberOfZeros;
}

/** @brief Another debug function to make sure the joint order is correctly resolved..!*/
static void bodyDebugNSDMJointOrder(char ** labels)
{
    for (int i=0; i<MNET_NSDM_BODY_NUMBER; i++)
    {
        fprintf(stderr,"NSDM body %s => skeletonSerialized %s\n ",mocapNET_body[i],labels[i]);
    }
}

/** @brief This function returns the euclidean distance between two input 2D joints and zero if either of them is invalid*/
static float getJoint2DDistance_BODY(std::vector<float> in,int jointA,int jointB)
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

/** @brief This function returns a vector of NSDM values ready for use with the MocapNET body network */
static std::vector<float> bodyCreateNDSM(std::vector<float> in,int havePositionalElements,int haveAngularElements,int doScaleCompensation)
{
    std::vector<float> result;
    for (int i=0; i<MNET_NSDM_BODY_NUMBER; i++)
    {
        //------------------------------------
        unsigned int iJointID=mocapNET_ResolveJoint_body[i];
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
            for (int j=0; j<MNET_NSDM_BODY_NUMBER; j++)
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
            switch (mocapNET_ArtificialJoint_body[i])
            {
            case 1 :
                iX+=mocapNET_ArtificialJointXOffset_body[i];
                iY+=mocapNET_ArtificialJointYOffset_body[i];
                break;

            default :
                //Normal NSDM element ( not virtual )
                break;
            };
            for (int j=0; j<MNET_NSDM_BODY_NUMBER; j++)
            {
                jJointID=mocapNET_ResolveJoint_body[j];
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
                    switch (mocapNET_ArtificialJoint_body[j])
                    {
                    case 1 :
                        jX+=mocapNET_ArtificialJointXOffset_body[j];
                        jY+=mocapNET_ArtificialJointYOffset_body[j];
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


    if (doScaleCompensation)
    {
        //Normalization-------------------------------------------------------------------------------
        unsigned int numberOfNSDMScalingRules=2;
        if (numberOfNSDMScalingRules>0)
        {
            unsigned int numberOfDistanceSamples=0;
            float sumOfDistanceSamples=0.0;
            for ( int i=0; i<numberOfNSDMScalingRules; i++)
            {
                unsigned int iJointID=mocapNET_ScalingStart_body[i];
                unsigned int jJointID=mocapNET_ScalingEnd_body[i];
                float distance = getJoint2DDistance_BODY(in,iJointID,jJointID);
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
    } //If scale compensation is enabled..
//-------------------------------------------------------------------------------------------------
    for (int i=0; i<result.size(); i++)
    {
        if (result[i]!=0.0)
        {
            result[i]=0.5+result[i];
        }
    }
//-------------------------------------------------------------------------------------------------
    return result;
}

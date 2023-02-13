#pragma once
/** @file bvh.hpp
 *  @brief This is an interface to the BVH code. The BVH code ( https://github.com/AmmarkoV/RGBDAcquisition/tree/master/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader )
 *  might not be available. If this is the case then CMake will not declare the USE_BVH compilation flag and the whole code will not be included.
 *  @author Ammar Qammaz (AmmarkoV)
 */

#include <iostream>
#include <vector>



/**
 * @brief This is a BVH header that can be easily injected in a file ( using just an fprintf call )
 */
//To do this get replace \n with \\n"\n" in headerWithHead.bvh
static const char * bvhHeader=
"HIERARCHY\n"
"ROOT hip\n"
"{\n"
"  OFFSET 0 0 0\n"
"  CHANNELS 6 Xposition Yposition Zposition Zrotation Yrotation Xrotation\n"
"  JOINT abdomen\n"
"  {\n"
"    OFFSET 0 20.6881 -0.73152\n"
"    CHANNELS 3 Zrotation Xrotation Yrotation\n"
"    JOINT chest\n"
"    {\n"
"      OFFSET 0 11.7043 -0.48768\n"
"      CHANNELS 3 Zrotation Xrotation Yrotation\n"
"      JOINT neck\n"
"      {\n"
"          OFFSET 0 22.1894 -2.19456\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"          JOINT neck1\n"
"          {\n"
"            OFFSET 0.000000 5.364170 1.574630\n"
"            CHANNELS 3 Zrotation Xrotation Yrotation \n"
"            JOINT head\n"
"            {\n"
"              OFFSET 0.000000 5.364141 1.574630\n"
"              CHANNELS 3 Zrotation Xrotation Yrotation \n"
"              JOINT __jaw\n"
"              {\n"
"                OFFSET 0.000000 13.604700 -0.502080\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                JOINT jaw\n"
"                {\n"
"                  OFFSET 0.000000 -13.499860 2.500710\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT special04\n"
"                  {\n"
"                    OFFSET -0.000000 -6.835370 4.375500\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT oris02\n"
"                    {\n"
"                      OFFSET 0.000000 1.711150 2.820850\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT oris01\n"
"                      {\n"
"                        OFFSET -0.000000 0.972390 0.845650\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        End Site\n"
"                        {\n"
"                          OFFSET 0.000000 1.162291 0.607091\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                    JOINT oris06.l\n"
"                    {\n"
"                      OFFSET 0.000000 1.711150 2.820850\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT oris07.l\n"
"                      {\n"
"                        OFFSET 1.168850 0.445180 0.506110\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        End Site\n"
"                        {\n"
"                          OFFSET 0.450611 1.195178 0.204519\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                    JOINT oris06.r\n"
"                    {\n"
"                      OFFSET 0.000000 1.711150 2.820850\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT oris07.r\n"
"                      {\n"
"                        OFFSET -1.168850 0.445180 0.506110\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        End Site\n"
"                        {\n"
"                          OFFSET -0.450611 1.195173 0.204519\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                  JOINT tongue00\n"
"                  {\n"
"                    OFFSET -0.000000 -6.835370 4.375500\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT tongue01\n"
"                    {\n"
"                      OFFSET 0.000000 3.973650 -3.762340\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT tongue02\n"
"                      {\n"
"                        OFFSET 0.000000 0.429760 2.924710\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        JOINT tongue03\n"
"                        {\n"
"                          OFFSET 0.000000 0.018530 2.059010\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          JOINT __tongue04\n"
"                          {\n"
"                            OFFSET 0.000000 -0.440240 0.838860\n"
"                            CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                            JOINT tongue04\n"
"                            {\n"
"                              OFFSET 0.000000 0.000000 0.000000\n"
"                              CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                              End Site\n"
"                              {\n"
"                                OFFSET 0.000000 -0.440230 0.838860\n"
"                              }\n"
"                            }\n"
"                          }\n"
"                          JOINT tongue07.l\n"
"                          {\n"
"                            OFFSET 0.000000 -0.440240 0.838860\n"
"                            CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                            End Site\n"
"                            {\n"
"                              OFFSET 1.160923 -0.331531 0.018227\n"
"                            }\n"
"                          }\n"
"                          JOINT tongue07.r\n"
"                          {\n"
"                            OFFSET 0.000000 -0.440240 0.838860\n"
"                            CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                            End Site\n"
"                            {\n"
"                              OFFSET -1.160922 -0.331531 0.018227\n"
"                            }\n"
"                          }\n"
"                        }\n"
"                        JOINT tongue06.l\n"
"                        {\n"
"                          OFFSET 0.000000 0.018530 2.059010\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          End Site\n"
"                          {\n"
"                            OFFSET 1.644752 -0.526075 -0.203281\n"
"                          }\n"
"                        }\n"
"                        JOINT tongue06.r\n"
"                        {\n"
"                          OFFSET 0.000000 0.018530 2.059010\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          End Site\n"
"                          {\n"
"                            OFFSET -1.644752 -0.526075 -0.203282\n"
"                          }\n"
"                        }\n"
"                      }\n"
"                      JOINT tongue05.l\n"
"                      {\n"
"                        OFFSET 0.000000 0.429760 2.924710\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        End Site\n"
"                        {\n"
"                          OFFSET 1.971028 -0.388618 0.239206\n"
"                        }\n"
"                      }\n"
"                      JOINT tongue05.r\n"
"                      {\n"
"                        OFFSET 0.000000 0.429760 2.924710\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        End Site\n"
"                        {\n"
"                          OFFSET -1.971028 -0.388618 0.239205\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT __levator02.l\n"
"              {\n"
"                OFFSET 0.000000 13.604700 -0.502080\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                JOINT levator02.l\n"
"                {\n"
"                  OFFSET 0.313580 -11.321120 11.599360\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT levator03.l\n"
"                  {\n"
"                    OFFSET 1.681690 -1.563730 -1.357570\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT levator04.l\n"
"                    {\n"
"                      OFFSET 0.504730 -1.676760 -0.058160\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT levator05.l\n"
"                      {\n"
"                        OFFSET 0.145440 -1.643170 -0.225470\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        End Site\n"
"                        {\n"
"                          OFFSET -0.310116 -0.760198 -0.121474\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT __levator02.r\n"
"              {\n"
"                OFFSET 0.000000 13.604700 -0.502080\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                JOINT levator02.r\n"
"                {\n"
"                  OFFSET -0.313580 -11.321120 11.599360\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT levator03.r\n"
"                  {\n"
"                    OFFSET -1.681690 -1.563740 -1.357570\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT levator04.r\n"
"                    {\n"
"                      OFFSET -0.504730 -1.676750 -0.058160\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT levator05.r\n"
"                      {\n"
"                        OFFSET -0.145440 -1.643170 -0.225470\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        End Site\n"
"                        {\n"
"                          OFFSET 0.310116 -0.760193 -0.121474\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT __special01\n"
"              {\n"
"                OFFSET 0.000000 13.604700 -0.502080\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                JOINT special01\n"
"                {\n"
"                  OFFSET -0.000000 -14.026930 -5.716970\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT oris04.l\n"
"                  {\n"
"                    OFFSET -0.000000 -0.492640 17.312620\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT oris03.l\n"
"                    {\n"
"                      OFFSET 1.215520 -0.627430 -0.393050\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      End Site\n"
"                      {\n"
"                        OFFSET 0.288776 -0.560899 -0.149645\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                  JOINT oris04.r\n"
"                  {\n"
"                    OFFSET -0.000000 -0.492640 17.312620\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT oris03.r\n"
"                    {\n"
"                      OFFSET -1.215520 -0.627440 -0.393050\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      End Site\n"
"                      {\n"
"                        OFFSET -0.288776 -0.560894 -0.149645\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                  JOINT oris06\n"
"                  {\n"
"                    OFFSET -0.000000 -0.492640 17.312620\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT oris05\n"
"                    {\n"
"                      OFFSET -0.000000 -0.486000 0.000950\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      End Site\n"
"                      {\n"
"                        OFFSET 0.000000 -0.630493 0.197635\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT __special03\n"
"              {\n"
"                OFFSET 0.000000 13.604700 -0.502080\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                JOINT special03\n"
"                {\n"
"                  OFFSET 0.000000 -13.499860 2.500710\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT __levator06.l\n"
"                  {\n"
"                    OFFSET -0.000000 1.035800 10.090229\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT levator06.l\n"
"                    {\n"
"                      OFFSET 0.522240 -0.615720 0.045900\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      End Site\n"
"                      {\n"
"                        OFFSET 1.107505 -0.131898 -2.142227\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                  JOINT __levator06.r\n"
"                  {\n"
"                    OFFSET -0.000000 1.035800 10.090229\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT levator06.r\n"
"                    {\n"
"                      OFFSET -0.522240 -0.615730 0.045900\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      End Site\n"
"                      {\n"
"                        OFFSET -1.107506 -0.131893 -2.142227\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT special06.l\n"
"              {\n"
"                OFFSET 0.000000 13.604700 -0.502080\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                JOINT special05.l\n"
"                {\n"
"                  OFFSET 2.108890 0.153870 5.595070\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT eye.l\n"
"                  {\n"
"                    OFFSET 0.857170 -10.254801 2.414670\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    End Site\n"
"                    {\n"
"                      OFFSET 0.111609 0.006490 4.051424\n"
"                    }\n"
"                  }\n"
"                  JOINT orbicularis03.l\n"
"                  {\n"
"                    OFFSET 0.857170 -10.254801 2.414670\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    End Site\n"
"                    {\n"
"                      OFFSET 0.097789 1.146431 3.788029\n"
"                    }\n"
"                  }\n"
"                  JOINT orbicularis04.l\n"
"                  {\n"
"                    OFFSET 0.857170 -10.254801 2.414670\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    End Site\n"
"                    {\n"
"                      OFFSET 0.113609 -1.130505 3.863064\n"
"                    }\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT special06.r\n"
"              {\n"
"                OFFSET 0.000000 13.604700 -0.502080\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                JOINT special05.r\n"
"                {\n"
"                  OFFSET -2.108890 0.153870 5.595070\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT eye.r\n"
"                  {\n"
"                    OFFSET -0.857170 -10.254801 2.414670\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    End Site\n"
"                    {\n"
"                      OFFSET -0.111609 0.006490 4.051424\n"
"                    }\n"
"                  }\n"
"                  JOINT orbicularis03.r\n"
"                  {\n"
"                    OFFSET -0.857170 -10.254801 2.414670\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    End Site\n"
"                    {\n"
"                      OFFSET -0.097789 1.146431 3.788029\n"
"                    }\n"
"                  }\n"
"                  JOINT orbicularis04.r\n"
"                  {\n"
"                    OFFSET -0.857170 -10.254801 2.414670\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    End Site\n"
"                    {\n"
"                      OFFSET -0.113609 -1.130505 3.863064\n"
"                    }\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT __temporalis01.l\n"
"              {\n"
"                OFFSET 0.000000 13.604700 -0.502080\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                JOINT temporalis01.l\n"
"                {\n"
"                  OFFSET 6.332510 -9.444281 6.595120\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT oculi02.l\n"
"                  {\n"
"                    OFFSET -0.804920 0.053010 1.621140\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT oculi01.l\n"
"                    {\n"
"                      OFFSET -2.161570 1.690970 2.142660\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      End Site\n"
"                      {\n"
"                        OFFSET -1.740622 0.281157 0.647323\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT __temporalis01.r\n"
"              {\n"
"                OFFSET 0.000000 13.604700 -0.502080\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                JOINT temporalis01.r\n"
"                {\n"
"                  OFFSET -6.332510 -9.444281 6.595120\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT oculi02.r\n"
"                  {\n"
"                    OFFSET 0.804920 0.053010 1.621140\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT oculi01.r\n"
"                    {\n"
"                      OFFSET 2.161570 1.690970 2.142660\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      End Site\n"
"                      {\n"
"                        OFFSET 1.740622 0.281157 0.647323\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT __temporalis02.l\n"
"              {\n"
"                OFFSET 0.000000 13.604700 -0.502080\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                JOINT temporalis02.l\n"
"                {\n"
"                  OFFSET 6.377600 -11.680510 6.235180\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT risorius02.l\n"
"                  {\n"
"                    OFFSET -0.814250 0.451130 1.721730\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT risorius03.l\n"
"                    {\n"
"                      OFFSET -0.649710 -2.514660 0.612550\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      End Site\n"
"                      {\n"
"                        OFFSET 0.479556 -1.760402 -1.642659\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                }\n"
"              }\n"
"              JOINT __temporalis02.r\n"
"              {\n"
"                OFFSET 0.000000 13.604700 -0.502080\n"
"                CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                JOINT temporalis02.r\n"
"                {\n"
"                  OFFSET -6.377600 -11.680510 6.235180\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT risorius02.r\n"
"                  {\n"
"                    OFFSET 0.814250 0.451130 1.721730\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT risorius03.r\n"
"                    {\n"
"                      OFFSET 0.649710 -2.514660 0.612550\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      End Site\n"
"                      {\n"
"                        OFFSET -0.479556 -1.760402 -1.642659\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                }\n"
"              }\n"
"            }\n"
"          }\n"
"        }\n"
"      JOINT rCollar\n"
"      {\n"
"        OFFSET -2.68224 19.2634 -4.8768\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT rShldr\n"
"        {\n"
"          OFFSET -8.77824 -1.95073 1.46304\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          JOINT rForeArm\n"
"          {\n"
"            OFFSET -28.1742 -1.7115 0.48768\n"
"            CHANNELS 3 Zrotation Xrotation Yrotation\n"
"            JOINT rHand\n"
"            {\n"
"                  OFFSET -21.049400 0.002190 -0.634230\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT metacarpal1.r\n"
"                  {\n"
"                    OFFSET -2.815680 -0.279180 0.531660\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT finger2-1.r\n"
"                    {\n"
"                      OFFSET -6.292930 0.272380 2.520090\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT finger2-2.r\n"
"                      {\n"
"                        OFFSET -2.310530 -0.320530 -0.060510\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        JOINT finger2-3.r\n"
"                        {\n"
"                          OFFSET -2.051030 -0.295400 -0.164890\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          End Site\n"
"                          {\n"
"                            OFFSET -2.376838 -0.681367 -0.183877\n"
"                          }\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                  JOINT metacarpal2.r\n"
"                  {\n"
"                    OFFSET -2.815680 -0.279180 0.531660\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT finger3-1.r\n"
"                    {\n"
"                      OFFSET -6.313640 0.626130 0.318530\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT finger3-2.r\n"
"                      {\n"
"                        OFFSET -3.015730 -0.589480 -0.088540\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        JOINT finger3-3.r\n"
"                        {\n"
"                          OFFSET -2.482120 -0.426280 0.076670\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          End Site\n"
"                          {\n"
"                            OFFSET -2.344174 -0.731969 0.003260\n"
"                          }\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                  JOINT __metacarpal3.r\n"
"                  {\n"
"                    OFFSET -2.815680 -0.279180 0.531660\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT metacarpal3.r\n"
"                    {\n"
"                      OFFSET -0.606080 -0.162120 -1.874870\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT finger4-1.r\n"
"                      {\n"
"                        OFFSET -5.355730 0.702040 0.402510\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        JOINT finger4-2.r\n"
"                        {\n"
"                          OFFSET -2.643900 -0.485530 -0.117520\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          JOINT finger4-3.r\n"
"                          {\n"
"                            OFFSET -2.215850 -0.353160 0.066220\n"
"                            CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                            End Site\n"
"                            {\n"
"                              OFFSET -2.350273 -0.621223 -0.046375\n"
"                            }\n"
"                          }\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                  JOINT __metacarpal4.r\n"
"                  {\n"
"                    OFFSET -2.815680 -0.279180 0.531660\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT metacarpal4.r\n"
"                    {\n"
"                      OFFSET -0.606080 -0.162120 -1.874870\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT finger5-1.r\n"
"                      {\n"
"                        OFFSET -4.761700 0.175470 -1.109590\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        JOINT finger5-2.r\n"
"                        {\n"
"                          OFFSET -1.916360 -0.173360 -0.146170\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          JOINT finger5-3.r\n"
"                          {\n"
"                            OFFSET -1.411290 -0.108670 -0.020110\n"
"                            CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                            End Site\n"
"                            {\n"
"                              OFFSET -1.799226 -0.102363 -0.078601\n"
"                            }\n"
"                          }\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                  JOINT rthumbBase\n"
"                  {\n"
"                    OFFSET -2.815680 -0.279180 0.531660\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT rthumb\n"
"                    {\n"
"                      OFFSET -0.283040 -0.142720 1.950690\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT finger1-2.r\n"
"                      {\n"
"                        OFFSET -0.915590 -2.152150 1.546760\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        JOINT finger1-3.r\n"
"                        {\n"
"                          OFFSET -3.213140 -0.470060 0.247480\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          End Site\n"
"                          {\n"
"                            OFFSET -2.521224 -0.161543 -0.511272\n"
"                          }\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"            }\n"
"          }\n"
"        }\n"
"      }\n"
"      JOINT lCollar\n"
"      {\n"
"        OFFSET 2.68224 19.2634 -4.8768\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT lShldr\n"
"        {\n"
"          OFFSET 8.77824 -1.95073 1.46304\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          JOINT lForeArm\n"
"          {\n"
"            OFFSET 28.1742 -1.7115 0.48768\n"
"            CHANNELS 3 Zrotation Xrotation Yrotation\n"
"            JOINT lHand\n"
"            {\n"
"             \n"
"                  OFFSET 21.049408 0.002200 -0.634230\n"
"                  CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                  JOINT metacarpal1.l\n"
"                  {\n"
"                    OFFSET 2.815670 -0.279180 0.531660\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT finger2-1.l\n"
"                    {\n"
"                      OFFSET 6.292930 0.272390 2.520090\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT finger2-2.l\n"
"                      {\n"
"                        OFFSET 2.310530 -0.320520 -0.060510\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        JOINT finger2-3.l\n"
"                        {\n"
"                          OFFSET 2.051030 -0.295400 -0.164880\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          End Site\n"
"                          {\n"
"                            OFFSET 2.376823 -0.681367 -0.183876\n"
"                          }\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                  JOINT metacarpal2.l\n"
"                  {\n"
"                    OFFSET 2.815670 -0.279180 0.531660\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT finger3-1.l\n"
"                    {\n"
"                      OFFSET 6.313640 0.626120 0.318530\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT finger3-2.l\n"
"                      {\n"
"                        OFFSET 3.015730 -0.589470 -0.088540\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        JOINT finger3-3.l\n"
"                        {\n"
"                          OFFSET 2.482120 -0.426270 0.076670\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          End Site\n"
"                          {\n"
"                            OFFSET 2.344170 -0.731978 0.003260\n"
"                          }\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                  JOINT __metacarpal3.l\n"
"                  {\n"
"                    OFFSET 2.815670 -0.279180 0.531660\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT metacarpal3.l\n"
"                    {\n"
"                      OFFSET 0.606080 -0.162120 -1.874870\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT finger4-1.l\n"
"                      {\n"
"                        OFFSET 5.355730 0.702050 0.402510\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        JOINT finger4-2.l\n"
"                        {\n"
"                          OFFSET 2.643900 -0.485530 -0.117510\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          JOINT finger4-3.l\n"
"                          {\n"
"                            OFFSET 2.215840 -0.353150 0.066210\n"
"                            CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                            End Site\n"
"                            {\n"
"                              OFFSET 2.350273 -0.621228 -0.046377\n"
"                            }\n"
"                          }\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                  JOINT __metacarpal4.l\n"
"                  {\n"
"                    OFFSET 2.815670 -0.279180 0.531660\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT metacarpal4.l\n"
"                    {\n"
"                      OFFSET 0.606080 -0.162120 -1.874870\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT finger5-1.l\n"
"                      {\n"
"                        OFFSET 4.761700 0.175480 -1.109600\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        JOINT finger5-2.l\n"
"                        {\n"
"                          OFFSET 1.916350 -0.173360 -0.146170\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          JOINT finger5-3.l\n"
"                          {\n"
"                            OFFSET 1.411290 -0.108670 -0.020110\n"
"                            CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                            End Site\n"
"                            {\n"
"                              OFFSET 1.799216 -0.102372 -0.078600\n"
"                            }\n"
"                          }\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"                  JOINT lthumbBase\n"
"                  {\n"
"                    OFFSET 2.815670 -0.279180 0.531660\n"
"                    CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                    JOINT lthumb\n"
"                    {\n"
"                      OFFSET 0.283040 -0.142710 1.950690\n"
"                      CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                      JOINT finger1-2.l\n"
"                      {\n"
"                        OFFSET 0.915930 -2.151960 1.546820\n"
"                        CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                        JOINT finger1-3.l\n"
"                        {\n"
"                          OFFSET 3.213210 -0.469680 0.247300\n"
"                          CHANNELS 3 Zrotation Xrotation Yrotation \n"
"                          End Site\n"
"                          {\n"
"                            OFFSET 2.521210 -0.161290 -0.511422\n"
"                          }\n"
"                        }\n"
"                      }\n"
"                    }\n"
"                  }\n"
"            }\n"
"          }\n"
"        }\n"
"      }\n"
"    }\n"
"  }\n"
"  JOINT rButtock\n"
"  {\n"
"    OFFSET -8.77824 4.35084 1.2192\n"
"    CHANNELS 3 Zrotation Xrotation Yrotation\n"
"    JOINT rThigh\n"
"    {\n"
"      OFFSET 0 -1.70687 -2.19456\n"
"      CHANNELS 3 Zrotation Xrotation Yrotation\n"
"      JOINT rShin\n"
"      {\n"
"        OFFSET 0 -36.8199 0.73152\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT rFoot\n"
"        {\n"
"         \n"
"       OFFSET 0.73152 -45.1104 -5.12064\n"
"       CHANNELS 3 Zrotation Xrotation Yrotation\n"
"       JOINT toe1-1.R\n"
"       {\n"
"        OFFSET 2.454000 -4.050002 13.194999\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT toe1-2.R\n"
"        {\n"
"         OFFSET -0.214000 -0.646000 2.427000\n"
"         CHANNELS 3 Zrotation Xrotation Yrotation\n"
"         End Site\n"
"         {\n"
"          OFFSET -0.401900 -0.827789 2.725930\n"
"         }\n"
"        }\n"
"       }\n"
"       JOINT toe2-1.R\n"
"       {\n"
"        OFFSET 0.177000 -4.299998 13.329000\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT toe2-2.R\n"
"        {\n"
"         OFFSET -0.177000 -0.323000 2.039000\n"
"         CHANNELS 3 Zrotation Xrotation Yrotation\n"
"         JOINT toe2-3.R\n"
"         {\n"
"          OFFSET -0.067000 -0.440998 1.248000\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          End Site\n"
"          {\n"
"           OFFSET -0.042990 -0.647306 1.660872\n"
"          }\n"
"         }\n"
"        }\n"
"       }\n"
"       JOINT toe3-1.R\n"
"       {\n"
"        OFFSET -1.396000 -4.461999 13.078999\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT toe3-2.R\n"
"        {\n"
"         OFFSET -0.161000 -0.247002 1.809000\n"
"         CHANNELS 3 Zrotation Xrotation Yrotation\n"
"         JOINT toe3-3.R\n"
"         {\n"
"          OFFSET -0.033000 -0.441999 1.202000\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          End Site\n"
"          {\n"
"           OFFSET 0.032040 -0.433550 1.271800\n"
"          }\n"
"         }\n"
"        }\n"
"       }\n"
"       JOINT toe4-1.R\n"
"       {\n"
"        OFFSET -2.888001 -4.480000 12.376999\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT toe4-2.R\n"
"        {\n"
"         OFFSET -0.160000 -0.331998 1.491001\n"
"         CHANNELS 3 Zrotation Xrotation Yrotation\n"
"         JOINT toe4-3.R\n"
"         {\n"
"          OFFSET 0.035999 -0.251002 1.138999\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          End Site\n"
"          {\n"
"           OFFSET -0.088911 -0.568814 0.969530\n"
"          }\n"
"         }\n"
"        }\n"
"       }\n"
"       JOINT toe5-1.R\n"
"       {\n"
"        OFFSET -4.257999 -4.467001 11.711999\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT toe5-2.R\n"
"        {\n"
"         OFFSET -0.046000 -0.265999 0.982000\n"
"         CHANNELS 3 Zrotation Xrotation Yrotation\n"
"         JOINT toe5-3.R\n"
"         {\n"
"          OFFSET 0.086999 -0.372000 0.791000\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          End Site\n"
"          {\n"
"           OFFSET -0.044329 -0.555482 1.085780\n"
"          }\n"
"         }\n"
"        }\n"
"       }\n"
"        }\n"
"      }\n"
"    }\n"
"  }\n"
"  JOINT lButtock\n"
"  {\n"
"    OFFSET 8.77824 4.35084 1.2192\n"
"    CHANNELS 3 Zrotation Xrotation Yrotation\n"
"    JOINT lThigh\n"
"    {\n"
"      OFFSET 0 -1.70687 -2.19456\n"
"      CHANNELS 3 Zrotation Xrotation Yrotation\n"
"      JOINT lShin\n"
"      {\n"
"        OFFSET 0 -36.8199 0.73152\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT lFoot\n"
"        { \n"
"\n"
"       OFFSET -0.73152 -45.1104 -5.12064\n"
"       CHANNELS 3 Zrotation Xrotation Yrotation\n"
"       JOINT toe1-1.L\n"
"       {\n"
"        OFFSET -2.454000 -4.050002 13.194999\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT toe1-2.L\n"
"        {\n"
"         OFFSET 0.214000 -0.646000 2.427000\n"
"         CHANNELS 3 Zrotation Xrotation Yrotation\n"
"         End Site\n"
"         {\n"
"          OFFSET 0.401900 -0.827789 2.725930\n"
"         }\n"
"        }\n"
"       }\n"
"       JOINT toe2-1.L\n"
"       {\n"
"        OFFSET -0.177000 -4.299998 13.329000\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT toe2-2.L\n"
"        {\n"
"         OFFSET 0.177000 -0.323000 2.039000\n"
"         CHANNELS 3 Zrotation Xrotation Yrotation\n"
"         JOINT toe2-3.L\n"
"         {\n"
"          OFFSET 0.067000 -0.440998 1.248000\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          End Site\n"
"          {\n"
"           OFFSET 0.042990 -0.647306 1.660872\n"
"          }\n"
"         }\n"
"        }\n"
"       }\n"
"       JOINT toe3-1.L\n"
"       {\n"
"        OFFSET 1.396000 -4.461999 13.078999\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT toe3-2.L\n"
"        {\n"
"         OFFSET 0.161000 -0.247002 1.809000\n"
"         CHANNELS 3 Zrotation Xrotation Yrotation\n"
"         JOINT toe3-3.L\n"
"         {\n"
"          OFFSET 0.033000 -0.441999 1.202000\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          End Site\n"
"          {\n"
"           OFFSET -0.032040 -0.433550 1.271800\n"
"          }\n"
"         }\n"
"        }\n"
"       }\n"
"       JOINT toe4-1.L\n"
"       {\n"
"        OFFSET 2.888001 -4.480000 12.376999\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT toe4-2.L\n"
"        {\n"
"         OFFSET 0.160000 -0.331998 1.491001\n"
"         CHANNELS 3 Zrotation Xrotation Yrotation\n"
"         JOINT toe4-3.L\n"
"         {\n"
"          OFFSET -0.035999 -0.251002 1.138999\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          End Site\n"
"          {\n"
"           OFFSET 0.088911 -0.568814 0.969530\n"
"          }\n"
"         }\n"
"        }\n"
"       }\n"
"       JOINT toe5-1.L\n"
"       {\n"
"        OFFSET 4.257999 -4.467001 11.711999\n"
"        CHANNELS 3 Zrotation Xrotation Yrotation\n"
"        JOINT toe5-2.L\n"
"        {\n"
"         OFFSET 0.046000 -0.265999 0.982000\n"
"         CHANNELS 3 Zrotation Xrotation Yrotation\n"
"         JOINT toe5-3.L\n"
"         {\n"
"          OFFSET -0.086999 -0.372000 0.791000\n"
"          CHANNELS 3 Zrotation Xrotation Yrotation\n"
"          End Site\n"
"          {\n"
"           OFFSET 0.044329 -0.555482 1.085780\n"
"          }\n"
"         }\n"
"        }\n"
"       }\n"
"        }\n"
"      }\n"
"    }\n"
"  }\n"
"}\n";


float * mallocVector(std::vector<float> bvhFrame);
int copyVectorToFloatArray(float * arr,std::vector<float> bvhFrame);

void * getRendererHandle();

void * getBVHMotionHandle();

void * getBVHLHandQBVHMotionHandle();
void * getBVHRHandQBVHMotionHandle();


void printBVHSummary();
void printBVHCodeVersion();

int loadCalibration(struct MocapNET2Options * options,const char* directory,const char * file);

void overrideBVHSubsystemFocalLength(float newFx,float newFy);


void convertStandaloneLHandSkeletonSerializedToBVHTransform(
                                                            struct BVH_MotionCapture * bvhMotion,
                                                            struct simpleRenderer * renderer,
                                                            struct BVH_Transform * out,
                                                            struct skeletonSerialized * in,
                                                            int skeletonSerializedInputIsAlreadyNormalized
                                                           );


void convertStandaloneRHandSkeletonSerializedToBVHTransform(
                                                            struct BVH_MotionCapture * bvhMotion,
                                                            struct simpleRenderer * renderer,
                                                            struct BVH_Transform * out,
                                                            struct skeletonSerialized * in,
                                                            int skeletonSerializedInputIsAlreadyNormalized
                                                           );

int fixBVHHip(std::vector<std::vector<float> > &bvhFrames);

/**
 * @brief After collecting a vector of BVH output vectors this call can write them to disk in BVH format
 * to make them accessible by third party 3D animation software like blender etc.
 * @param Path to output file i.e. "output.bvh"
 * @param Pointer to BVH header string, if set to null it will default to the bvhHeader found in this header file.
 * @param Vector of BVH frame vectors.
 * @retval 1=Success,0=Failure
 */
int writeBVHFile(
                  const char * filename,
                  const char * header,
                  int prependTPose,
                   std::vector<std::vector<float> > bvhFrames
                );

/**
 * @brief This call loads a BVH file into a BVH_MotionCapture* structure that is casted in a void pointer
 * to avoid all the include hassle
 * @param Path to input file i.e. "input.bvh"
 * @retval BVH_MotionCapture pointer that holds all the BVH information
 */
void * loadBVHFile(const char * filename);


/**
 * @brief This call frees all memory consumed by the BVH file loaded by a loadBVHFile call
 * @param Path to input file i.e. "input.bvh"
 * @retval 1=Success/0=Failure
 */
int freeBVHFile(void * bvhMemoryHandler);


/**
 * @brief This call opens a BVH file decodes it and returns a vector of bvh frames where each frame has all the motions in another vector
 * @param Path to input file i.e. "input.bvh"
 * @retval Vector of Vectors that hold all the BVH motions
 */
std::vector<std::vector<float> > loadBVHFileMotionFrames(const char * filename);


int scaleAllJoints(float scaleRatio);


int changeHandsScale(float newHandsScale);
int changeHeadScale(float newHeadScale);

int changeFeetDimensions(
                          float hipToKneeLength,
                          float kneeToFootLength
                         );



int getJointDimensions(
                       float * neckLength,
                       float * torsoLength,
                       float * chestWidth,
                       float * shoulderToElbowLength,
                       float * elbowToHandLength,
                       float * waistWidth,
                       float * hipToKneeLength,
                       float * kneeToFootLength,
                       float * shoeLength
                      );


int changeJointDimensions(
                          float neckLength,
                          float torsoLength,
                          float chestWidth,
                          float shoulderToElbowLength,
                          float elbowToHandLength,
                          float waistWidth,
                          float hipToKneeLength,
                          float kneeToFootLength,
                          float shoeLength
                         );


int codeOptimizationsForIKEnabled();

/**
 * @brief Initialize BVH code ( if it is present ) , supplying a null first argument uses default bvh armature
 * @retval 1=Success/0=Failure
 */
int initializeBVHConverter(const char * specificBVHFilename,int width,int height,int noIKNeeded);


unsigned int getBVHNumberOfValuesPerFrame();

unsigned int getBVHNumberOfJoints();



/**
 * @brief Get the joint ID of the parent of a joint
 * @param JointID that we want to get the parent for
 * @retval JointID of the parent/ 0 in the case of an error
 * @bug 0 can both be a valid joint and signal an error, so it needs special care
 */
int getBVHJointOffset(unsigned int currentJoint,float * x,float *y,float *z);

/**
 * @brief Get the joint ID of the parent of a joint
 * @param JointID that we want to get the parent for
 * @retval JointID of the parent/ 0 in the case of an error
 * @bug 0 can both be a valid joint and signal an error, so it needs special care
 */
unsigned int getBVHParentJoint(unsigned int currentJoint);


#if USE_BVH
 const char * getBVHJointNameFromMotionStructure(struct BVH_MotionCapture * mc,unsigned int currentJoint);
#endif

/**
 * @brief Get a string with the name of the joint
 * @param JointID that we want to get the name for
 * @retval CString with the name, null means no result
 */
const char * getBVHJointName(unsigned int currentJoint);

/**
 * @brief Return the name of a motion channel on the target C-String
 * @param Motion channel ID
 * @param Output C String that will contain the label
 * @param Size of C String
 * @retval 1=Success/0=Failure
 */
int getBVHMotionValueName(unsigned int currentMotionValue,char * target,unsigned int targetLength);


/**
 * @brief Get the jointID based on a string with the name of the joint
 * @param CString with the name we want to search for
 * @retval JointID that we want to get the name for, 0 means no result or the first joint
 */
unsigned int getBVHJointIDFromJointName(const char * jointName);




int transferVectorToMotionBufferArray(struct MotionBuffer * mb,const std::vector<float> motionVector);

/**
 * @brief This function performs the Inverse Kinematics logic. You give it the previous BVH frame, the current BVH neural network estimation as well as a skeletonSerialized
 * structure that holds the observation, it will perform Hierarchical Coordinate Descent and return a BVH vector that is closer to the observation
 * https://github.com/AmmarkoV/RGBDAcquisition/tree/master/opengl_acquisition_shared_library/opengl_depth_and_color_renderer/src/Library/MotionCaptureLoader/ik
 * @param  Vector holding the previous BVH frame
 * @param  Vector holding the current BVH frame
 * @param  A skeletonSerialized structure holding the current 2D skeleton observed
 * @param  Toggle Left hand IK
 * @param  Toggle Right hand IK
 * @param  The learning rate used for Hierarchical Coordinate Descent
 * @param  The number of iterations (i.e. total number of passes for each joint)
 * @param  The number of epochs per joint (i.e. total incremental improvements of each joint)
 * @param  The magnitude of the spring damping coefficient that keeps solutions from drifting away from previous solutions
 * @param  A flag that can cause the spring to ignore all incremental changes
 * @retval BVH Vector that is derived from bvhFrame argument and that more closely resembles inputPoints2D
 */
std::vector<float> improveBVHFrameUsingInverseKinematics(
                                                          const std::vector<float> bvhPenultimateFrame,
                                                          const std::vector<float> bvhPreviousFrame,
                                                          const std::vector<float> bvhFrame,
                                                          //--------------
                                                          unsigned int frameNumber,
                                                          //--------------
                                                          int doLeftHand,
                                                          int doRightHand,
                                                          int doFace,
                                                          struct skeletonSerialized * inputPoints2D,
                                                          float learningRate,
                                                          unsigned int iterations,
                                                          unsigned int epochs,
                                                          float spring,
                                                          unsigned int springIgnoreChanges,
                                                          int dontUseHistory,
                                                          int multiThreading
                                                        );


/**
 * @brief Initialize BVH code ( if it is present )
 * @param  A float corresponding to a BVH frame
 * @retval Vector of 2D points
 */
std::vector<std::vector<float> > convertBVHFrameTo2DPoints(const std::vector<float> bvhFrame);


std::vector<std::vector<float> > convertStandaloneLHandBVHFrameTo2DPoints(const std::vector<float> bvhFrame);

std::vector<std::vector<float> > convertStandaloneRHandBVHFrameTo2DPoints(const std::vector<float> bvhFrame);

/**
 * @brief Convert a BVH motion frame to 3D points for a given 2D viewport
 * @param  Input BVH motion frame
 * @retval Vector of 3D points
 */
std::vector<float>  convertBVHFrameToFlat3DPoints(std::vector<float> bvhFrame);


std::vector<float> convertBVHFrameToFlat3DPoints(std::vector<float> bvhFrame);

std::vector<float> convertStandaloneLHandBVHFrameToFlat3DPoints(const std::vector<float> bvhFrame);

std::vector<float> convertStandaloneRHandBVHFrameToFlat3DPoints(const std::vector<float> bvhFrame);

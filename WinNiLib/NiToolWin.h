#pragma once

#include  "tools++.h"
#include  "NiDevice.h"


#ifdef WIN64
    #define   OPENNI2_DLLNAME       "OpenNI2.dll"
    #define   NITE2_DLLNAME         "NiTE2.dll"
    #define   OPENNI_NITE_DLLNAME   "OpenNI64.dll"
    #define   KINECT_SDK_DLLNAME    "Kinect10.dll"
    #define   KINECT_FACE_DLLNAME   "FaceTrackLib.dll"
#else
    #define   OPENNI2_DLLNAME       "OpenNI2.dll"
    #define   NITE2_DLLNAME         "NiTE2.dll"
    #define   OPENNI_NITE_DLLNAME   "OpenNI.dll"
    #define   KINECT_SDK_DLLNAME    "Kinect10.dll"
    #define   KINECT_FACE_DLLNAME   "FaceTrackLib.dll"
#endif


#define   NI_DETECT_COLOR_BRIGHT    0xe0
#define   NI_DEFAULT_GROUND_LEVEL   -999
#define   NI_RING_BUFFER_SIZE       20



namespace jbxwl {

using namespace jbxl;


//
enum  NiSDK_Lib
{
    NiSDK_Default,
    NiSDK_None,
    NiSDK_Kinect,
    NiSDK_OpenNI,
    NiSDK_OpenNI2
};


// Color
void         NiSetUserColor(int label, uByte* ptr, BOOL use_image=TRUE);
unsigned int NiGetSkeletonColor(int label);


}   // name space

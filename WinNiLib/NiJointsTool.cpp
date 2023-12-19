
#include  "tools++.h"
#include  "NiJointsTool.h"


using namespace jbxl;
using namespace jbxwl;


//////////////////////////////////////////////////////////////////////////
// SDK(OpenNI, Kinect SDK)用 作業ジョイント番号．
// 二つのSDKのジョイント名はこの名前に集約される．
//

// Default or No SDK
int  NI_SDK_PELVIS    = NI_PELVIS;
int  NI_SDK_TORSO     = NI_TORSO;
int  NI_SDK_NECK      = NI_NECK;
int  NI_SDK_HEAD      = NI_HEAD;

int  NI_SDK_L_EYE     = NI_L_EYE;
int  NI_SDK_R_EYE     = NI_R_EYE;
int  NI_SDK_L_BUST    = NI_L_BUST;
int  NI_SDK_R_BUST    = NI_R_BUST;
    
int  NI_SDK_L_SHLDR   = NI_L_SHLDR;
int  NI_SDK_L_ELBOW   = NI_L_ELBOW;
int  NI_SDK_L_WRIST   = NI_L_WRIST;
int  NI_SDK_L_HAND    = NI_L_HAND;
int  NI_SDK_L_FNGRTIP = NI_L_FNGRTIP;

int  NI_SDK_R_SHLDR   = NI_R_SHLDR;
int  NI_SDK_R_ELBOW   = NI_R_ELBOW;
int  NI_SDK_R_WRIST   = NI_R_WRIST;
int  NI_SDK_R_HAND    = NI_R_HAND;
int  NI_SDK_R_FNGRTIP = NI_R_FNGRTIP;

int  NI_SDK_L_HIP     = NI_L_HIP;
int  NI_SDK_L_KNEE    = NI_L_KNEE;
int  NI_SDK_L_ANKLE   = NI_L_ANKLE;
int  NI_SDK_L_FOOT    = NI_L_FOOT;
    
int  NI_SDK_R_HIP     = NI_R_HIP;
int  NI_SDK_R_KNEE    = NI_R_KNEE;
int  NI_SDK_R_ANKLE   = NI_R_ANKLE;
int  NI_SDK_R_FOOT    = NI_R_FOOT;

int  NI_SDK_L_FINGER  = NI_L_FINGER;    // Parameter of Finger
int  NI_SDK_R_FINGER  = NI_R_FINGER;    // Parameter of Finger
int  NI_SDK_FACE      = NI_FACE;        // Parameter of Expression
int  NI_SDK_AVATAR    = NI_AVATAR;




//////////////////////////////////////////////////////////////////////////
// 回転の制約条件を設定する変数．
// 具体的な内容はライブラリ外部で指定する．
//

RBound<double>  RBound_NI_TORSO;
RBound<double>  RBound_NI_CHEST;
RBound<double>  RBound_NI_NECK;
RBound<double>  RBound_NI_HEAD;

RBound<double>  RBound_NI_L_COLLAR;
RBound<double>  RBound_NI_R_COLLAR;
RBound<double>  RBound_NI_L_SHLDR;
RBound<double>  RBound_NI_R_SHLDR;
RBound<double>  RBound_NI_L_ELBOW;
RBound<double>  RBound_NI_R_ELBOW;
RBound<double>  RBound_NI_L_WRIST;
RBound<double>  RBound_NI_R_WRIST;
RBound<double>  RBound_NI_L_HAND;
RBound<double>  RBound_NI_R_HAND;

RBound<double>  RBound_NI_L_HIP;
RBound<double>  RBound_NI_R_HIP;
RBound<double>  RBound_NI_L_KNEE;
RBound<double>  RBound_NI_R_KNEE;
RBound<double>  RBound_NI_L_ANKLE;
RBound<double>  RBound_NI_R_ANKLE;
RBound<double>  RBound_NI_L_FOOT;
RBound<double>  RBound_NI_R_FOOT;




/////////////////////////////////////////////////////////////////////////////
//

static std::string  _NiJointName[] =        // NI_TOTAL_JOINT_NUM (37)
{
    // 0         1        2        3        4       5       6        
    "PELVIS", "WAIST", "TORSO", "CHEST", "NECK", "HEAD", "SKULL", 
    //  7        8        9        10
    "L_EYE", "R_EYE", "L_BUST", "R_BUST",
    // 11           12         13        14         15        16
    "L_COLLAR", "L_SHLDR", "L_ELBOW", "L_WRIST", "L_HAND", "L_FNGRTIP",
    // 17           18         19        20         21        22
    "R_COLLAR", "R_SHLDR", "R_ELBOW", "R_WRIST", "R_HAND", "R_FNGRTIP",
    // 23       24        25         26        27
    "L_HIP", "L_KNEE", "L_ANKLE", "L_FOOT", "L_TOE",
    // 28       29        30         31        32
    "R_HIP", "R_KNEE", "R_ANKLE", "R_FOOT", "R_TOE",
    // 33          34         35       36
    "L_FINGER", "R_FINGER", "FACE", "AVATAR"
};




////////////////////////////////////////////////////////////////////////////////////////////
// 共通のジョイントの名前と番号
//

std::string  jbxwl::NiJointName(int n)
{
    std::string str = "";

    if (n>=0 && n<NI_TOTAL_JOINT_NUM) {
        str = _NiJointName[n];
    }

    return str; 
}



int   jbxwl::NiJointNum(char* name)
{
    for (int i=0; i<NI_TOTAL_JOINT_NUM; i++) {
        if (!strcmp(name, _NiJointName[i].c_str())) return i;
    }
    return -1;
}




////////////////////////////////////////////////////////////////////////////////////////////
// Library 固有のジョイント番号から共通のジョイント名の番号を得る
//

static  int  _OpenNI2JointNameNum[] =   // NI_OPENNI_JOINT_NUM (25)
{
     0,  5,  4,  2,  1,
    17, 18, 19, 20, 21, 22,
    11, 12, 13, 14, 15, 16,
    28, 29, 30, 31,
    23, 24, 25, 26
};

//
static  int  _OpenNI22JointNameNum[] =  // NI_OPENNI2_JOINT_NUM (16)
{
     0,  5,  4,
    18, 12, 19, 13, 21, 15,
     2,
    28, 23, 29, 24, 31, 26
};

//
static  int  _Kinect2JointNameNum[] =   // NI_KINECT_JOINT_NUM (20)
{
     0,  2,  4,  5, 
    12, 13, 14, 15,
    18, 19, 20, 21,
    23, 24, 25, 26,
    28, 29, 30, 31 
};

/*
static  int  _Ni2JointNameNum[] =       // NI_TOTAL_JOINT_NUM (37)
{
     0,  1,  2,  3,  4,  5,  6,
     7,  8,  9, 10,
    11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 
    23, 24, 25, 26, 27,
    28, 29, 30, 31, 32, 
    33, 34, 35, 36
};
*/



std::string  jbxwl::NiSDK2JointName(int j, NiSDK_Lib lib)
{
    std::string str = "";

    if (lib==NiSDK_OpenNI2) {
        int n = _OpenNI22JointNameNum[j];
        if (n>=0) str = _NiJointName[n];
    }
    else if (lib==NiSDK_OpenNI) {
        int n = _OpenNI2JointNameNum[j];
        if (n>=0) str = _NiJointName[n];
    }
    else if (lib==NiSDK_Kinect) {
        int n = _Kinect2JointNameNum[j];
        if (n>=0) str = _NiJointName[n];
    }
    else if (lib==NiSDK_None) {
        //str = _NiJointName[_Ni2JointNameNum[j]];
        str = _NiJointName[j];
    }

    return str;
}




////////////////////////////////////////////////////////////////////////////////////////////
// Library 固有のジョイント番号のミラージョイントの番号を得る
//

static  int  _OpenNIMirrorJointNum[] =  // NI_OPENNI_JOINT_NUM (25)
{
     0,  1,  2,  3,  4,
    11, 12, 13, 14, 15, 16,
     5,  6,  7,  8,  9, 10,
    21, 22, 23, 24,
    17, 18, 19, 20
};

//
static  int  _OpenNI2MirrorJointNum[] = // NI_OPENNI2_JOINT_NUM (16)
{
     0,  1,  2,
     4,  3,  6,  5,  8,  7,
     9,
    11, 10, 13, 12, 15, 14
};

//
static  int  _KinectMirrorJointNum[] =  // NI_KINECT_JOINT_NUM (20)
{
     0,  1,  2,  3, 
     8,  9, 10, 11,
     4,  5,  6,  7,
    16, 17, 18, 19,
    12, 13, 14, 15
};

//
static  int  _NiMirrorJointNum[] =      // NI_TOTAL_JOINT_NUM (37)
{
     0,  1,  2,  3,  4,  5,  6,
     8,  7, 10,  9, 
    17, 18, 19, 20, 21, 22, 
    11, 12, 13, 14, 15, 16,
    28, 29, 30, 31, 32, 
    23, 24, 25, 26, 27,
    34, 33, 35, 36
};



int  jbxwl::NiSDKMirrorJointNum(int j, NiSDK_Lib lib)
{
    int ret = -1;
    if (j<0) return ret;

    if (lib==NiSDK_OpenNI2) {
        ret = _OpenNI2MirrorJointNum[j];
    }
    else if (lib==NiSDK_OpenNI) {
        ret = _OpenNIMirrorJointNum[j];
    }
    else if (lib==NiSDK_Kinect) {
        ret = _KinectMirrorJointNum[j];
    }
    else if (lib==NiSDK_None) {
        ret = _NiMirrorJointNum[j];
    }

    return ret;
}




////////////////////////////////////////////////////////////////////////////////////////////
// 共通ジョイント番号から，Library固有の座標用ジョイント番号を得る
//

// HAND->WRIST, FOOT->ANKLE
static  int  _Ni2OpenNIPosJointNum[] =      // NI_TOTAL_JOINT_NUM (37)
{
     0, -1,  3, -1,  2,  1, -1, 
    -1, -1, -1, -1,
    -1, 12, 13, 15, -1, -1,
    -1,  6,  7,  9, -1, -1,
    21, 22, 24, -1, -1,
    17, 18, 20, -1, -1,
    -1, -1, -1, -1
};

// HAND->WRIST, FOOT->ANKLE
static  int  _Ni2OpenNI2PosJointNum[] =     // NI_TOTAL_JOINT_NUM (37)
{
     0, -1,  9, -1,  2,  1, -1, 
    -1, -1, -1, -1,
    -1,  4,  6,  8, -1, -1,
    -1,  3,  5,  7, -1, -1,
    11, 13, 15, -1, -1,
    10, 12, 14, -1, -1,
    -1, -1, -1, -1
};

//
static  int  _Ni2KinectPosJointNum[] =      // NI_TOTAL_JOINT_NUM (37)
{
     0, -1,  1, -1,  2,  3, -1, 
    -1, -1, -1, -1,
    -1,  4,  5,  6,  7, -1,
    -1,  8,  9, 10, 11, -1,
    12, 13, 14, 15, -1,
    16, 17, 18, 19, -1,
    -1, -1, -1, -1
};


//
// 暫定的!!!!
/*
static  int  _Ni2ValidPosJointNum[] =       // NI_TOTAL_JOINT_NUM (37)
{
     0, -1,  2,  3,  4,  5,  -1, 
     7,  8,  9, 10,
    11, 12, 13, 14, -1, -1,
    17, 18, 19, 20, -1, -1,
    23, 24, 25, 26, -1,
    28, 29, 30, 31, -1,
    33, 34, 35, 36
};*/



int  jbxwl::Ni2SDKPosJointNum(int j, NiSDK_Lib lib)
{
    int ret = -1;

    if (lib==NiSDK_OpenNI2) {
        ret = _Ni2OpenNI2PosJointNum[j];
    }
    else if (lib==NiSDK_OpenNI) {
        ret = _Ni2OpenNIPosJointNum[j];
    }
    else if (lib==NiSDK_Kinect) {
        ret = _Ni2KinectPosJointNum[j];
    }
    else if (lib==NiSDK_None) {
        //ret = _Ni2ValidPosJointNum[j];
        ret = j;
    }

    return ret;
}




////////////////////////////////////////////////////////////////////////////////////////////
// 共通ジョイント番号から，Library固有の回転用ジョイント番号を得る

static  int  _Ni2OpenNIRotJointNum[] =      // NI_TOTAL_JOINT_NUM (37)
{
//   0, -1,  3, -1,  2,  1, -1,
     0, -1,  3, -1,  2, -1, -1,     // exclude HEAD
    -1, -1, -1, -1,
    -1, 12, 13, -1, -1, -1,
    -1,  6,  7, -1, -1, -1,
    21, 22, -1, -1, -1,
    17, 18, -1, -1, -1,
    -1, -1, -1, -1
};

//
static  int  _Ni2OpenNI2RotJointNum[] =     // NI_TOTAL_JOINT_NUM (37)
{
     0, -1,  9, -1,  2, -1, -1, 
    -1, -1, -1, -1,
    -1,  4,  6, -1, -1, -1,
    -1,  3,  5, -1, -1, -1,
    11, 13, -1, -1, -1,
    10, 12, -1, -1, -1,
    -1, -1, -1, -1
};

//
static  int  _Ni2KinectRotJointNum[] =      // NI_TOTAL_JOINT_NUM (37)
{
     0, -1,  1, -1,  2,  3, -1,
    -1, -1, -1, -1,
    -1,  4,  5,  6, -1, -1,
    -1,  8,  9, 10, -1, -1,
    12, 13, 14, -1, -1,
    16, 17, 18, -1, -1,
    -1, -1, -1, -1
};

//
static  int  _Ni2RotJointNum[] =            // NI_TOTAL_JOINT_NUM (37)
{
     0,  1,  2,  3,  4,  5, -1,  
     7,  8,  9, 10, 
    11, 12, 13, 14, 15, -1, 
    17, 18, 19, 20, 21, -1, 
    23, 24, 25, 26, -1,
    28, 29, 30, 31, -1, 
    33, 34, 35, 36
};



int  jbxwl::Ni2SDKRotJointNum(int j, NiSDK_Lib lib)
{
    int ret = -1;

    if (lib==NiSDK_OpenNI2) {
        ret = _Ni2OpenNI2RotJointNum[j];
    }
    else if (lib==NiSDK_OpenNI) {
        ret = _Ni2OpenNIRotJointNum[j];
    }
    else if (lib==NiSDK_Kinect) {
        ret = _Ni2KinectRotJointNum[j];
    }
    else if (lib==NiSDK_None) {
        ret = _Ni2RotJointNum[j];
    }

    return ret;
}




////////////////////////////////////////////////////////////////////////////////////////////
// 共通のジョイント名から Library 固有のジョイント番号を得る

/*
static  int  _JointNameNum2Ni[] =       // NI_TOTAL_JOINT_NUM (37)
{
     0,  1,  2,  3,  4,  5,  6,
     7,  8,  9, 10,
    11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 
    23, 24, 25, 26, 27,
    28, 29, 30, 31, 32, 
    33, 34, 35, 36
};
*/

//
int   jbxwl::JointName2NiSDK(char* jname, NiSDK_Lib lib)
{
    int ret = -1;

    int j;
    for (j=0; j<NI_TOTAL_JOINT_NUM; j++) {
        if (!strcasecmp(jname, (char*)_NiJointName[j].c_str())) break;
    }

    if (j<NI_TOTAL_JOINT_NUM) {
        ret = Ni2SDKJointNum(j, lib);
    }
    else if (j==NI_TOTAL_JOINT_NUM) {
        //DEBUG_INFO("JointName2NiSDK: Unknown Joint Name = %s\n", jname);
    }

    return ret;
}




///////////////////////////////////////////////////////////////////////////
//

// OpenNI
void  jbxwl::NiSetOpenNIJointNums(void)
{
    NI_SDK_PELVIS    =  0;  //
    NI_SDK_TORSO     =  3;  // XN_SKEL_TORSO
    NI_SDK_NECK      =  2;  // XN_SKEL_NECK
    NI_SDK_HEAD      =  1;  // XN_SKEL_HEAD

    NI_SDK_L_EYE     = -1;
    NI_SDK_R_EYE     = -1;
    NI_SDK_L_BUST    = -1;
    NI_SDK_R_BUST    = -1;
    
    NI_SDK_L_SHLDR   = 12;  // XN_SKEL_RIGHT_SHOULDER
    NI_SDK_L_ELBOW   = 13;  // XN_SKEL_RIGHT_ELBOW
    NI_SDK_L_WRIST   = 15;  // XN_SKEL_RIGHT_HAND
    NI_SDK_L_HAND    = -1;
    NI_SDK_L_FNGRTIP = -1;

    NI_SDK_R_SHLDR   =  6;  // XN_SKEL_LEFT_SHOULDER
    NI_SDK_R_ELBOW   =  7;  // XN_SKEL_LEFT_ELBOW
    NI_SDK_R_WRIST   =  9;  // XN_SKEL_LEFT_HAND
    NI_SDK_R_HAND    = -1;
    NI_SDK_R_FNGRTIP = -1;
    
    NI_SDK_L_HIP     = 21;  // XN_SKEL_RIGHT_HIP
    NI_SDK_L_KNEE    = 22;  // XN_SKEL_RIGHT_KNEE
    NI_SDK_L_ANKLE   = 24;  // XN_SKEL_RIGHT_FOOT
    NI_SDK_L_FOOT    = -1;

    NI_SDK_R_HIP     = 17;  // XN_SKEL_LEFT_HIP
    NI_SDK_R_KNEE    = 18;  // XN_SKEL_LEFT_KNEE
    NI_SDK_R_ANKLE   = 20;  // XN_SKEL_LEFT_FOOT
    NI_SDK_R_FOOT    = -1;

    NI_SDK_L_FINGER  = -1;
    NI_SDK_R_FINGER  = -1;
    NI_SDK_FACE      = -1;
    NI_SDK_AVATAR    = -1;

    return;
}



// OpenNI2
void  jbxwl::NiSetOpenNI2JointNums(void)
{
    NI_SDK_PELVIS    =  0;  //
    NI_SDK_TORSO     =  9;  // NITE_JOINT_TORSO + 1
    NI_SDK_NECK      =  2;  // NITE_JOINT_NECK + 1
    NI_SDK_HEAD      =  1;  // NITE_JOINT_HEAD + 1

    NI_SDK_L_EYE     = -1;
    NI_SDK_R_EYE     = -1;
    NI_SDK_L_BUST    = -1;
    NI_SDK_R_BUST    = -1;
    
    NI_SDK_L_SHLDR   =  4;  // NITE_JOINT_RIGHT_SHOULDER + 1
    NI_SDK_L_ELBOW   =  6;  // NITE_JOINT_RIGHT_ELBOW + 1
    NI_SDK_L_WRIST   =  8;  // NITE_JOINT_RIGHT_HAND + 1
    NI_SDK_L_HAND    = -1;
    NI_SDK_L_FNGRTIP = -1;

    NI_SDK_R_SHLDR   =  3;  // NITE_JOINT_LEFT_SHOULDER + 1
    NI_SDK_R_ELBOW   =  5;  // NITE_JOINT_LEFT_ELBOW + 1
    NI_SDK_R_WRIST   =  7;  // NITE_JOINT_LEFT_HAND + 1
    NI_SDK_R_HAND    = -1;
    NI_SDK_R_FNGRTIP = -1;
    
    NI_SDK_L_HIP     = 11;  // NITE_JOINT_RIGHT_HIP + 1
    NI_SDK_L_KNEE    = 13;  // NITE_JOINT_RIGHT_KNEE + 1
    NI_SDK_L_ANKLE   = 15;  // NITE_JOINT_RIGHT_FOOT + 1
    NI_SDK_L_FOOT    = -1;

    NI_SDK_R_HIP     = 10;  // NITE_JOINT_LEFT_HIP + 1
    NI_SDK_R_KNEE    = 12;  // NITE_JOINT_LEFT_KNEE + 1
    NI_SDK_R_ANKLE   = 14;  // NITE_JOINT_LEFT_FOOT + 1
    NI_SDK_R_FOOT    = -1;

    NI_SDK_L_FINGER  = -1;
    NI_SDK_R_FINGER  = -1;
    NI_SDK_FACE      = -1;
    NI_SDK_AVATAR    = -1;

    return;
}



// Kinect SDK
void  jbxwl::NiSetKinectJointNums(void)
{
    NI_SDK_PELVIS    =  0;  // NUI_SKELETON_POSITION_HIP_CENTER
    NI_SDK_TORSO     =  1;  // SPINE は書き換えて使用 
    NI_SDK_NECK      =  2;  // NUI_SKELETON_POSITION_SHOULDER_CENTER
    NI_SDK_HEAD      =  3;  // NUI_SKELETON_POSITION_HEAD

    NI_SDK_L_EYE     = -1;
    NI_SDK_R_EYE     = -1;
    NI_SDK_L_BUST    = -1;
    NI_SDK_R_BUST    = -1;
        
    NI_SDK_L_SHLDR   =  4;  // NUI_SKELETON_POSITION_SHOULDER_LEFT
    NI_SDK_L_ELBOW   =  5;  // NUI_SKELETON_POSITION_ELBOW_LEFT
    NI_SDK_L_WRIST   =  6;  // NUI_SKELETON_POSITION_WRIST_LEFT
    NI_SDK_L_HAND    =  7;  // NUI_SKELETON_POSITION_HAND_LEFT
    NI_SDK_L_FNGRTIP = -1;
    
    NI_SDK_R_SHLDR   =  8;  // NUI_SKELETON_POSITION_SHOULDER_RIGHT
    NI_SDK_R_ELBOW   =  9;  // NUI_SKELETON_POSITION_ELBOW_RIGHT
    NI_SDK_R_WRIST   = 10;  // NUI_SKELETON_POSITION_WRIST_RIGHT
    NI_SDK_R_HAND    = 11;  // NUI_SKELETON_POSITION_HAND_RIGHT
    NI_SDK_R_FNGRTIP = -1;

    NI_SDK_L_HIP     = 12;  // NUI_SKELETON_POSITION_HIP_LEFT
    NI_SDK_L_KNEE    = 13;  // NUI_SKELETON_POSITION_KNEE_LEFT
    NI_SDK_L_ANKLE   = 14;  // NUI_SKELETON_POSITION_ANKLE_LEFT
    NI_SDK_L_FOOT    = 15;  // NUI_SKELETON_POSITION_FOOT_LEFT
    
    NI_SDK_R_HIP     = 16;  // NUI_SKELETON_POSITION_HIP_RIGHT
    NI_SDK_R_KNEE    = 17;  // NUI_SKELETON_POSITION_KNEE_RIGHT
    NI_SDK_R_ANKLE   = 18;  // NUI_SKELETON_POSITION_ANKLE_RIGHT
    NI_SDK_R_FOOT    = 19;  // NUI_SKELETON_POSITION_FOOT_RIGHT

    NI_SDK_L_FINGER  = -1;
    NI_SDK_R_FINGER  = -1;
    NI_SDK_FACE      = -1;
    NI_SDK_AVATAR    = -1;

    return;
}




//////////////////////////////////////////////////////////////////////////////////////////////////
//  共通ジョイントのクラス
//      ジョイントを抽象化するクラス

void  CNiJoints::init(void)
{
    sdk_lib     = NiSDK_None;

    posVect     = NULL;
    rotQuat     = NULL;
    jntAngl     = NULL;

    posRing     = NULL;
    rotRing     = NULL;

    clearNiJointsData();
}



void  CNiJoints::clearNiJointsData(void)
{
    //
    for (int j=0; j<NI_TOTAL_JOINT_NUM; j++) {
        //
        niPosVect[j].init(-1.0);
        niRotQuat[j].init(-1.0);
        niJntAngl[j] = 0.0;
    }

    return;
}



void  CNiJoints::connectJointsData(Vector<double>* vct, Quaternion<double>* qut, 
                                   Vector<int>* crd, double* agl, CRingBuffer* vrg, CRingBuffer* qrg)
{
    posVect = vct;
    rotQuat = qut;
    jntAngl = agl;
    crdVect = crd;
    posRing = vrg;
    rotRing = qrg;
}



void  CNiJoints::copyJoints2NiJoints(BOOL mirror)
{
    if (posVect!=NULL) {
        for (int j=0; j<NI_TOTAL_JOINT_NUM; j++) {
            int n = Ni2SDKPosJointNum(j, sdk_lib);
            if (n>=0) {
                niPosVect[j] = posVect[n];
            }
        }
    }

    if (rotQuat!=NULL) {
        for (int j=0; j<NI_TOTAL_JOINT_NUM; j++) {
            int n = Ni2SDKRotJointNum(j, sdk_lib);
            if (n>=0) {
                niRotQuat[j] = rotQuat[n];
            }
        }
    }

    if (jntAngl!=NULL) {
        for (int j=0; j<NI_TOTAL_JOINT_NUM; j++) {
            int n = Ni2SDKRotJointNum(j, sdk_lib);
            if (n>=0) {
                //if (mirror) n = NiSDKMirrorJointNum(n, sdk_lib);
                niJntAngl[j] = jntAngl[n];
            }
        }
    }
    
    return;
}




//////////////////////////////////////////////////////////////////////////////////////////////////
// Frame Data

/**
NiFrameData*  jbxwl::makeVarFrameData(int frame, int* joint_num, int* frame_num)

フレームデータの格納先メモリ（フレーム毎のジョイント数とフレーム間隔が可変）を作成，初期化．

@param frame     フレームの数
@param joint_num ジョイント数
@param frame_num フレームの番号（時間）の入った配列．NULLならフレーム間隔一定．
@return フレームデータを格納するデータ領域へのポインタ

*/
NiFrameData*  jbxwl::makeFramesData(int frame, int jnum, int* frame_num)
{
    if (jnum<0) return NULL;    // 0 is OK for dummy

    NiFrameData* frmdata = (NiFrameData*)malloc(frame*sizeof(NiFrameData));
    if (frmdata==NULL) return NULL;
    memset(frmdata, 0, frame*sizeof(NiFrameData));

    for (int i=0; i<frame; i++) {
        if (frame_num!=NULL) {
            frmdata[i].frmn = frame_num[i];
            frmdata[i].msec = frame_num[i];     // デフォルトはフレーム間 1ms
        }
        else {
            frmdata[i].frmn = i;
            frmdata[i].msec = i;
        }
        frmdata[i].jnum = jnum;

        if (frmdata[i].jnum>0) {
            frmdata[i].jdat = (NiJointData*)malloc(frmdata[i].jnum*sizeof(NiJointData));
            if (frmdata[i].jdat==NULL) {
                freeFramesData(frmdata, frame);
                return NULL;
            }
            //
            for (int j=0; j<frmdata[i].jnum; j++) {
                frmdata[i].jdat[j].joint = -1;
                frmdata[i].jdat[j].index = -1;
                frmdata[i].jdat[j].vect.init(-1.0);
                frmdata[i].jdat[j].quat.init(-1.0);
            }
        }
        else {
            frmdata[i].jdat = NULL;     // for dummy
        }
    }

    return frmdata;
}



/**
NiFrameData*  jbxwl::makeFramesData(int frame, int jnum)

フレームデータの格納先メモリ（ジョイント数固定）を作成，初期化．@n
フレーム毎のジョイント数，フレーム間隔が一定

@param frame     フレームの数
@param joint_num ジョイントの数
@return フレームデータを格納するデータ領域へのポインタ
*/
/*
NiFrameData*  jbxwl::makeFramesData(int frame, int jnum)
{
    if (jnum<0) return NULL;

    int* joint_num = (int*)malloc(frame*sizeof(int));
    if (joint_num==NULL) return NULL;
    for (int i=0; i<frame; i++) joint_num[i] = jnum;

    NiFrameData* frmdata = makeVarFrameData(frame, joint_num, NULL);
    ::free(joint_num);

    return frmdata;
}*/



void  jbxwl::freeFramesData(NiFrameData* frmdata, int frm_num)
{
    if (frmdata==NULL || frm_num<=0) return;

    for (int i=0; i<frm_num; i++) {
        if (frmdata[i].jdat!=NULL) {
            ::free(frmdata[i].jdat);
            frmdata[i].jdat = NULL;
        }
    }

    ::free(frmdata);
    return;
}




//////////////////////////////////////////////////////////////////////////////////////////////////
// 回転角の補正

//
// tmin が 0.0の場合は，補正は適用されない．
// 
void  jbxwl::NiInitRBoundJointsRotation()
{
    // Initialize
    RBound_NI_TORSO.init();
    RBound_NI_CHEST.init();
    RBound_NI_NECK.init();
    RBound_NI_HEAD.init();

    RBound_NI_L_COLLAR.init();
    RBound_NI_R_COLLAR.init();
    RBound_NI_L_SHLDR.init();
    RBound_NI_R_SHLDR.init();
    RBound_NI_L_ELBOW.init();
    RBound_NI_R_ELBOW.init();
    RBound_NI_L_WRIST.init();
    RBound_NI_R_WRIST.init();
    RBound_NI_L_HAND.init();
    RBound_NI_R_HAND.init();

    RBound_NI_L_HIP.init();
    RBound_NI_R_HIP.init();
    RBound_NI_L_KNEE.init();
    RBound_NI_R_KNEE.init();
    RBound_NI_L_ANKLE.init();
    RBound_NI_R_ANKLE.init();
    RBound_NI_L_FOOT.init();
    RBound_NI_R_FOOT.init();

    return;
}



void  jbxwl::NiCorrectJointsRotation(Quaternion<double>* quat, NiSDK_Lib lib)
{
    int   joint;

    // TORSO
    joint = Ni2SDKRotJointNum(NI_TORSO, lib);   // abdomen
    if (joint>0) NiCorrectJointEuler(NI_TORSO, quat+joint, RBound_NI_TORSO);
    // CHEST
    joint = Ni2SDKRotJointNum(NI_CHEST, lib);
    if (joint>0) NiCorrectJointEuler(NI_CHEST, quat+joint, RBound_NI_CHEST);
    // NECK
    joint = Ni2SDKRotJointNum(NI_NECK, lib);
    if (joint>0) NiCorrectJointEuler(NI_NECK,  quat+joint, RBound_NI_NECK);
    // HEAD
    joint = Ni2SDKRotJointNum(NI_HEAD, lib);
    if (joint>0) NiCorrectJointEuler(NI_HEAD,  quat+joint, RBound_NI_HEAD);

    // L_COLLAR
    joint = Ni2SDKRotJointNum(NI_L_COLLAR, lib);
    if (joint>0) NiCorrectJointEuler(NI_L_COLLAR, quat+joint, RBound_NI_L_COLLAR);
    // R_COLLAR
    joint = Ni2SDKRotJointNum(NI_R_COLLAR, lib);
    if (joint>0) NiCorrectJointEuler(NI_R_COLLAR, quat+joint, RBound_NI_R_COLLAR);
    // L_SHLDR
    joint = Ni2SDKRotJointNum(NI_L_SHLDR, lib);
    if (joint>0) NiCorrectJointEuler(NI_L_SHLDR,  quat+joint, RBound_NI_L_SHLDR);
    // R_SHLDR
    joint = Ni2SDKRotJointNum(NI_R_SHLDR, lib);
    if (joint>0) NiCorrectJointEuler(NI_R_SHLDR,  quat+joint, RBound_NI_R_SHLDR);
    // L_ELBOW
    joint = Ni2SDKRotJointNum(NI_L_ELBOW, lib);
    if (joint>0) NiCorrectJointEuler(NI_L_ELBOW,  quat+joint, RBound_NI_L_ELBOW);
    // R_ELBOW
    joint = Ni2SDKRotJointNum(NI_R_ELBOW, lib);
    if (joint>0) NiCorrectJointEuler(NI_R_ELBOW,  quat+joint, RBound_NI_R_ELBOW);
    // L_WRIST
    joint = Ni2SDKRotJointNum(NI_L_WRIST, lib);
    if (joint>0) NiCorrectJointEuler(NI_L_WRIST,  quat+joint, RBound_NI_L_WRIST);
    // R_WRIST
    joint = Ni2SDKRotJointNum(NI_R_WRIST, lib);
    if (joint>0) NiCorrectJointEuler(NI_R_WRIST,  quat+joint, RBound_NI_R_WRIST);
    // L_HAND
    joint = Ni2SDKRotJointNum(NI_L_HAND, lib);
    if (joint>0) NiCorrectJointEuler(NI_L_HAND,   quat+joint, RBound_NI_L_HAND);
    // R_HAND
    joint = Ni2SDKRotJointNum(NI_R_HAND, lib);
    if (joint>0) NiCorrectJointEuler(NI_R_HAND,   quat+joint, RBound_NI_R_HAND);

    // L_HIP
    joint = Ni2SDKRotJointNum(NI_L_HIP, lib);
    if (joint>0) NiCorrectJointEuler(NI_L_HIP,   quat+joint, RBound_NI_L_HIP);
    // R_HIP
    joint = Ni2SDKRotJointNum(NI_R_HIP, lib);
    if (joint>0) NiCorrectJointEuler(NI_R_HIP,   quat+joint, RBound_NI_R_HIP);
    // L_KNEE
    joint = Ni2SDKRotJointNum(NI_L_KNEE, lib);
    if (joint>0) NiCorrectJointEuler(NI_L_KNEE,  quat+joint, RBound_NI_L_KNEE);
    // R_KNEE
    joint = Ni2SDKRotJointNum(NI_R_KNEE, lib);
    if (joint>0) NiCorrectJointEuler(NI_R_KNEE,  quat+joint, RBound_NI_R_KNEE);
    // L_ANKLE
    joint = Ni2SDKRotJointNum(NI_L_ANKLE, lib);
    if (joint>0) NiCorrectJointEuler(NI_L_ANKLE, quat+joint, RBound_NI_L_ANKLE);
    // R_ANKLE
    joint = Ni2SDKRotJointNum(NI_R_ANKLE, lib);
    if (joint>0) NiCorrectJointEuler(NI_R_ANKLE, quat+joint, RBound_NI_R_ANKLE);
    // L_FOOT
    joint = Ni2SDKRotJointNum(NI_L_FOOT, lib);
    if (joint>0) NiCorrectJointEuler(NI_L_FOOT,  quat+joint, RBound_NI_L_FOOT);
    // R_FOOT
    joint = Ni2SDKRotJointNum(NI_R_FOOT, lib);
    if (joint>0) NiCorrectJointEuler(NI_R_FOOT,  quat+joint, RBound_NI_R_FOOT);

    return;
}



void  jbxwl::NiCorrectJointEuler(int joint, Quaternion<double>* quat, RBound<double> rb)
{
    if (rb.tmin==0.0) return;               // 修正しない

    Vector<double> eul[2];
    
    eul[0].set(0.0, 0.0, 0.0, 0.0, -1.0);
    NiGetJointEuler(joint, quat, eul);
    if (eul[0].c==-1.0) return;             // オイラー角への変換に失敗

    //
    BOOL chgf = FALSE;
    if      (eul[0].x<rb.xmin) { eul[0].x = rb.xmin; chgf = TRUE;}
    else if (eul[0].x>rb.xmax) { eul[0].x = rb.xmax; chgf = TRUE;}
    if      (eul[0].y<rb.ymin) { eul[0].y = rb.ymin; chgf = TRUE;}
    else if (eul[0].y>rb.ymax) { eul[0].y = rb.ymax; chgf = TRUE;}
    if      (eul[0].z<rb.zmin) { eul[0].z = rb.zmin; chgf = TRUE;}
    else if (eul[0].z>rb.zmax) { eul[0].z = rb.zmax; chgf = TRUE;}
    if (!chgf) return;

    //
    chgf = FALSE;
    if      (eul[1].x<rb.xmin) { eul[1].x = rb.xmin; chgf = TRUE;}
    else if (eul[1].x>rb.xmax) { eul[1].x = rb.xmax; chgf = TRUE;}
    if      (eul[1].y<rb.ymin) { eul[1].y = rb.ymin; chgf = TRUE;}
    else if (eul[1].y>rb.ymax) { eul[1].y = rb.ymax; chgf = TRUE;}
    if      (eul[1].z<rb.zmin) { eul[1].z = rb.zmin; chgf = TRUE;}
    else if (eul[1].z>rb.zmax) { eul[1].z = rb.zmax; chgf = TRUE;}
    if (!chgf) return;

    //
    NiSetJointEuler(joint, eul, quat);

    return;
}



void  jbxwl::NiGetJointEuler(int joint, Quaternion<double>* quat, Vector<double>* vect)
{
    if (vect==NULL) return;

    Vector<double> vt[2];
    vt[0].set(0.0, 0.0, 0.0, 0.0, -1.0);

    if (joint==NI_L_ANKLE || joint==NI_R_ANKLE) {
        Quaternion2ExtEulerXZY(*quat, vt);
        if (vt[0].c!=-1.0) { 
            vect[0].set(vt[0].element1(), vt[0].element3(), vt[0].element2());
            vect[1].set(vt[1].element1(), vt[1].element3(), vt[1].element2());
        }
    }
    else if (joint==NI_L_COLLAR || joint==NI_R_COLLAR) {
        Quaternion2ExtEulerYXZ(*quat, vt);
        if (vt[0].c!=-1.0) { 
            vect[0].set(vt[0].element2(), vt[0].element1(), vt[0].element3());
            vect[1].set(vt[1].element2(), vt[1].element1(), vt[1].element3());
        }
    }
    else if (joint==NI_L_SHLDR || joint==NI_R_SHLDR) {
        Quaternion2ExtEulerYZX(*quat, vt);
        if (vt[0].c!=-1.0) { 
            vect[0].set(vt[0].element3(), vt[0].element1(), vt[0].element2());
            vect[1].set(vt[1].element3(), vt[1].element1(), vt[1].element2());
        }
    }
    else if (joint==NI_L_ELBOW || joint==NI_R_ELBOW) {
        Quaternion2ExtEulerYXZ(*quat, vt);
        if (vt[0].c!=-1.0) { 
            vect[0].set(vt[0].element2(), vt[0].element1(), vt[0].element3());
            vect[1].set(vt[1].element2(), vt[1].element1(), vt[1].element3());
        }
    }
    else if (joint==NI_L_WRIST || joint==NI_R_WRIST) {
        Quaternion2ExtEulerYZX(*quat, vt);
        if (vt[0].c!=-1.0) { 
            vect[0].set(vt[0].element3(), vt[0].element1(), vt[0].element2());
            vect[1].set(vt[1].element3(), vt[1].element1(), vt[1].element2());
        }
    }
    else {
        Quaternion2ExtEulerZXY(*quat, vt);
        if (vt[0].c!=-1.0) { 
            vect[0].set(vt[0].element2(), vt[0].element3(), vt[0].element1());
            vect[1].set(vt[1].element2(), vt[1].element3(), vt[1].element1());
        }
    }

    return;
}



void  jbxwl::NiSetJointEuler(int joint, Vector<double>* vect, Quaternion<double>* quat)
{   
    Quaternion<double> qut[2], rot[2];
    Vector<double>     eul[2];

    if (joint==NI_L_ANKLE || joint==NI_R_ANKLE) {
        eul[0].set(vect[0].x, vect[0].z, vect[0].y);
        eul[1].set(vect[1].x, vect[1].z, vect[1].y);
        qut[0].setExtEulerXZY(eul[0]);
        qut[1].setExtEulerXZY(eul[1]);
    }
    else if (joint==NI_L_COLLAR || joint==NI_R_COLLAR) {
        eul[0].set(vect[0].y, vect[0].x, vect[0].z);
        eul[1].set(vect[1].y, vect[1].x, vect[1].z);
        qut[0].setExtEulerYXZ(eul[0]);
        qut[1].setExtEulerYXZ(eul[1]);
    }
    else if (joint==NI_L_SHLDR || joint==NI_R_SHLDR) {
        eul[0].set(vect[0].y, vect[0].z, vect[0].x);
        eul[1].set(vect[1].y, vect[1].z, vect[1].x);
        qut[0].setExtEulerYZX(eul[0]);
        qut[1].setExtEulerYZX(eul[1]);
    }
    else if (joint==NI_L_ELBOW || joint==NI_R_ELBOW) {
        eul[0].set(vect[0].y, vect[0].x, vect[0].z);
        eul[1].set(vect[1].y, vect[1].x, vect[1].z);
        qut[0].setExtEulerYXZ(eul[0]);
        qut[1].setExtEulerYXZ(eul[1]);
    }
    else if (joint==NI_L_WRIST || joint==NI_R_WRIST) {
        eul[0].set(vect[0].y, vect[0].z, vect[0].x);
        eul[1].set(vect[1].y, vect[1].z, vect[1].x);
        qut[0].setExtEulerYZX(eul[0]);
        qut[1].setExtEulerYZX(eul[1]);
    }
    else {
        eul[0].set(vect[0].z, vect[0].x, vect[0].y);
        eul[1].set(vect[1].z, vect[1].x, vect[1].y);
        qut[0].setExtEulerZXY(eul[0]);
        qut[1].setExtEulerZXY(eul[1]);
    }

    //
    rot[0] = qut[0]*~(*quat);
    rot[1] = qut[1]*~(*quat);
    //
    if (rot[0].getAngle()<=rot[1].getAngle()) {     // 回転角の小さい方を選択
        *quat = qut[0];
    }
    else {
        *quat = qut[1];
    }

    return;
}




//////////////////////////////////////////////////////////////////////////////////////////////////
//  CBaseFrameTool Class

CBaseFrameTool::CBaseFrameTool(void)  
{ 
    init_data();
}



CBaseFrameTool::~CBaseFrameTool(void) 
{
    DEBUG_INFO("DESTRUCTOR: CBaseFrameTool\n");

    free_data();
}



void  CBaseFrameTool::free_data(void)
{
    if (framesData!=NULL) freeFramesData(framesData, frames_num);
    if (jointsData!=NULL) ::free(jointsData);

    if (posVect!=NULL) ::free(posVect);
    if (rotQuat!=NULL) ::free(rotQuat);

    framesData = NULL; 
    jointsData = NULL;

    posVect    = NULL;
    rotQuat    = NULL;
    
    clear_data();
}



void  CBaseFrameTool::clear_data(void)
{
    frames_num = 0;
    joints_num = 0;

    start_time = 0;
    stop_time  = 0;
    exec_time  = 0;
}



void  CBaseFrameTool::init_data(void)
{
    framesData = NULL; 
    jointsData = NULL;

    posVect    = NULL;
    rotQuat    = NULL;

    clear_data();
}



/////////////////////////////////////////////////////////////////

void  CBaseFrameTool::clearJointsData(int jnum)
{
    if (jointsData!=NULL) {
        for (int j=0; j<jnum; j++) {
            jointsData[j].index = -1;
            jointsData[j].joint = -1;
            jointsData[j].vect.init(-1.0);
            jointsData[j].quat.init(-1.0);
        }
    }
}



void  CBaseFrameTool::clearVectorData(int jnum)
{
    if (posVect!=NULL && rotQuat!=NULL) {
        for (int j=0; j<jnum; j++) {
            posVect[j].init(-1.0);
            rotQuat[j].init(-1.0);
        }
    }
}

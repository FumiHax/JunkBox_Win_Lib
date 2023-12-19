#pragma once

#include  "NiToolWin.h"
#include  "Vector.h"
#include  "Rotation.h"
#include  "RingBuffer.h"


#define   NI_JTXT_FILE_ID       "NI_JOINTS_TEXT_FILE"


#define   NI_NORML_JOINT_NUM    33  // NORMAL JOINTS
#define   NI_SPECL_JOINT_NUM    4   // NI_L_FINGER, NI_R_FINGER, NI_FACE, NI_AVATAR
#define   NI_TOTAL_JOINT_NUM    37  // 33 + 4


#define   NI_OPENNI_JOINT_NUM   25  // OPENNI_JOINT_NUM
#define   NI_OPENNI2_JOINT_NUM  16  // OPENNI2_JOINT_NUM
#define   NI_KINECT_JOINT_NUM   20  // KINECT_JOINT_NUM, NUI_SKELETON_POSITION_COUNT

//
#define   NI_SDK_IS_TRACKING    0x0100
#define   NI_SDK_PROFILE_UPPER  0x1000
#define   NI_SDK_AVATAR_TRAILER 0x2000

#define   NI_PROFILE_UPPER      3


//////////////////////////////////////////////////////////////////////////
// 共通ジョイント番号

#define   NI_PELVIS      0
#define   NI_WAIST       1
#define   NI_TORSO       2
#define   NI_CHEST       3
#define   NI_NECK        4
#define   NI_HEAD        5
#define   NI_SKULL       6

#define   NI_L_EYE       7
#define   NI_R_EYE       8
#define   NI_L_BUST      9
#define   NI_R_BUST     10

#define   NI_L_COLLAR   11
#define   NI_L_SHLDR    12
#define   NI_L_ELBOW    13
#define   NI_L_WRIST    14
#define   NI_L_HAND     15
#define   NI_L_FNGRTIP  16

#define   NI_R_COLLAR   17
#define   NI_R_SHLDR    18
#define   NI_R_ELBOW    19
#define   NI_R_WRIST    20
#define   NI_R_HAND     21
#define   NI_R_FNGRTIP  22

#define   NI_L_HIP      23
#define   NI_L_KNEE     24
#define   NI_L_ANKLE    25
#define   NI_L_FOOT     26
#define   NI_L_TOE      27
    
#define   NI_R_HIP      28
#define   NI_R_KNEE     29
#define   NI_R_ANKLE    30
#define   NI_R_FOOT     31
#define   NI_R_TOE      32

// Special Joints
#define   NI_L_FINGER   33      // Parameter for Left Finger
#define   NI_R_FINGER   34      // Parameter for Right Finger
#define   NI_FACE       35      // Parameter for Expression

#define   NI_AVATAR     36


//////////////////////////////////////////////////////////////////////////
// SDK独立な作業用ジョイント番号．
// 二つのSDK（OpenNI, Kinect SDK）のジョイント名はこの名前に集約される．

extern int  NI_SDK_PELVIS;
extern int  NI_SDK_TORSO;
extern int  NI_SDK_NECK;
extern int  NI_SDK_HEAD;

extern int  NI_SDK_L_EYE;
extern int  NI_SDK_R_EYE;
extern int  NI_SDK_L_BUST;
extern int  NI_SDK_R_BUST;
    
extern int  NI_SDK_L_SHLDR;
extern int  NI_SDK_L_ELBOW;
extern int  NI_SDK_L_WRIST;
extern int  NI_SDK_L_HAND;
    
extern int  NI_SDK_R_SHLDR;
extern int  NI_SDK_R_ELBOW;
extern int  NI_SDK_R_WRIST;
extern int  NI_SDK_R_HAND;

extern int  NI_SDK_L_HIP;
extern int  NI_SDK_L_KNEE;
extern int  NI_SDK_L_ANKLE;
extern int  NI_SDK_L_FOOT;
    
extern int  NI_SDK_R_HIP;
extern int  NI_SDK_R_KNEE;
extern int  NI_SDK_R_ANKLE;
extern int  NI_SDK_R_FOOT;

extern int  NI_SDK_L_FINGER;
extern int  NI_SDK_R_FINGER;
extern int  NI_SDK_FACE;

extern int  NI_SDK_AVATAR;

//
extern  jbxl::RBound<double>    RBound_NI_TORSO;
extern  jbxl::RBound<double>    RBound_NI_CHEST;
extern  jbxl::RBound<double>    RBound_NI_NECK;
extern  jbxl::RBound<double>    RBound_NI_HEAD;

extern  jbxl::RBound<double>    RBound_NI_L_COLLAR;
extern  jbxl::RBound<double>    RBound_NI_L_COLLAR;  
extern  jbxl::RBound<double>    RBound_NI_L_COLLAR;  
extern  jbxl::RBound<double>    RBound_NI_R_COLLAR;
extern  jbxl::RBound<double>    RBound_NI_L_SHLDR;
extern  jbxl::RBound<double>    RBound_NI_R_SHLDR;
extern  jbxl::RBound<double>    RBound_NI_L_ELBOW;
extern  jbxl::RBound<double>    RBound_NI_R_ELBOW;
extern  jbxl::RBound<double>    RBound_NI_L_WRIST;
extern  jbxl::RBound<double>    RBound_NI_R_WRIST;
extern  jbxl::RBound<double>    RBound_NI_L_HAND;
extern  jbxl::RBound<double>    RBound_NI_R_HAND;

extern  jbxl::RBound<double>    RBound_NI_L_HIP;
extern  jbxl::RBound<double>    RBound_NI_R_HIP;
extern  jbxl::RBound<double>    RBound_NI_L_KNEE;
extern  jbxl::RBound<double>    RBound_NI_R_KNEE;
extern  jbxl::RBound<double>    RBound_NI_L_ANKLE;
extern  jbxl::RBound<double>    RBound_NI_R_ANKLE;
extern  jbxl::RBound<double>    RBound_NI_L_FOOT;
extern  jbxl::RBound<double>    RBound_NI_R_FOOT;



namespace jbxwl {

using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////

typedef struct __NiJointData
{
    int               joint;        // Niジョイントの番号
    int               index;        // フレームの通し番号．フレームデータの添え字．NiFrameData[i].jdata[j].index==i

    Vector<double>     vect;        // ジョイントの座標
    Quaternion<double> quat;        // ジョイントの回転

    double             angl;        // 回転角（度）
    //
} NiJointData;


//
typedef struct __NiFrameData
{
    int          frmn;          // フレーム番号
    int          msec;          // フレーム時間
    int          jnum;          // Joint の数
    NiJointData* jdat;          // Joint データの配列（jnum個）
    //
} NiFrameData;



///////////////////////////////////////////////////////////////////////////////////////

// 共通のジョイントの名前と番号
std::string NiJointName(int n);
int         NiJointNum(char* name);

// SDK 固有のジョイント番号から共通のジョイント名を得る
std::string NiSDK2JointName(int joint, NiSDK_Lib lib);

// SDK 固有のジョイント番号のミラージョイントの番号を得る
int         NiSDKMirrorJointNum(int joint, NiSDK_Lib lib);

#define     NiMirrorJointNum(j)  NiSDKMirrorJointNum((j), NiSDK_None)

// 共通のジョイント名から Library 固有のジョイント番号を得る
int         JointName2NiSDK(char* jname, NiSDK_Lib lib);

// 共通ジョイント番号から，SDK 固有の座標用または回転用ジョイント番号を得る
int         Ni2SDKPosJointNum(int joint, NiSDK_Lib lib);
int         Ni2SDKRotJointNum(int joint, NiSDK_Lib lib);

#define     Ni2SDKJointNum(j, l)  Ni2SDKPosJointNum((j), (l))

// SDK 固有のジョイント番号を設定する
void        NiSetKinectJointNums(void);
void        NiSetOpenNIJointNums(void);
void        NiSetOpenNI2JointNums(void);

// Frame
//NiFrameData* makeFramesData(int frame, int  jnum);
//NiFrameData* makeVarFrameData(int frame, int* jnum, int* frtm=NULL);
NiFrameData* makeFramesData(int frame, int joint_num, int* frame_num);
void         freeFramesData(NiFrameData* frmdata, int frm_num);



///////////////////////////////////////////////////////////////////////////////////////////////////
// ジョイントの回転角の補正

void        NiInitRBoundJointsRotation(void);

void        NiCorrectJointsRotation(Quaternion<double>* quat, NiSDK_Lib lib);
void        NiCorrectJointEuler(int joint, Quaternion<double>* quat, RBound<double> rb);

void        NiGetJointEuler(int joint, Quaternion<double>* quat, Vector<double>* vect);
void        NiSetJointEuler(int joint, Vector<double>* vect, Quaternion<double>* quat);



//////////////////////////////////////////////////////////////////////////////////////////////////
//  共通ジョイントのクラス
//      ジョイントを抽象化するクラス

class  CNiJoints
{
public:
    CNiJoints(void)          { init();}
    CNiJoints(NiSDK_Lib lib) { init(); sdk_lib = lib;}
    virtual ~CNiJoints(void) {}

    void     clear(void)     { init();}

public:
    NiSDK_Lib   sdk_lib;

public:
    void    init(void);

    void    clearNiJointsData(void);
    void    connectJointsData(Vector<double>* vct, Quaternion<double>* qut, 
                              Vector<int>* crd, double* agl, CRingBuffer* vrg, CRingBuffer* qrg);
    void    copyJoints2NiJoints(BOOL mirror);

public:
    Vector<double>*     posVect;
    Quaternion<double>* rotQuat;
    Vector<int>*        crdVect;

    double*             jntAngl;

    CRingBuffer*        posRing;
    CRingBuffer*        rotRing;
    
    Vector<double>      niPosVect[NI_TOTAL_JOINT_NUM];
    Quaternion<double>  niRotQuat[NI_TOTAL_JOINT_NUM];
    double              niJntAngl[NI_TOTAL_JOINT_NUM];

//  BOOL            validPosVect[NI_TOTAL_JOINT_NUM];
//  BOOL            validRotQuat[NI_TOTAL_JOINT_NUM];
//  BOOL            validJntAngl[NI_TOTAL_JOINT_NUM];
};



////////////////////////////////////////////////////////////////////////////////////////
//  CBaseFrameTool Class
//      アニメーションのフレームデータを扱う基本クラス

class  CBaseFrameTool
{
public:
    CBaseFrameTool(void);
    virtual ~CBaseFrameTool(void);

public:
    virtual unsigned int    getFramesNumber(void) { return frames_num;}
    virtual int             getJointsNumber(void) { return joints_num;}

    virtual NiFrameData*    getFramesData(void)   { return framesData;}
    virtual NiJointData*    getJointsData(int frmnum, int fps) { return jointsData;}

    virtual int             getPlayBackTime(void) { return exec_time;}
    virtual int             getPlayBackFPS(void)  { return 30;}

    virtual BOOL            readFile (FILE* fp) { return FALSE;}
    virtual BOOL            writeFile(FILE* fp) { return FALSE;}

//
protected:
    void            init_data(void);
    void            free_data(void);
    void            clear_data(void);

protected:
    unsigned int    frames_num;
    int             joints_num;

    int             start_time;     // msec
    int             stop_time;      // msec
    int             exec_time;      // msec

protected:
    NiFrameData*    framesData;     // 全フレームデータ
    NiJointData*    jointsData;

    void            clearJointsData(int jnum);

protected:
    // Ni Joints
    void            clearVectorData(int jnum);
    
    Vector<double>*     posVect;
    Quaternion<double>* rotQuat;
};


}   // name space



/*
//
// OpenNI
//
    PELVIS                  ( 0)
    XN_SKEL_HEAD            ( 1)
    XN_SKEL_NECK            ( 2)
    XN_SKEL_TORSO           ( 3)
    XN_SKEL_WAIST           ( 4)

    XN_SKEL_LEFT_COLLAR     ( 5)
    XN_SKEL_LEFT_SHOULDER   ( 6)
    XN_SKEL_LEFT_ELBOW      ( 7)
    XN_SKEL_LEFT_WRIST      ( 8)
    XN_SKEL_LEFT_HAND       ( 9)
    XN_SKEL_LEFT_FINGERTIP  (10)

    XN_SKEL_RIGHT_COLLAR    (11)
    XN_SKEL_RIGHT_SHOULDER  (12)
    XN_SKEL_RIGHT_ELBOW     (13)
    XN_SKEL_RIGHT_WRIST     (14)
    XN_SKEL_RIGHT_HAND      (15)
    XN_SKEL_RIGHT_FINGERTIP (16)

    XN_SKEL_LEFT_HIP        (17)
    XN_SKEL_LEFT_KNEE       (18)
    XN_SKEL_LEFT_ANKLE      (19)
    XN_SKEL_LEFT_FOOT       (20)

    XN_SKEL_RIGHT_HIP       (21)
    XN_SKEL_RIGHT_KNEE      (22)
    XN_SKEL_RIGHT_ANKLE     (23)
    XN_SKEL_RIGHT_FOOT      (24)
*/


/*
//
// OpenNI2
//
    PELVIS                           ( 0)

    NITE_JOINT_HEAD              0+1 ( 1)
    NITE_JOINT_NECK              1+1 ( 2)

    NITE_JOINT_LEFT_SHOULDER     2+1 ( 3)
    NITE_JOINT_RIGHT_SHOULDER    3+1 ( 4)
    NITE_JOINT_LEFT_ELBOW        4+1 ( 5)
    NITE_JOINT_RIGHT_ELBOW       5+1 ( 6)
    NITE_JOINT_LEFT_HAND         6+1 ( 7)
    NITE_JOINT_RIGHT_HAND        7+1 ( 8)

    NITE_JOINT_TORSO             8+1 ( 9)

    NITE_JOINT_LEFT_HIP          9+1 (10)
    NITE_JOINT_RIGHT_HIP        10+1 (11)
    NITE_JOINT_LEFT_KNEE        11+1 (12)
    NITE_JOINT_RIGHT_KNEE       12+1 (13)
    NITE_JOINT_LEFT_FOOT        13+1 (14)
    NITE_JOINT_RIGHT_FOOT       14+1 (15)
*/


/*
//
// Kinect SDK
//
    NUI_SKELETON_POSITION_HIP_CENTER      ( 0)
    NUI_SKELETON_POSITION_SPINE           ( 1)
    NUI_SKELETON_POSITION_SHOULDER_CENTER ( 2)
    NUI_SKELETON_POSITION_HEAD            ( 3)

    NUI_SKELETON_POSITION_SHOULDER_LEFT   ( 4)
    NUI_SKELETON_POSITION_ELBOW_LEFT      ( 5)
    NUI_SKELETON_POSITION_WRIST_LEFT      ( 6)
    NUI_SKELETON_POSITION_HAND_LEFT       ( 7)

    NUI_SKELETON_POSITION_SHOULDER_RIGHT  ( 8)
    NUI_SKELETON_POSITION_ELBOW_RIGHT     ( 9)
    NUI_SKELETON_POSITION_WRIST_RIGHT     (10)
    NUI_SKELETON_POSITION_HAND_RIGHT      (11)

    NUI_SKELETON_POSITION_HIP_LEFT        (12)
    NUI_SKELETON_POSITION_KNEE_LEFT       (13)
    NUI_SKELETON_POSITION_ANKLE_LEFT      (14)
    NUI_SKELETON_POSITION_FOOT_LEFT       (15)

    NUI_SKELETON_POSITION_HIP_RIGHT       (16)
    NUI_SKELETON_POSITION_KNEE_RIGHT      (17)
    NUI_SKELETON_POSITION_ANKLE_RIGHT     (18)
    NUI_SKELETON_POSITION_FOOT_RIGHT      (19)
*/


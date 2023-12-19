#pragma once


#ifdef  ENABLE_KINECT_SDK


#include  <NuiApi.h>

#include  "ExClass.h"
#include  "Vector.h"

#include  "KinectFaceTracker.h"


// Lib
#pragma comment(lib, "Kinect10.lib")


/*
mode:
    NUI_INITIALIZE_FLAG_USES_COLOR                  カメライメージ
    NUI_INITIALIZE_FLAG_USES_DEPTH                  デプス
    NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX デプスとユーザーラベル
    NUI_INITIALIZE_FLAG_USES_SKELETON               スケルトン
*/


#define   KINECT_JOINT_NUM  NUI_SKELETON_POSITION_COUNT     //  20
#define   KINECT_USERS_NUM  7
//#define   KINECT_USERS_NUM    NUI_SKELETON_COUNT (6)


#define   KINECT_SKEL_PROFILE_ALL       2   // OpenNIに合わせる
#define   KINECT_SKEL_PROFILE_UPPER     3


//
namespace jbxwl {

//
// Image クラス
//
class  CKinectImage : public EventHandler
{
public:
    CKinectImage(void);
    virtual ~CKinectImage(void) { free();}

public:
    HANDLE  m_stream;
    uByte*  m_data;
    int     m_data_len;

    int     m_xsize;
    int     m_ysize;
    int     m_nfps;

public:
    void    free(void);
    void    make_data(void);
};


//
// Depth クラス
//
class  CKinectDepth : public EventHandler
{
public:
    CKinectDepth(void);
    virtual ~CKinectDepth(void) { free();}

public:
    HANDLE  m_stream;
    uByte*  m_data;
    int     m_data_len;

    BOOL    has_map;
    int*    m_xmap_i2d;
    int*    m_ymap_i2d;
    int*    m_xmap_d2i;
    int*    m_ymap_d2i;
    int     m_map_len;

    int     m_xsize;
    int     m_ysize;
    int     m_nfps;

    int     userLabel[KINECT_USERS_NUM];

public:
    void    free(void);
    void    make_data(void);
    void    make_map(void);
    void    delete_map(void);
    void    correct_map(int cx, int cy);

    int     get_user_index(int i, int j);
    uWord   get_depth(int i, int j);

    void    get_users(void);
    void    get_image_coord(int* i, int* j);

};


//
// Skeleton クラス
//
class  CKinectSkeleton : public EventHandler
{
public:
    CKinectSkeleton(void);
    virtual ~CKinectSkeleton(void) { free();}

public:
    HANDLE  m_stream;
    NUI_SKELETON_DATA m_data;

public:
    void    free(void);
};




///////////////////////////////////////////////////////////////////////////////
// Device クラス

class  CKinectDevice
{
public:
    CKinectDevice(void);
    virtual ~CKinectDevice(void);

    INuiSensor*         context;    
    CKinectImage*       image;
    CKinectDepth*       depth;
    CKinectSkeleton*    skeleton;
    CKinectFaceTracker* face;
    
    NUI_IMAGE_FRAME     imageFrame;
    NUI_IMAGE_FRAME     depthFrame;
    NUI_SKELETON_FRAME  skltnFrame;

    int     m_count;
    int     m_state;
    CString m_err_mesg;

    int     m_xsize;
    int     m_ysize;
    int     m_nfps;
    
    //
    NUI_TRANSFORM_SMOOTH_PARAMETERS* smoothParam;

    //
    Vector4 jointPosData[KINECT_JOINT_NUM];

public:
    BOOL    init(DWORD mode, BOOL use_image);
    void    free(void);
    void    clear_JointsData(void);

    BOOL    create_Context(DWORD mode, int index=0);

    BOOL    create_Image(void);
    BOOL    create_Depth(void);
    BOOL    create_Skeleton(int profile);
    BOOL    create_Face(void);

    void    delete_Context(void);
    void    delete_Image(void);
    void    delete_Depth(void);
    void    delete_Skeleton(void);
    void    delete_Face(void);

    BOOL    wait_Image(DWORD msec=INFINITE);
    BOOL    wait_Depth(DWORD msec=INFINITE);
    int     wait_Skeleton(int tuser, int duser, DWORD msec=INFINITE);   // return tracking user

    BOOL    start_Detection(int profile);
    BOOL    stop_Detection(void);

    int     get_TrackingUser(int duser);

};


}       // namespace


#endif

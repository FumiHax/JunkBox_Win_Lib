#pragma once

#ifdef  ENABLE_KINECT_SDK


#include  "KinectDevice.h"
#include  "KinectAudio.h"

#include  "NiToolWin.h"
#include  "Rotation.h"
#include  "RingBuffer.h"

#include  "NiSpeech.h"


//
namespace jbxwl {



class  CKinectWin
{
public:
    CKinectWin(void);
    virtual ~CKinectWin(void);

private:
    CKinectDevice*  device;
    CKinectAudio*   audio;
#ifdef ENABLE_NI_SPEECH
    CNiSpeech*      speech;
#endif

public:
    CString m_err_mesg;

    int     m_image_scale;
    int     m_depth_scale;
    int     m_skeleton_line;

    BOOL    m_is_detected;
    BOOL    m_is_tracking;
    BOOL    m_is_mirroring;

    BOOL    m_use_image;
    BOOL    m_use_led;
    BOOL    m_use_motor;
    BOOL    m_use_face;
    BOOL    m_use_speech;

    // 実際に使用できるかどうか．
    BOOL    m_enable_face;
    BOOL    m_enable_speech;
    BOOL    m_enable_motor;

    BOOL    m_use_knct_smth;

    int     m_profile;

    NUI_TRANSFORM_SMOOTH_PARAMETERS smoothParam;

    double  m_confidence;
    double  m_ground_level;

    int     tracking_user;
    int     tracking_deny;

public:
    ExCmnHead*  pViewData;
    ExCmnHead*  pDepthData;

    BOOL    hasDepthData;
    BOOL    isDetectShadow;
    BOOL    isDetectFace;

public:
    BOOL    init(void);
    void    free(void);
    void    deleteDevice(void);

    CString get_err_message(void);  // { return m_err_mesg;}

public:
    //
    void    clearJointsData(void);
    void    clearAvatarDetected(void);
    BOOL    checkAvatarDetected(void);

    BOOL    openUSBDevice(void)   { return TRUE;}
    void    closeUSBDevice(void)  {}
    void    setLEDColor(int col)  {}    // { if (m_use_led) set_LEDColor(col);}
    void    setTiltMotor(int ang);

    void    setMirroring(BOOL mirror);
    int     getDevState(void) { if (device==NULL) return NI_STATE_DETECT_STOPPED; else return device->m_state;}

    BOOL    startDetection(void);
    BOOL    stopDetection(void);
    BOOL    restartDetection(void);

    int     getTrackingUser(void)   { if (device!=NULL) return device->get_TrackingUser(tracking_deny); else return 0;}
    void    setDenyTrackingSearch(int user) { tracking_deny = user;}

    //
    Vector4 joint_PositionData(int joint);

    void    getJointsPosData(void);
    void    getJointsRotData(void) {}

    //
    void    makeDisplayImage(void); 
    void    makeDisplayDepth(CExView* pview);   
    BOOL    trackingJoints(void);

    //
    BOOL    detectShadow(void);
    void    paintShadow(void);
    void    drawSkeleton(int col, int line);
    void    drawJointConnection(int j1, int j2, int col, int line);
    void    set2DCoordinate(void);

    //
    BOOL    initRingBuffer(void);
    void    freeRingBuffer(void);
    void    clearRingBuffer(void);
    void    backup2RingBuffer(void);

    Vector4 jointPositionData(int joint);

public:
    // Face
    Quaternion<double>  getFaceRotation(void);

public:
#ifdef ENABLE_NI_SPEECH
    // Speech
    BOOL    initSpeech(void);
    BOOL    createSpeech(LPCTSTR lang, LPCTSTR grfile);
    BOOL    startSpeech(double confidence);
    void    stopSpeech(void);
    void    deleteSpeech(BOOL rls=TRUE);    // rls: メンバをリリースするか？

    void    setSpeechConfidence(double confd) { if (speech!=NULL) speech->setConfidence(confd);}

    virtual CNiSpeech* makeSpeech(void) { CNiSpeech* spch = new CNiSpeech(); return spch;}
#endif

public:
    virtual void saveJointsData(void) {}
    virtual void loggingJointsData(void) {}
    
    virtual void checkBoneLength(void) {}
    virtual void checkGroundLevel(void) {}

    virtual void convertJointsData(void) {}
    virtual void drawAddition(int col, int line) {}

    virtual void lostTrackingUser(int uid) {}
    virtual void detectTrackingUser(int uid) {}

    virtual void callback_status_changed(BOOL success, const OLECHAR* instanceName, const OLECHAR* deviceName);

protected:
    void    setDevState(int state) { if (device!=NULL) device->m_state = state;}

public:
    // Data
    Vector<double>     startPos;
    Vector<double>     currentPos;

    Vector<double>     posVect[KINECT_JOINT_NUM];
    Quaternion<double> rotQuat[KINECT_JOINT_NUM];
    Vector<int>        crdVect[KINECT_JOINT_NUM];   // for 2D Image

    double             jntAngl[KINECT_JOINT_NUM];   // angle between joints.

    CRingBuffer        posRing[KINECT_JOINT_NUM];
    CRingBuffer        rotRing[KINECT_JOINT_NUM];

// device Iterface
public:
    BOOL    hasContext(void)  { if (device!=NULL && device->context!=NULL) return TRUE; else return FALSE;}
    BOOL    hasImageGen(void) { if (device!=NULL && device->image!=NULL)   return TRUE; else return FALSE;}
    BOOL    hasDepthGen(void) { if (device!=NULL && device->depth!=NULL)   return TRUE; else return FALSE;}

    BOOL    createImage(void) { if (device!=NULL) return device->create_Image(); else return FALSE;}
    BOOL    createDepth(void) { if (device!=NULL) return device->create_Depth(); else return FALSE;}
    void    deleteImage(void) { if (device!=NULL) device->delete_Image();}
    void    deleteDepth(void) { if (device!=NULL) device->delete_Depth();}


    BOOL    waitImage(void)   { if (device!=NULL) return device->wait_Image(); else return FALSE;}
    BOOL    waitDepth(void)   { if (device!=NULL) return device->wait_Depth(); else return FALSE;}

    //
    BOOL    faceDetect(FT_VECTOR3D* hint) 
    { 
        if (device!=NULL && device->face!=NULL) return device->face->detect(hint);
        else return FALSE;
    }
    
    void  drawFaceRect(ExCmnHead* viewdata, int scale, BOOL mirror, int col, int line)
    {
        if (device!=NULL && device->face!=NULL) {
            device->face->drawFaceRect(pViewData, m_image_scale, m_is_mirroring, col, line);
        }
    }

    //
    int     getXSize(void) { if (device!=NULL) return device->m_xsize; else return 0;}
    int     getYSize(void) { if (device!=NULL) return device->m_ysize; else return 0;}
    int     getFPS(void)   { if (device!=NULL) return device->m_nfps;  else return 0;}
};




///////////////////////////////////////////////////////////////////////////////
// CallBack

void  CALLBACK  KinectStatusProc(HRESULT hr, const OLECHAR* instanceName, const OLECHAR* deviceName, void* pUserData);


}       // namespace


#endif

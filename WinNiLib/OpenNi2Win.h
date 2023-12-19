#pragma once

#ifdef  ENABLE_OPENNI

#include  "OpenNiTool.h"
#include  "NiToolWin.h"
//#include  "WinAudioTool.h"
#include  "OpenNiAudio.h"

#include  "ExView.h"
#include  "Rotation.h"
#include  "RingBuffer.h"

#include  "NiSpeech.h"

// Lib
#ifdef  WIN64
    #pragma comment(lib, "XnVNITE64_1_5_2.lib")
    #pragma comment(lib, "OpenNI64.lib")
#else
    #pragma comment(lib, "XnVNITE_1_5_2.lib")
    #pragma comment(lib, "OpenNI.lib")
#endif


//
namespace jbxwl {


class  COpenNiWin : public COpenNiTool
{
public:
    COpenNiWin(void);
    virtual ~COpenNiWin(void) { free();}

    COpenNiAudio*   audio;
    CNiSpeech*      speech;

    CString m_err_mesg;

    int     m_image_scale;
    int     m_depth_scale;
    int     m_skeleton_line;

    BOOL    m_is_tracking;
    BOOL    m_is_detected;
    BOOL    m_is_mirroring;

    BOOL    m_use_image;
    BOOL    m_use_led;
    BOOL    m_use_motor;
    BOOL    m_use_face;
    BOOL    m_use_speech;

    BOOL    m_enable_face;
    BOOL    m_enable_speech;
    BOOL    m_enable_motor;

    BOOL    m_use_nite_smth;
    BOOL    m_force_pose;

    int     m_profile;
    float   m_confidence;
    float   m_nite_smooth;
    double  m_ground_level;

public:
    ExCmnHead*  pViewData;
    ExCmnHead*  pDepthData;

    BOOL    hasDepthData;
    BOOL    hasUserData;
    BOOL    isDetectShadow;
    BOOL    isDetectFace;

public:
    BOOL    init(void);
    void    free(void);

    CString get_err_message(void);

public:
    //
    void    deleteDevice(void) { delete_Device();}
    void    clearJointsData(void);
    void    clearAvatarDetected(void);
    BOOL    checkAvatarDetected(void);


    BOOL    openUSBDevice(void)   { return open_USB_Device();}
    void    closeUSBDevice(void)  { close_USB_Device();}
    void    setLEDColor(int col)  { if (m_use_led) set_LED_Color(col);}
    void    setTiltMotor(int ang);

    void    setMirroring(BOOL mirror);
    int     getDevState(void) { return m_state;}

    BOOL    startDetection(BOOL force_pose);
    BOOL    stopDetection(void);
    BOOL    restartDetection(void);

    int     getTrackingUser(void)   { return get_TrackingUser();}
    void    setDenyTrackingSearch(XnUserID user) { set_DenyTrackingSearch(user);}

    //
    void    getJointsPosData(XnUserID uid);
    void    getJointsRotData(XnUserID uid);

    //
    void    makeDisplayImage(void);
    void    makeDisplayDepth(CExView* pview);
    BOOL    trackingJoints(BOOL use_rot_data);

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

public:
    // Face
//  Quaternion  getFaceRotation(void) {};

public:
    // Speech　  作成中
    BOOL    initSpeech(void);
    BOOL    createSpeech(LPCTSTR lang, LPCTSTR grfile);
    BOOL    startSpeech(float confidence);
    void    stopSpeech(void);
    void    deleteSpeech(BOOL rls=TRUE);    // rls: メンバをリリースするか？

    void    setSpeechConfidence(float confd) { if (speech!=NULL) speech->setConfidence(confd);}

    virtual CNiSpeech* makeSpeech(void) { CNiSpeech* spch = new CNiSpeech(); return spch;}

public:
    virtual void saveJointsData(void) {}
    virtual void loggingJointsData(void) {}

    virtual void checkBoneLength(void) {}
    virtual void checkGroundLevel(void) {}

    virtual void convertJointsData(void) {}
    virtual void drawAddition(int col, int line) {}

    virtual void lostTrackingUser(int uid) {}
    virtual void detectTrackingUser(int uid) {}

protected:
    void    setDevState(int state) { m_state = state;}

public:
    // Data
    Vector<double>  startPos;
    Vector<double>  currentPos;

    Vector<double>  posVect[OPENNI_JOINT_NUM];
    Quaternion      rotQuat[OPENNI_JOINT_NUM];
    Vector<int>     crdVect[OPENNI_JOINT_NUM];  // for 2D Image

    double          jntAngl[OPENNI_JOINT_NUM];  // angle between joints.

    CRingBuffer     posRing[OPENNI_JOINT_NUM];
    CRingBuffer     rotRing[OPENNI_JOINT_NUM];
};


}       // namespace


#endif

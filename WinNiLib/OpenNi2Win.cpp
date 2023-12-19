
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif


#include  "..\stdafx.h"

#include  "OpenNiWin.h"
#include  "NiJointsTool.h"

#include  "Graph.h"
#include  "WinTools.h"


using namespace jbxl;


#ifdef  ENABLE_OPENNI

using namespace jbxwl;


COpenNiWin::COpenNiWin(void)
{
    m_err_mesg      = _T("");

    audio           = NULL;
    speech          = NULL;

    m_image_scale   = 2;
    m_depth_scale   = 2;
    m_skeleton_line = 2;

    m_is_tracking   = FALSE;
    m_is_detected   = FALSE;
    m_is_mirroring  = TRUE;
    //
    m_use_image     = TRUE;
    m_use_led       = FALSE;
    m_use_motor     = FALSE;
    m_use_face      = FALSE;
    m_use_speech    = FALSE;

    // 実際に使用できるかどうか
    m_enable_face   = FALSE;
    m_enable_speech = FALSE;
    m_enable_motor  = TRUE;

    m_use_nite_smth = TRUE;
    m_nite_smooth   = 0.0;
    m_force_pose    = FALSE;

    m_confidence    = 0.75;
    m_profile       = XN_SKEL_PROFILE_ALL;
    m_ground_level  = NI_DEFAULT_GROUND_LEVEL;

    pViewData       = NULL;
    pDepthData      = NULL;

    hasDepthData    = FALSE;
    hasUserData     = FALSE;
    isDetectShadow  = FALSE;
    isDetectFace    = FALSE;

    clearAvatarDetected();
}



BOOL  COpenNiWin::init(void)
{
    BOOL ret = COpenNiTool::init(m_use_image);

    return ret;
}



void  COpenNiWin::free(void)
{
    freeRingBuffer();
    COpenNiTool::free();

    return;
}



CString  COpenNiWin::get_err_message(void)
{
    CString mesg = mbs2ts((char*)COpenNiTool::m_err_mesg.buf);
    
    if (mesg.IsEmpty()) {
        mesg = m_err_mesg;
        m_err_mesg = _T("");
    }

    return mesg;
}



void  COpenNiWin::clearJointsData(void)
{
    for (int i=0; i<OPENNI_JOINT_NUM; i++) {
        rotQuat[i].init(-1.0);
        posVect[i].init(-1.0);
        jntAngl[i] = 0.0;
    }
}



BOOL  COpenNiWin::initRingBuffer(void)
{
    BOOL ret = TRUE;

    for (int j=0; j<OPENNI_JOINT_NUM; j++) {
        posRing[j].init(NI_RING_BUFFER_SIZE, sizeof(Vector<double>));
        rotRing[j].init(NI_RING_BUFFER_SIZE, sizeof(Quaternion));
        if (posRing[j].state<0 || rotRing[j].state<0) {
            freeRingBuffer();
            m_err_mesg = _T("COpenNiWin::initRingBuffer(): WARNING: Out of Memory.");
            ret = FALSE;
            break;
        }
    }

    return ret;
}



void  COpenNiWin::freeRingBuffer(void)
{
    for (int j=0; j<OPENNI_JOINT_NUM; j++) {
        if (posRing[j].enable) posRing[j].free();
        if (rotRing[j].enable) rotRing[j].free();
    }
}



void  COpenNiWin::clearRingBuffer(void)
{
    for (int j=0; j<OPENNI_JOINT_NUM; j++) {
        if (posRing[j].enable) posRing[j].clear();
        if (rotRing[j].enable) rotRing[j].clear();
    }
}



void  COpenNiWin::backup2RingBuffer(void)
{
    for (int j=0; j<OPENNI_JOINT_NUM; j++) {
        if (posVect[j].c>=m_confidence) posRing[j].put(&posVect[j]);
        if (rotQuat[j].c>=m_confidence) rotRing[j].put(&rotQuat[j]);
        //posRing[j].put(&posVect[j]);
        //rotRing[j].put(&rotQuat[j]);
    }
}




/////////////////////////////////////////////////////////////////////////////////////////////////
//

void  COpenNiWin::clearAvatarDetected(void)
{
    clearJointsData();
    clearRingBuffer();

    startPos   = Vector<double>(0.0, 0.0, 0.0);
    currentPos = Vector<double>(0.0, 0.0, 0.0);
    m_is_detected  = FALSE;
    m_ground_level = NI_DEFAULT_GROUND_LEVEL;

    return;
}


//
BOOL  COpenNiWin::checkAvatarDetected(void)
{
    if (posVect[NI_SDK_PELVIS].c>=m_confidence && crdVect[NI_SDK_PELVIS].c>0.0) {
        startPos = posVect[NI_SDK_PELVIS];
        m_is_detected = TRUE;

        // m_ground_level is not used now
        if (posVect[NI_SDK_R_ANKLE].c>=m_confidence && crdVect[NI_SDK_R_ANKLE].c>0.0) {
            m_ground_level = posVect[NI_SDK_R_ANKLE].z - startPos.z;
            if (posVect[NI_SDK_L_ANKLE].c>=m_confidence && crdVect[NI_SDK_L_ANKLE].c>0.0) {
                m_ground_level = Min(m_ground_level, posVect[NI_SDK_L_ANKLE].z - startPos.z);
            }
        }
        else if (posVect[NI_SDK_L_ANKLE].c>=m_confidence && crdVect[NI_SDK_L_ANKLE].c>0.0) {
            m_ground_level = posVect[NI_SDK_L_ANKLE].z - startPos.z;
        }
    }

    return m_is_detected;
}



void  COpenNiWin::setMirroring(BOOL mirror)
{
    if (m_is_mirroring!=mirror) {
        if (device!=NULL && device->context!=NULL) {
            device->context->SetGlobalMirror(mirror);
        }
    }
    m_is_mirroring = mirror;
}


//
void  COpenNiWin::setTiltMotor(int ang) 
{ 
    if (m_use_motor && m_enable_motor) {
        m_enable_motor = FALSE;
        set_Tilt_Motor(ang);    // Sleep(1000)
        m_enable_motor = TRUE;
    }
}   


//
BOOL  COpenNiWin::startDetection(BOOL force_pose)
{
    clearJointsData();

    float smooth = 0.0;
    if (m_use_nite_smth) smooth = m_nite_smooth;
    m_force_pose = force_pose;

    BOOL ret = start_Detection(force_pose, m_profile, smooth);
    if (ret) {
        setLEDColor(NI_LED_BLINK_ORANGE);
    }
    else {
        m_err_mesg = _T("COpenNiWin::startDetection(): ERROR: Maybe NITE is not installed.");
    }

    return ret;
}



BOOL  COpenNiWin::stopDetection(void)
{
    BOOL ret = stop_Detection();
    setLEDColor(NI_LED_GREEN);

    return ret;
}



BOOL  COpenNiWin::restartDetection(void)
{
    BOOL ret = stopDetection();
    if (ret) ret = startDetection(m_force_pose);

    return ret;
}


//
// color is GRAPH_COLOR_BGRA
//
void  COpenNiWin::makeDisplayImage()
{
    int      index;
    uByte*   src = NULL;
    uByte*   ptr;

    if (pViewData==NULL) return;

    if (device->imageMD!=NULL && m_use_image) {
        src = (uByte*)device->imageMD->RGB24Data();
    }

    //
    if (src!=NULL) {
        for (int j=0; j<pViewData->ysize; j++) {
            int ls = j*m_image_scale*device->m_xsize;   //outputMode.nXRes;

            for(int i=0; i<pViewData->xsize; i++) {
                int li = i*m_image_scale;
                ptr    = &(pViewData->point(i, j));
                index  = (ls + li)*3;
                ptr[0] = src[index+2];      // B
                ptr[1] = src[index+1];      // G
                ptr[2] = src[index];        // R
                ptr[3] = 0;                 // A
            }
        }
    }

    // No Camera Image
    else {
        for(int j=0; j<pViewData->ysize; j++) {
            int ls = j*m_image_scale*device->m_xsize;   //outputMode.nXRes;

            for(int i=0; i<pViewData->xsize; i++) {
                int li = i*m_image_scale;
                ptr = &(pViewData->point(i, j));
                ptr[0] = ptr[1] = ptr[2] = 224;     // Gray Background
                ptr[3] = 0;
            }
        }
    }
}


//
// color is GRAPH_COLOR_MONO16
// 表示データは 8bit
//
void  COpenNiWin::makeDisplayDepth(CExView* pview)
{
    if (pview==NULL) return;
    pDepthData = &(pview->viewData);

    const XnDepthPixel* src = NULL;
    if (device->depth!=NULL && hasDepthData) {
        src = device->depthMD->Data();
    }
    if (src==NULL) return;

    //
    uWord  max = pview->cMax;
    uWord  min = pview->cMin;
    //
    uWord* wrk = (uWord*)malloc(sizeof(uWord)*pDepthData->ysize*pDepthData->xsize);

    //
    for (int j=0; j<pDepthData->ysize; j++) {
        int jj = j*pDepthData->xsize;
        int ls = j*m_depth_scale*device->m_xsize;

        for (int i=0; i<pDepthData->xsize; i++) {
            // Mirroring
            int li = i*m_depth_scale;
            if (!m_is_mirroring) li = device->m_xsize - li - 1; 
            //
            int k = jj + i;
            wrk[k] = (uWord)src[ls+li];
            if      (wrk[k]>max) wrk[k] = max;
            else if (wrk[k]<min) wrk[k] = min;
        }
    }

    //
    float  dif = (float)(max - min);
    uByte* ptr = (uByte*)pDepthData->grptr;

    if (dif!=0.0) {
        for (int i=0; i<pDepthData->ysize*pDepthData->xsize; i++) {
            ptr[i] = (uByte)((max - wrk[i])/dif*255);
        }
    }
    else {
        for (int i=0; i<pDepthData->ysize*pDepthData->xsize; i++) {         
            ptr[i] = (uByte)(255 - wrk[i]/256);
        }
    }
    ::free(wrk);

    return;
}




//////////////////////////////////////////////////////////////////////////////////
// Tracking Main
//
BOOL  COpenNiWin::trackingJoints(BOOL use_rot_data)
{
    BOOL ret = FALSE;
    
    if (hasUserData) {
        // Lost Avatar の反応が鈍いので isDetectShadow を使う   
        isDetectShadow = detectShadow();
        
        //
        if ((int)tracking_user>0) {
            XnBool istracking = skeleton->IsTracking(tracking_user);
            if (!istracking || !isDetectShadow) {
                clearAvatarDetected();
                lostTrackingUser((int)tracking_user);   // virtual
                if (istracking) skeleton->StopTracking(tracking_user);
                setDenyTrackingSearch(tracking_user);
                tracking_user = (XnUserID)0;
            }
        }

        if ((int)tracking_user==0) {
            m_is_detected = FALSE;
            tracking_user = getTrackingUser();
            if ((int)tracking_user>0 && (int)tracking_user<=OPENNI_USERS_NUM) {
                clearJointsData();
                detectTrackingUser((int)tracking_user); // virtual
            }
        }

        /////////////////////////////////////
        // Tracking
        //
        if ((int)tracking_user>0 && (int)tracking_user<=OPENNI_USERS_NUM) {
            //
            getJointsPosData(tracking_user);
            if (use_rot_data) getJointsRotData(tracking_user);
            //
            if (m_is_detected) {
                convertJointsData();    // virtual
                //
                backup2RingBuffer();
                saveJointsData();       // virtual
                loggingJointsData();    // virtual
                //
                paintShadow();
                drawSkeleton(NiGetSkeletonColor((int)tracking_user), m_skeleton_line);
                drawAddition(NiGetSkeletonColor((int)tracking_user), m_skeleton_line);  // virtual
            }
            ret = TRUE;
        }
    }

    else {
        tracking_user = (XnUserID)0;
        m_is_detected = FALSE;
    }

    //
    if (ret!=m_is_tracking) {
        m_is_tracking = ret;
        if (ret) setLEDColor(NI_LED_RED);
        else     setLEDColor(NI_LED_BLINK_ORANGE);
    }

    return ret;
}


//
// Calculate posVect, crdVect
//
void  COpenNiWin::getJointsPosData(XnUserID uid)
{
    get_JointsPositionData(uid);

    //
    for (int j=1; j<OPENNI_JOINT_NUM; j++) {
        XnVector3D pos = jointPosData[j];
        posVect[j].set(-pos.Z/1000., pos.X/1000., pos.Y/1000., 0.0, jointPosConfidence[j]);
    }

    // Tracking Mode
    if (m_profile==XN_SKEL_PROFILE_ALL) {
        posVect[NI_SDK_PELVIS] = (posVect[NI_SDK_R_HIP] + posVect[NI_SDK_L_HIP])*0.5;
        posVect[NI_SDK_PELVIS].c = Min(jointPosConfidence[NI_SDK_R_HIP], jointPosConfidence[NI_SDK_L_HIP]);
    }
    else if (m_profile==XN_SKEL_PROFILE_UPPER) {
        posVect[NI_SDK_PELVIS] = 2.0*posVect[NI_SDK_TORSO] - posVect[NI_SDK_NECK];
        posVect[NI_SDK_PELVIS].c = Min(jointPosConfidence[NI_SDK_TORSO], jointPosConfidence[NI_SDK_NECK]);
        jointPosConfidence[NI_SDK_PELVIS] = (XnConfidence)posVect[NI_SDK_PELVIS].c;
    }

    //
    set2DCoordinate();
    checkBoneLength();
    if (!m_is_detected) checkAvatarDetected();

    //
    currentPos = posVect[NI_SDK_PELVIS];
    for (int j=0; j<OPENNI_JOINT_NUM; j++) {
        posVect[j] = posVect[j] - startPos;
        if (posVect[j].c<m_confidence || crdVect[j].c<0.0) posVect[j].c = -1.0;
    }

    checkGroundLevel();
}


//
// Get rotQuat
//
void  COpenNiWin::getJointsRotData(XnUserID uid)
{
    get_JointsRotationData(uid);

    //
    for (int j=1; j<OPENNI_JOINT_NUM; j++) {
        XnMatrix3X3 rot = jointRotData[j];

        double m11 = rot.elements[0];
        double m12 = rot.elements[1];
        double m13 = rot.elements[2];
        double m21 = rot.elements[3];
        double m31 = rot.elements[6];
        double m32 = rot.elements[7];
        double m33 = rot.elements[8];

        Vector<double> eul = RotMatrixElements2EulerXYZ(m11, m12, m13, m21, m31, m32, m33);
        Vector<double> vct(-eul.x, -eul.y, eul.z);  // Mirror: vct(-eul.x, eul.y, -eul.z);
        rotQuat[j].setEulerYZX(vct);
        rotQuat[j].c = jointRotConfidence[j];

        if (rotQuat[j].c<m_confidence || crdVect[j].c<0.0) rotQuat[j].c = -1.0;
    }
}




/////////////////////////////////////////////////////////////////////////////////////
//

BOOL  COpenNiWin::detectShadow(void)
{
    XnLabel  label = 0;
    XnLabel* lbl = NULL;

    if (pViewData==NULL) return FALSE;;

    if (getDevState()==NI_STATE_DETECT_EXEC && hasUserData) {
        lbl = (XnLabel*)device->sceneMD->Data();
    }
    if (lbl==NULL) return FALSE;

    for(int j=0; j<pViewData->ysize; j++) {
        int ls = j*m_image_scale*device->m_xsize;   //outputMode.nXRes;

        for(int i=0; i<pViewData->xsize; i++) {
            int li = i*m_image_scale;
            //
            label = lbl[ls+li];
            if (label>0) {
                if ((int)label==(int)tracking_user) return TRUE;
            }
        }
    }

    return FALSE;
}



void  COpenNiWin::paintShadow(void)
{
    XnLabel  label = 0;
    XnLabel* lbl = NULL;
    uByte*   ptr;

    if (pViewData==NULL) return;

    if (getDevState()==NI_STATE_DETECT_EXEC && hasUserData) {
        lbl = (XnLabel*)device->sceneMD->Data();
    }
    if (lbl==NULL) return;

    //
    for(int j=0; j<pViewData->ysize; j++) {
        int ls = j*m_image_scale*device->m_xsize;   // outputMode.nXRes;

        for(int i=0; i<pViewData->xsize; i++) {
            int li = i*m_image_scale;
            //
            label = lbl[ls+li];
            if (label>0) {
                ptr = &(pViewData->point(i, j));
                NiSetUserColor(label, ptr, m_use_image);
            }
        }
    }
}



void  COpenNiWin::drawSkeleton(int col, int line)
{
    drawJointConnection(NI_SDK_HEAD,    NI_SDK_NECK,    col, line);
    drawJointConnection(NI_SDK_NECK,    NI_SDK_TORSO,   col, line);
    drawJointConnection(NI_SDK_TORSO,   NI_SDK_PELVIS,  col, line);

    drawJointConnection(NI_SDK_NECK,    NI_SDK_L_SHLDR, col, line);
    drawJointConnection(NI_SDK_NECK,    NI_SDK_R_SHLDR, col, line);

    drawJointConnection(NI_SDK_L_SHLDR, NI_SDK_L_ELBOW, col, line);
    drawJointConnection(NI_SDK_R_SHLDR, NI_SDK_R_ELBOW, col, line);
    drawJointConnection(NI_SDK_L_ELBOW, NI_SDK_L_WRIST, col, line);
    drawJointConnection(NI_SDK_R_ELBOW, NI_SDK_R_WRIST, col, line);

    drawJointConnection(NI_SDK_PELVIS,  NI_SDK_L_HIP,   col, line);
    drawJointConnection(NI_SDK_PELVIS,  NI_SDK_R_HIP,   col, line);
    drawJointConnection(NI_SDK_L_HIP,   NI_SDK_L_KNEE,  col, line);
    drawJointConnection(NI_SDK_R_HIP,   NI_SDK_R_KNEE,  col, line);
    drawJointConnection(NI_SDK_L_KNEE,  NI_SDK_L_ANKLE, col, line);
    drawJointConnection(NI_SDK_R_KNEE,  NI_SDK_R_ANKLE, col, line);
}



void  COpenNiWin::drawJointConnection(int j1, int j2, int col, int line)
{
    if (pViewData==NULL) return;

    MSGraph<unsigned int> vp;
    vp.xs = pViewData->xsize;
    vp.ys = pViewData->ysize;
    vp.zs = 1;
    vp.gp = (unsigned int*)pViewData->grptr;

    if (crdVect[j1].c>0.0 && crdVect[j2].c>0.0) {
        MSGraph_Line(vp, crdVect[j1].x, crdVect[j1].y, crdVect[j2].x, crdVect[j2].y, col);
        for (int i=1; i<line; i++) {
            MSGraph_Line(vp, crdVect[j1].x+i, crdVect[j1].y, crdVect[j2].x+i, crdVect[j2].y, col);
            MSGraph_Line(vp, crdVect[j1].x-i, crdVect[j1].y, crdVect[j2].x-i, crdVect[j2].y, col);
            MSGraph_Line(vp, crdVect[j1].x, crdVect[j1].y+i, crdVect[j2].x, crdVect[j2].y+i, col);
            MSGraph_Line(vp, crdVect[j1].x, crdVect[j1].y-i, crdVect[j2].x, crdVect[j2].y-i, col);
        }
    }
}



void  COpenNiWin::set2DCoordinate(void)
{
    for (int j=0; j<OPENNI_JOINT_NUM; j++) {
        crdVect[j].init(-1.0);
    }

    //
    if (device->depth!=NULL && pViewData!=NULL) {
        //
        for (int j=0; j<OPENNI_JOINT_NUM; j++) {
            float cnfd;

            if (j==NI_SDK_PELVIS) {
                // Tracking Mode
                if (m_profile==XN_SKEL_PROFILE_UPPER) {
                    cnfd = (float)Min(jointPosConfidence[NI_SDK_TORSO], jointPosConfidence[NI_SDK_NECK]);
                }
                else {
                    cnfd = (float)Min(jointPosConfidence[NI_SDK_R_HIP], jointPosConfidence[NI_SDK_L_HIP]);
                }
            }
            else {
                cnfd = (float)jointPosConfidence[j];
            }

            //
            if (cnfd>=m_confidence) {
                XnVector3D pos;

                if (j==NI_SDK_PELVIS) {
                    if (m_profile==XN_SKEL_PROFILE_UPPER) {
                        pos.X = (XnFloat)(2.0*jointPosData[NI_SDK_TORSO].X - jointPosData[NI_SDK_NECK].X);
                        pos.Y = (XnFloat)(2.0*jointPosData[NI_SDK_TORSO].Y - jointPosData[NI_SDK_NECK].Y);
                        pos.Z = (XnFloat)(2.0*jointPosData[NI_SDK_TORSO].Z - jointPosData[NI_SDK_NECK].Z);
                    }
                    else {
                        pos.X = (XnFloat)((jointPosData[NI_SDK_R_HIP].X + jointPosData[NI_SDK_L_HIP].X)*0.5);
                        pos.Y = (XnFloat)((jointPosData[NI_SDK_R_HIP].Y + jointPosData[NI_SDK_L_HIP].Y)*0.5);
                        pos.Z = (XnFloat)((jointPosData[NI_SDK_R_HIP].Z + jointPosData[NI_SDK_L_HIP].Z)*0.5);
                    }
                }
                else {
                    pos = jointPosData[j];
                }

                XnPoint3D pt[1] = {pos};
                device->depth->ConvertRealWorldToProjective(1, pt, pt);
                int xs = (int)(pt[0].X/m_image_scale);
                int ys = (int)(pt[0].Y/m_image_scale);

                crdVect[j].x = xs;
                crdVect[j].y = ys;
                if (xs>0 && xs<pViewData->xsize && ys>0 && ys<pViewData->ysize) {   // 縁(0) は除外
                    crdVect[j].c = 1.0;
                }
            }
        }
    }

    return;
}




/////////////////////////////////////////////////////////////////////////////////////
// Speech Platform
//      作成中

BOOL  COpenNiWin::initSpeech(void)
{
    m_err_mesg = _T("");

    if (audio==NULL) audio = new COpenNiAudio(NULL);

    BOOL ret = audio->init(NULL);
    if (!ret) m_err_mesg = _T("COpenNiWin::initSpeech(): ERROR: Audio Initialization Error!!");
    
    //
    if (ret) {
        speech = makeSpeech();
        WAVEFORMATEX format = audio->getAudioFormat();
        IStream*     stream = audio->getIStream();
        ret = speech->init(stream, &format);
        if (!ret) m_err_mesg = _T("COpenNiWin::initSpeech(): ERROR: Speech Initialization Error!!");
    }

    //
    if (!ret) {
        if (speech!=NULL) {
            speech->free();
            deleteNull(speech);
        }
        deleteNull(audio);
        return FALSE;
    }
    
    return TRUE;
}



BOOL  COpenNiWin::createSpeech(LPCTSTR lang, LPCTSTR grfile)
{
    m_err_mesg = _T("");

    BOOL ret = TRUE;
    if (speech==NULL) ret = initSpeech(); 

    if (ret) {
        ret = speech->create(lang);
        if (!ret) m_err_mesg = _T("COpenNiWin::createSpeech(): ERROR: Speech Context Creation Error!! Perhaps, Language Pack is not installed.");
    }
    //
    if (ret) {
        ret = speech->load(grfile);
        if (!ret) {
            m_err_mesg  = _T("COpenNiWin::createSpeech(): ERROR: Grammar File Loading Error!!  File = ");
            m_err_mesg += grfile;
        }
    }
    
    //
    if (!ret) return FALSE;
    return TRUE;
}



BOOL  COpenNiWin::startSpeech(float confidence)
{
    m_err_mesg = _T("");

    if (audio==NULL || speech==NULL) {
        m_err_mesg = _T("COpenNiWin::startSpeech(): ERROR: Audio or Speech Instance is/are NULL!!");
        return FALSE;
    }

    BOOL ret = audio->startCapture();
    if (!ret) m_err_mesg = _T("COpenNiWin::startSpeech(): ERROR: Audio Capture Starting Error!!");
    //
    if (ret) {
        ret = speech->start(confidence);
        if (!ret) {
            audio->stopCapture();
            m_err_mesg = _T("COpenNiWin::startSpeech(): ERROR: Speech Platform Starting Error!!");
        }
    }
    //
    if (!ret) {
        if (m_err_mesg==_T("")) m_err_mesg = _T("COpenNiWin::startSpeech(): ERROR: Unknown Error!!");
        return FALSE;
    }
    
    return TRUE;
}



void  COpenNiWin::stopSpeech(void)
{
    if (speech!=NULL) {
        speech->stop();
    }
    //
    if (audio!=NULL) {
        audio->stopCapture();
    }

    return;
}



void  COpenNiWin::deleteSpeech(BOOL rls)
{
    DEBUG_INFO("COpenNiWin::deleteSpeech(): START");

    if (speech!=NULL) {
        stopSpeech();
        //
        ////////////////////////////////////////////////////////////////
        if (rls) speech->free();
        delete speech;
        speech = NULL;
    }

    //
    deleteNull(audio);

    DEBUG_INFO("COpenNiWin::deleteSpeech(): END");
    return;
}


#endif      // ifdef ENABLE_OPENNI

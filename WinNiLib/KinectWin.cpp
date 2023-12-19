
#include  "WinBaseLib.h"
#include  "KinectWin.h"
#include  "NiJointsTool.h"

#include  "Graph.h"

using namespace jbxl;


#ifdef  ENABLE_KINECT_SDK

using namespace jbxwl;


CKinectWin::CKinectWin(void)
{
    device          = NULL;
    audio           = NULL;

#ifdef ENABLE_NI_SPEECH
    speech          = NULL;
#endif

    m_err_mesg      = _T("");

    m_image_scale   = 2;
    m_depth_scale   = 2;
    m_skeleton_line = 2;

    m_is_detected   = FALSE;
    m_is_tracking   = FALSE;
    m_is_mirroring  = TRUE;
    //
    m_use_image     = TRUE;
    m_use_led       = FALSE;
    m_use_motor     = FALSE;
    m_use_face      = FALSE;
    m_use_speech    = TRUE;

    //
    m_enable_face   = FALSE;
    m_enable_speech = FALSE;
    m_enable_motor  = TRUE;
    
    m_use_knct_smth = TRUE;
    m_confidence    = 0.5;          // 現時点では 0.0より大きければ何でも良い．
    m_profile       = KINECT_SKEL_PROFILE_ALL;
    m_ground_level  = NI_DEFAULT_GROUND_LEVEL;

    pViewData       = NULL;
    pDepthData      = NULL;

    hasDepthData    = FALSE;
    isDetectShadow  = FALSE;
    isDetectFace    = FALSE;

    tracking_user   = 0;
    tracking_deny   = 0;

    //
    smoothParam.fSmoothing  = 0.5f;
    smoothParam.fCorrection = 0.5f;
    smoothParam.fPrediction = 0.5f;
    smoothParam.fJitterRadius = 0.05f;
    smoothParam.fMaxDeviationRadius = 0.04f;

    clearAvatarDetected();
}



CKinectWin::~CKinectWin(void)
{
    DEBUG_INFO("DESTRUCTOR: CKinectWin: START\n");
    free();
    DEBUG_INFO("DESTRUCTOR: CKinectWin: END\n");
}



BOOL  CKinectWin::init(void)
{
    device = new CKinectDevice();

    // for re-Initialize
    ::NuiSetDeviceStatusCallback((NuiStatusProc)&KinectStatusProc, this);

    //
    DWORD mode = NUI_INITIALIZE_FLAG_USES_COLOR |
                 NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | 
                 NUI_INITIALIZE_FLAG_USES_SKELETON |
                 NUI_INITIALIZE_FLAG_USES_AUDIO;

    BOOL ret = device->init(mode, m_use_image); 
    if (!ret) {
        m_err_mesg = device->m_err_mesg;
    }

    //
    setDevState(NI_STATE_DETECT_STOPPED);

    return ret;
}



void  CKinectWin::free(void)
{
    freeRingBuffer();

    deleteDevice();
    return;
}



void  CKinectWin::deleteDevice(void)
{
    DEBUG_INFO("CKinectWin::deleteDevice(): START\n");

    deleteNull(device);

    DEBUG_INFO("CKinectWin::deleteDevice(): END\n");

    return;
}



CString CKinectWin::get_err_message(void) 
{
    DEBUG_INFO("CKinectWin::get_err_message(): => %s\n", ::ts2mbs(m_err_mesg));
    CString mesg = m_err_mesg;
    m_err_mesg = _T("");

    return mesg;
}



void  CKinectWin::clearJointsData(void)
{
    for (int i=0; i<KINECT_JOINT_NUM; i++) {
        rotQuat[i].init(-1.0);
        posVect[i].init(-1.0);
        jntAngl[i] = 0.0;
    }
}



BOOL  CKinectWin::initRingBuffer(void)
{
    BOOL ret = TRUE;

    for (int j=0; j<KINECT_JOINT_NUM; j++) {
        posRing[j].init(NI_RING_BUFFER_SIZE, sizeof(Vector<float>));
        rotRing[j].init(NI_RING_BUFFER_SIZE, sizeof(Quaternion<float>));
        if (posRing[j].state<0 || rotRing[j].state<0) {
            freeRingBuffer();
            m_err_mesg = _T("CKinectWin::initRingBuffer(): WARNING: Out of Memory.");
            ret = FALSE;
            break;
        }
    }

    return ret;
}



void  CKinectWin::freeRingBuffer(void)
{
    for (int j=0; j<KINECT_JOINT_NUM; j++) {
        if (posRing[j].enable) posRing[j].free();
        if (rotRing[j].enable) rotRing[j].free();
    }
}



void  CKinectWin::clearRingBuffer(void)
{
    for (int j=0; j<KINECT_JOINT_NUM; j++) {
        if (posRing[j].enable) posRing[j].clear();
        if (rotRing[j].enable) rotRing[j].clear();
    }
}



void  CKinectWin::backup2RingBuffer(void)
{
    for (int j=0; j<KINECT_JOINT_NUM; j++) {
        if (posVect[j].c>=m_confidence) posRing[j].put(&posVect[j]);
        if (rotQuat[j].c>=m_confidence) rotRing[j].put(&rotQuat[j]);
    }
}




/////////////////////////////////////////////////////////////////////////////////////////////////
//

void  CKinectWin::clearAvatarDetected(void)
{
    clearJointsData();
    clearRingBuffer();

    startPos   = Vector<float>(0.0, 0.0, 0.0);
    currentPos = Vector<float>(0.0, 0.0, 0.0);
    m_is_detected = FALSE;
    m_ground_level= NI_DEFAULT_GROUND_LEVEL;

    return;
}



BOOL  CKinectWin::checkAvatarDetected(void)
{
    if (posVect[NI_SDK_PELVIS].c>m_confidence && crdVect[NI_SDK_PELVIS].c>0.0) {
        startPos = posVect[NI_SDK_PELVIS];
        m_is_detected = TRUE;

        // m_ground_level is not used now
        if (posVect[NI_SDK_R_ANKLE].c>m_confidence && crdVect[NI_SDK_R_ANKLE].c>0.0) {
            m_ground_level = posVect[NI_SDK_R_ANKLE].z - startPos.z;
            if (posVect[NI_SDK_L_FOOT].c>m_confidence && crdVect[NI_SDK_L_FOOT].c>0.0) {
                m_ground_level = Min(m_ground_level, posVect[NI_SDK_L_ANKLE].z - startPos.z);
            }
        }
        else if (posVect[NI_SDK_L_FOOT].c>m_confidence && crdVect[NI_SDK_L_FOOT].c>0.0) {
            m_ground_level = posVect[NI_SDK_L_ANKLE].z - startPos.z;
        }
    }

    return m_is_detected;
}



void  CKinectWin::setTiltMotor(int ang) 
{ 
    if (m_use_motor && m_enable_motor) { 
        m_enable_motor = FALSE;
        NuiCameraElevationSetAngle(ang); 
        Sleep(1000);
        m_enable_motor = TRUE;
    }
}



void  CKinectWin::setMirroring(BOOL mirror)
{ 
    m_is_mirroring = mirror;
}



BOOL  CKinectWin::startDetection(void)
{
    clearJointsData();

    device->smoothParam = NULL;
    if (m_use_knct_smth) device->smoothParam = &smoothParam;

    tracking_user = 0;
    tracking_deny = 0;
    BOOL ret = device->start_Detection(m_profile);
    if (ret) setLEDColor(NI_LED_BLINK_ORANGE);
    else m_err_mesg = device->m_err_mesg;

    // Start Face Tracking
    if (ret && m_use_face) {
        ret = device->create_Face();
        if (!ret) m_err_mesg = device->m_err_mesg;
    }

    return ret;
}



BOOL  CKinectWin::stopDetection(void)
{
    BOOL ret = device->stop_Detection();
    if (!ret) m_err_mesg = device->m_err_mesg;
    setLEDColor(NI_LED_GREEN);  

    tracking_user = 0;
    tracking_deny = 0;

    // Stop Face Tracking
    isDetectFace = FALSE;
    device->delete_Face();

    return ret;
}



BOOL  CKinectWin::restartDetection(void)
{
    BOOL ret = stopDetection();
    if (ret) ret = startDetection();

    return ret;
}



Vector4  CKinectWin::jointPositionData(int j)
{ 
    Vector4 vect;
    
    if (j>=0 && j<KINECT_JOINT_NUM) {
        vect = device->jointPosData[j];
    }
    else {
        memset(&vect, 0, sizeof(Vector4));
    }

    return vect;
}



//
// color is GRAPH_COLOR_BGRA
//
void  CKinectWin::makeDisplayImage()
{
    int    index;
    uByte* src = NULL;
    uByte* ptr;

    if (pViewData==NULL) return;

    if (device->image!=NULL && m_use_image) {
        src = (uByte*)device->image->m_data;
    }

    //
    if (src!=NULL) {
        for (int j=0; j<pViewData->ysize; j++) {
            int li;
            int ls = j*m_image_scale*device->m_xsize;

            for (int i=0; i<pViewData->xsize; i++) {
                // Mirroring
                if (m_is_mirroring) li = i*m_image_scale;
                else                li = (pViewData->xsize-i-1)*m_image_scale; 

                ptr = &(pViewData->point(i, j));
                index  = (ls + li)*4;
                ptr[0] = src[index];        // B
                ptr[1] = src[index+1];      // G
                ptr[2] = src[index+2];      // R
                ptr[3] = src[index+3];      // X
            }
        }
    }

    // No Camera Image
    else {
        for (int j=0; j<pViewData->ysize; j++) {
            int li;
            int ls = j*m_image_scale*device->m_xsize;

            for (int i=0; i<pViewData->xsize; i++) {
                // Mirror
                if (m_is_mirroring) li = i*m_image_scale;
                else                li = (pViewData->xsize-i-1)*m_image_scale; 

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
void  CKinectWin::makeDisplayDepth(CExView* pview)
{
    if (pview==NULL) return;
    pDepthData = &(pview->viewData);

    uWord* src = NULL;
    if (device->depth!=NULL && hasDepthData) {
        src = (uWord*)device->depth->m_data;
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
            wrk[k] = ::NuiDepthPixelToDepth(src[ls+li]);
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

BOOL  CKinectWin::trackingJoints(void)
{
    m_is_tracking = FALSE;

    if (hasDepthData) {
        //
        //isDetectShadow = detectShadow();  // not use
        int tuser = tracking_user;
        tracking_user = device->wait_Skeleton(tracking_user, tracking_deny);    
    
        if (tuser!=tracking_user) {
            m_is_detected = FALSE;
            if (tracking_user==0) {
                clearAvatarDetected();
                lostTrackingUser(tuser);                    // virtual
            }
            else {
                clearJointsData();
                detectTrackingUser(tracking_user);          // virtual
            }
        }

        // Tracking
        if (tracking_user>0 && tracking_user<=KINECT_USERS_NUM) {
            //
            getJointsPosData();
            //
            if (m_is_detected) {
                convertJointsData();    // virtual
                //
                backup2RingBuffer();
                saveJointsData();       // virtual
                loggingJointsData();    // virtual
            }
                //
            paintShadow();
            if (m_is_detected) {
                drawSkeleton(NiGetSkeletonColor(tracking_user), m_skeleton_line);
                drawAddition(NiGetSkeletonColor(tracking_user), m_skeleton_line);   // virtual
            }
            //
            m_is_tracking = TRUE;
        }
    }
    else {
        m_is_detected = FALSE;
    }

    return m_is_tracking;
}



Vector4  CKinectWin::joint_PositionData(int j)
{ 
    Vector4 vect;

    if (j>=0 && j<KINECT_JOINT_NUM) {
        vect = device->jointPosData[j];
    }
    else {
        memset(&vect, 0, sizeof(Vector4));
    }

    return vect;
}


//
// Calculate posVect, crdVect
//
void  CKinectWin::getJointsPosData(void)
{
    if (!m_is_mirroring) {
        for (int j=0; j<KINECT_JOINT_NUM; j++) {
            device->jointPosData[j].x = - device->jointPosData[j].x;
        }
    }

    //
    for (int j=0; j<KINECT_JOINT_NUM; j++) {
        Vector4 pos = device->jointPosData[j];
        int n = j;
        if (m_is_mirroring) n = NiSDKMirrorJointNum(j, NiSDK_Kinect);
        posVect[n].set(-pos.z, pos.x, pos.y);
    }

    // Tracking Mode
    if (m_profile==KINECT_SKEL_PROFILE_ALL) {
        posVect[NI_SDK_TORSO] = 2.0*posVect[NI_SDK_PELVIS] - (posVect[NI_SDK_R_HIP] + posVect[NI_SDK_L_HIP])/2.0;
        posVect[NI_SDK_TORSO].c = Min(posVect[NI_SDK_R_HIP].c, posVect[NI_SDK_L_HIP].c);
        posVect[NI_SDK_TORSO].c = Min(posVect[NI_SDK_TORSO].c, posVect[NI_SDK_PELVIS].c);
    }
    else if (m_profile==KINECT_SKEL_PROFILE_UPPER) {
        /* Neck の座標は体軸上にない
        // NECK + (CSHLDR-NECK)*4
        Vector<float> torso_down = (posVect[NI_SDK_R_SHLDR] + posVect[NI_SDK_L_SHLDR])/2.0 - posVect[NI_SDK_NECK];
        if (torso_down.z>0.0) torso_down = - torso_down;
        Vector<float> vect = posVect[NI_SDK_NECK] + torso_down*4.0;

        posVect[NI_SDK_TORSO] = vect;
        posVect[NI_SDK_TORSO].c = Min(posVect[NI_SDK_R_SHLDR].c, posVect[NI_SDK_L_SHLDR].c);
        posVect[NI_SDK_TORSO].c = Min(posVect[NI_SDK_TORSO].c,   posVect[NI_SDK_NECK].c);
        */

        Vector<float> torso_down(0.0f, 0.0f, -0.2f);

        posVect[NI_SDK_TORSO] = (posVect[NI_SDK_R_SHLDR] + posVect[NI_SDK_L_SHLDR])/2.0 + torso_down;
        posVect[NI_SDK_TORSO].c = Min(posVect[NI_SDK_R_SHLDR].c, posVect[NI_SDK_L_SHLDR].c);
        posVect[NI_SDK_PELVIS] = posVect[NI_SDK_TORSO] + torso_down;
    }

    //
    set2DCoordinate();
    checkBoneLength();
    if (!m_is_detected) checkAvatarDetected();

    //
    currentPos = posVect[NI_SDK_PELVIS];
    for (int j=0; j<KINECT_JOINT_NUM; j++) {
        posVect[j] = posVect[j] - startPos;
        if (posVect[j].c<m_confidence || crdVect[j].c<0.0) posVect[j].c = -1.0;
    }
    checkGroundLevel();
}




/////////////////////////////////////////////////////////////////////////////////////
//

BOOL  CKinectWin::detectShadow(void)
{
    int label = 0;

    if (pViewData==NULL) return FALSE;

    //
    for (int j=0; j<pViewData->ysize; j++) {
        int li;
        int lj = j*m_image_scale;

        for (int i=0; i<pViewData->xsize; i++) {
            // Mirroring
            if (m_is_mirroring) li = i*m_image_scale;
            else                li = (pViewData->xsize-i-1)*m_image_scale; 

            // User Label
            if (getDevState()==NI_STATE_DETECT_EXEC && hasDepthData) {
                //label = device->depth->get_user(li/2, lj/2);  // 320x240
                label = device->depth->get_user_index(li, lj);  // 640x480
                if (label>0) {
                    if (tracking_user==label) return TRUE;
                }
            }
        }
    }

    return FALSE;
}



void  CKinectWin::paintShadow(void)
{
    int    label = 0;
    uByte* ptr;

    if (pViewData==NULL) return;

    if (getDevState()==NI_STATE_DETECT_EXEC && hasDepthData) {
        //
        for (int j=0; j<pViewData->ysize; j++) {
            int li;
            int lj = j*m_image_scale;

            for (int i=0; i<pViewData->xsize; i++) {
                // Mirroring
                if (m_is_mirroring) li = i*m_image_scale;
                else                li = (pViewData->xsize-i-1)*m_image_scale; 

                // User Label
                label = device->depth->get_user_index(li, lj);  // 640x480
                if (label>0) {
                    ptr = &(pViewData->point(i, j));
                    NiSetUserColor(label, ptr, m_use_image);
                }
            }
        }
    }
}



void  CKinectWin::drawSkeleton(int col, int line)
{
    drawJointConnection(NI_SDK_PELVIS,  NI_SDK_TORSO,   col, line);
    drawJointConnection(NI_SDK_TORSO,   NI_SDK_NECK,    col, line);
    drawJointConnection(NI_SDK_NECK,    NI_SDK_HEAD,    col, line);

    drawJointConnection(NI_SDK_NECK,    NI_SDK_L_SHLDR, col, line);
    drawJointConnection(NI_SDK_L_SHLDR, NI_SDK_L_ELBOW, col, line);
    drawJointConnection(NI_SDK_L_ELBOW, NI_SDK_L_WRIST, col, line);
    drawJointConnection(NI_SDK_L_WRIST, NI_SDK_L_HAND,  col, line);

    drawJointConnection(NI_SDK_NECK,    NI_SDK_R_SHLDR, col, line);
    drawJointConnection(NI_SDK_R_SHLDR, NI_SDK_R_ELBOW, col, line);
    drawJointConnection(NI_SDK_R_ELBOW, NI_SDK_R_WRIST, col, line);
    drawJointConnection(NI_SDK_R_WRIST, NI_SDK_R_HAND,  col, line);

    drawJointConnection(NI_SDK_PELVIS,  NI_SDK_L_HIP,   col, line);
    drawJointConnection(NI_SDK_L_HIP,   NI_SDK_L_KNEE,  col, line);
    drawJointConnection(NI_SDK_L_KNEE,  NI_SDK_L_ANKLE, col, line);
    drawJointConnection(NI_SDK_L_ANKLE, NI_SDK_L_FOOT,  col, line);

    drawJointConnection(NI_SDK_PELVIS,  NI_SDK_R_HIP,   col, line);
    drawJointConnection(NI_SDK_R_HIP,   NI_SDK_R_KNEE,  col, line);
    drawJointConnection(NI_SDK_R_KNEE,  NI_SDK_R_ANKLE, col, line);
    drawJointConnection(NI_SDK_R_ANKLE, NI_SDK_R_FOOT,  col, line);
}



void  CKinectWin::drawJointConnection(int j1, int j2, int col, int line)
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



void  CKinectWin::set2DCoordinate(void)
{
    for (int j=0; j<KINECT_JOINT_NUM; j++) {
        crdVect[j].init(-1.0);
    }

    //
    if (device->depth!=NULL && pViewData!=NULL) {
        //
        for (int j=0; j<KINECT_JOINT_NUM; j++) {
            Vector4 pos;

            if (j==NI_SDK_PELVIS || j==NI_SDK_TORSO) {
                if (m_profile==KINECT_SKEL_PROFILE_UPPER) {
                    Vector4 pos_neck = device->jointPosData[NI_SDK_NECK];
                    Vector4 pos_rshd = device->jointPosData[NI_SDK_R_SHLDR];
                    Vector4 pos_lshd = device->jointPosData[NI_SDK_L_SHLDR];

                    pos.x = (FLOAT)((pos_rshd.x + pos_lshd.x)*1.5 - pos_neck.x*2.0);    // NECK + (CSHLDR-NECK)*3   
                    pos.y = (FLOAT)((pos_rshd.y + pos_lshd.y)*1.5 - pos_neck.y*2.0);    
                    pos.z = (FLOAT)((pos_rshd.z + pos_lshd.z)*1.5 - pos_neck.z*2.0);    
                    //pos.w = (FLOAT)((pos_rshd.w + pos_lshd.w)*1.5 - pos_neck.w*2.0);
                }
                else if (j==NI_SDK_TORSO) {
                    Vector4 pos_pelv = device->jointPosData[NI_SDK_PELVIS];
                    Vector4 pos_rhip = device->jointPosData[NI_SDK_R_HIP];
                    Vector4 pos_lhip = device->jointPosData[NI_SDK_L_HIP];
                    
                    pos.x = (FLOAT)(pos_pelv.x*2.0 -(pos_rhip.x+pos_lhip.x)*0.5);
                    pos.y = (FLOAT)(pos_pelv.y*2.0 -(pos_rhip.y+pos_lhip.y)*0.5);
                    pos.z = (FLOAT)(pos_pelv.z*2.0 -(pos_rhip.z+pos_lhip.z)*0.5);
                    //pos.w = (FLOAT)(pos_pelv.w*2.0 -(pos_rhip.w+pos_lhip.w)*0.5);
                }
                else {
                    pos = device->jointPosData[j];  // j==NI_SDK_PELVIS
                }
            }
            else {
                pos = device->jointPosData[j];
            }

            float x, y;
            NuiTransformSkeletonToDepthImage(pos, &x, &y, NUI_IMAGE_RESOLUTION_640x480);

            int xs = (int)x;
            int ys = (int)y;
            if (xs>0 && ys>0) { // 縁(0)は除外
                device->depth->get_image_coord(&xs, &ys);
                if (xs>=0) xs = xs/m_image_scale;
                if (ys>=0) ys = ys/m_image_scale;

                crdVect[j].x = xs;
                crdVect[j].y = ys;
                if (xs>0 && xs<pViewData->xsize && ys>0 && ys<pViewData->ysize) {
                    crdVect[j].c = 1.0;
                }
            }
        }
    }

    return;
}




/////////////////////////////////////////////////////////////////////////////////////
// Speech Platform

#ifdef ENABLE_NI_SPEECH

BOOL  CKinectWin::initSpeech(void)
{
    m_err_mesg = _T("");

    if (audio==NULL) audio = new CKinectAudio();

    BOOL ret = audio->init(device->context, KINECT_AUDIO_SINGLE_AEC);
    if (!ret) m_err_mesg = _T("CKinectWin::initSpeech(): ERROR: Audio Initialization Error!!");
    
    //
    if (ret) {
        if (speech==NULL) speech = makeSpeech();
        WAVEFORMATEX format = audio->getAudioFormat();
        IStream*     stream = audio->getIStream();
        ret = speech->init(stream, &format);
        if (!ret) m_err_mesg = _T("CKinectWin::initSpeech(): ERROR: Speech Initialization Error!!");
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



BOOL  CKinectWin::createSpeech(LPCTSTR lang, LPCTSTR grfile)
{
    m_err_mesg = _T("");

    BOOL ret = TRUE;
    if (speech==NULL) ret = initSpeech(); 

    if (ret) {
        ret = speech->create(lang);
        if (!ret) m_err_mesg = _T("CKinectWin::createSpeech(): ERROR: Speech Context Creation Error!!\nPerhaps, Language Pack is not installed.");
    }
    //
    if (ret) {
        ret = speech->load(grfile);
        if (!ret) {
            m_err_mesg  = _T("CKinectWin::createSpeech(): ERROR: Grammar File Loading Error!!\nFile = ");
            m_err_mesg += grfile;
        }
    }
    
    //
    if (!ret) return FALSE;
    return TRUE;
}



BOOL  CKinectWin::startSpeech(float confidence)
{
    m_err_mesg = _T("");

    if (audio==NULL || speech==NULL) {
        m_err_mesg = _T("CKinectWin::startSpeech(): ERROR: Audio or Speech Instance is/are NULL!!");
        return FALSE;
    }

    BOOL ret = audio->startCapture();
    if (!ret) m_err_mesg = _T("CKinectWin::startSpeech(): ERROR: Audio Capture Starting Error!!");
    //
    if (ret) {
        ret = speech->start(confidence);
        if (!ret) {
            audio->stopCapture();
            m_err_mesg = _T("CKinectWin::startSpeech(): ERROR: Speech Platform Starting Error!!");
        }
    }
    //
    if (!ret) {
        if (m_err_mesg==_T("")) m_err_mesg = _T("CKinectWin::startSpeech(): ERROR: Unknown Error!!");
        return FALSE;
    }
    
    return TRUE;
}



void  CKinectWin::stopSpeech(void)
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



void  CKinectWin::deleteSpeech(BOOL rls)
{
    DEBUG_INFO("CKinectWin::deleteSpeech(): START\n");

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

    DEBUG_INFO("CKinectWin::deleteSpeech(): END\n");
    return;
}


#endif


/////////////////////////////////////////////////////////////////////////////////////
// Face

Quaternion<float>   CKinectWin::getFaceRotation(void)
{
    Quaternion<float> quat(1.0, 0.0, 0.0, 0.0, 1.0);
    Vector<float> vect;
    Vector<float> eulr = device->face->getFaceEulerXYZ();

    if (m_is_mirroring) {
        vect.set(-eulr.x, -eulr.y, eulr.z);
    }
    else {
        vect.set(-eulr.x, eulr.y, -eulr.z);
    }

    quat.setEulerYZX(vect);

    return quat;
}




/////////////////////////////////////////////////////////////////////////////////////
// CallBack 

void  CKinectWin::callback_status_changed(BOOL success, const OLECHAR* instanceName, const OLECHAR* deviceName)
{
    if (success) {
        DEBUG_INFO("CKinectWin:::called_change_status(): SUCCEEDED: %s %s\n", (char*)instanceName, (char*)deviceName);
        // Initialize the Kinect sensor identified by the instanceName parameter.
    }      
    else {
        DEBUG_INFO("CKinectWin::called_change_status(): ERROR: %s %s\n", (char*)instanceName, (char*)deviceName);
        // Uninitialize the Kinect sensor identified by the instanceName parameter.
    }
}




//////////////////////////////////////////////////////////////////////////////////////////
// CallBack 

void  CALLBACK  jbxwl::KinectStatusProc(HRESULT hr, const OLECHAR* instanceName, const OLECHAR* deviceName, void* pUserData)
{
    CKinectWin* kinect = (CKinectWin*)pUserData;

    if (SUCCEEDED(hr)) {
        if (kinect!=NULL) kinect->callback_status_changed(TRUE, instanceName, deviceName);
    }      
    else {
        if (kinect!=NULL) kinect->callback_status_changed(FALSE, instanceName, deviceName);
    }
}
 

#endif      // ENABLE_KINECT_SDK



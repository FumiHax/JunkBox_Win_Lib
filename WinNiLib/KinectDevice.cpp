
#include  "WinBaseLib.h"
#include  "KinectDevice.h"
#include  "NiDevice.h"


#ifdef  ENABLE_KINECT_SDK

using namespace jbxl;
using namespace jbxwl;


//////////////////////////////////////////////////////////////////////////////////////////
// Device

CKinectDevice::CKinectDevice(void)
{
    context     = NULL;

    image       = NULL;
    depth       = NULL;
    skeleton    = NULL;
    face        = NULL;

    smoothParam = NULL;

    m_count     = 0;
    m_state     = NI_STATE_DETECT_STOPPED;
    m_err_mesg  = _T("");

    m_xsize     = 0;
    m_ysize     = 0;
    m_nfps      = 0;
        
    clear_JointsData();
}



CKinectDevice::~CKinectDevice(void)
{
    DEBUG_INFO("DESTRUCTOR: CKinectDevice: START\n");
    free();
    DEBUG_INFO("DESTRUCTOR: CKinectDevice: END\n");
}



BOOL  CKinectDevice::init(DWORD mode, BOOL use_image)
{
    //
    HRESULT hrt = ::NuiGetSensorCount(&m_count);
    if (FAILED(hrt) || m_count<=0) {
        m_err_mesg = _T("CKinectDevice::init(): WARNING: No Kinect is detected.\n");
        return FALSE;
    }

    BOOL ret = create_Context(mode);
    if (ret && use_image) create_Image();
    if (ret) ret = create_Depth();

    if (ret) {
        if (image!=NULL) {
            m_xsize = image->m_xsize;
            m_ysize = image->m_ysize;
            m_nfps  = image->m_nfps;
        }
        else if (depth!=NULL) {
            m_xsize = depth->m_xsize;
            m_ysize = depth->m_ysize;
            m_nfps  = depth->m_nfps;
        }
        else {
            m_err_mesg = _T("CKinectDevice::init(): ERROR: Unknown Error is occurred!!");
            ret = FALSE;
        }
    }

    return ret;
}



void  CKinectDevice::free(void)
{
    delete_Face();
    delete_Depth();
    delete_Image();
    delete_Context();

//  free_Buffer(&m_err_mesg);
}



void  CKinectDevice::clear_JointsData(void)
{
    memset(jointPosData, 0, sizeof(Vector4)*KINECT_JOINT_NUM);
}




///////////////////////////////////////////////////////////////////
// Create

BOOL  CKinectDevice::create_Context(DWORD mode, int index)
{
    HRESULT hrt = NuiCreateSensorByIndex(index, &context);  // Now supports 1 kinect only
    if (FAILED(hrt)) {
        m_err_mesg = _T("CKinectDevice::create_Context(): ERROR: Fail to create Device.");
        return FALSE;
    }

    hrt = context->NuiInitialize(mode);
    if (FAILED(hrt)) {
        delete_Context();
        m_err_mesg = _T("CKinectDevice::create_Context(): ERROR: Fail to initalize Device.");
        return FALSE;
    }
    
    return TRUE;
}



BOOL  CKinectDevice::create_Image(void)
{
    NUI_IMAGE_RESOLUTION reso = NUI_IMAGE_RESOLUTION_640x480;
    //NUI_IMAGE_RESOLUTION reso = NUI_IMAGE_RESOLUTION_320x240;

    if (context==NULL) {
        m_err_mesg = _T("CKinectDevice::create_Image(): ERROR: context is NULL.");
        return FALSE;
    }
    if (image==NULL) image = new CKinectImage();

    //
    HANDLE hdl = image->create(TRUE, FALSE, NULL);
    if (hdl==NULL) {
        delete_Image();
        m_err_mesg = _T("CKinectDevice::create_Image(): ERROR: Fail to create Image.");
        return FALSE;
    }

    HRESULT hrt = context->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, reso, NULL, 2, image->m_handle, &image->m_stream);
    if (FAILED(hrt)) {
        delete_Image();
        m_err_mesg = _T("CKinectDevice::create_Image(): ERROR: Fail to open stream of Image.");
        return FALSE;
    }

    NuiImageResolutionToSize(reso, (DWORD&)image->m_xsize, (DWORD&)image->m_ysize);
    image->m_nfps = 30;

    if (image->m_data==NULL) image->make_data();    // 4byte/pixcel

    return TRUE;
}



BOOL  CKinectDevice::create_Depth(void)
{
    NUI_IMAGE_RESOLUTION reso = NUI_IMAGE_RESOLUTION_640x480;
    //NUI_IMAGE_RESOLUTION reso = NUI_IMAGE_RESOLUTION_320x240;

    if (context==NULL) {
        m_err_mesg = _T("CKinectDevice::create_Depth(): ERROR: context is NULL.");
        return FALSE;
    }
    if (depth==NULL) depth = new CKinectDepth();

    //
    HANDLE hdl = depth->create(TRUE, FALSE, NULL);
    if (hdl==NULL) {
        delete_Depth();
        m_err_mesg = _T("CKinectDevice::create_Depth(): ERROR: Fail to create Depth.");
        return FALSE;
    }

    HRESULT hrt = context->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, reso, NULL, 2, depth->m_handle, &depth->m_stream);
    if (FAILED(hrt)) {
        delete_Depth();
        m_err_mesg = _T("CKinectDevice::create_Depth(): ERROR: Fail to open stream of Depth.");
        return FALSE;
    }

    //
    NuiImageResolutionToSize(reso, (DWORD&)depth->m_xsize, (DWORD&)depth->m_ysize);
    depth->m_nfps = 30;

    if (depth->m_data==NULL) depth->make_data();    // 2byte/pixcel
//  if (!depth->has_map)     depth->make_map();

    return TRUE;
}



BOOL  CKinectDevice::create_Skeleton(int profile)
{
    if (context==NULL) {
        m_err_mesg = _T("CKinectDevice::create_Skeleton(): ERROR: context is NULL.");
        return FALSE;
    }
    if (depth==NULL) {
        m_err_mesg = _T("CKinectDevice::create_Skeleton(): ERROR: depth is NULL.");
        return FALSE;
    }
    if (skeleton==NULL) skeleton = new CKinectSkeleton();

    //
    HANDLE hdl = skeleton->create(TRUE, FALSE, NULL);
    if (hdl==NULL) {
        delete_Skeleton();
        m_err_mesg = _T("CKinectDevice::create_Skeleton(): ERROR: Fail to create Skeleton.");
        return FALSE;
    }

    //
    HRESULT hrt;
    DWORD ftracking = 0;
    DWORD streamode = 0;
    if (profile==KINECT_SKEL_PROFILE_UPPER) {
        ftracking  = NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT;
        ftracking |= NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE;
        streamode  = NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE;
    }

    // for Near mode
    hrt = context->NuiImageStreamSetImageFrameFlags(depth->m_stream, streamode);
    if (SUCCEEDED(hrt)) {
        if (profile==KINECT_SKEL_PROFILE_UPPER) depth->correct_map(-15, 8);
    }

    //
    hrt = context->NuiSkeletonTrackingEnable(skeleton->m_handle, ftracking);
    if (FAILED(hrt)) {
        delete_Skeleton();
        m_err_mesg = _T("CKinectDevice::create_Skeleton(): ERROR: Fail to enable Skeleton Tracking.");
        return FALSE;
    }
    
    return TRUE;
}



BOOL  CKinectDevice::create_Face(void)
{
    if (context==NULL) {
        m_err_mesg = _T("CKinectDevice::create_Face(): ERROR: context is NULL.");
        return FALSE;
    }
    if (image==NULL) {
        m_err_mesg = _T("CKinectDevice::create_Face(): ERROR: image is NULL.");
        return FALSE;
    }
    if (depth==NULL) {
        m_err_mesg = _T("CKinectDevice::create_Face(): ERROR: depth is NULL.");
        return FALSE;
    }

    if (face==NULL) face = new CKinectFaceTracker();

    BOOL ret = face->create(image->m_xsize, image->m_ysize, image->m_data, depth->m_data);
    if (!ret) {
        delete_Face();
        m_err_mesg = _T("CKinectDevice::create_Face(): ERROR: Fail to create Face Tracker\nMaybe FaceTrackData.dll is not found.");
    }

    return ret;
}




///////////////////////////////////////////////////////////////////
// Wait Event

BOOL  CKinectDevice::wait_Image(DWORD msec)
{
    if (image==NULL) return FALSE;
    if (image->m_data==NULL) return FALSE;

    BOOL ret = image->wait(msec);
    if (ret) {
        HRESULT hrt = context->NuiImageStreamGetNextFrame(image->m_stream, 0, &imageFrame); // 0 msec wait
        if (FAILED(hrt)) return FALSE;
        //
        NUI_LOCKED_RECT rect;
        imageFrame.pFrameTexture->LockRect(0, &rect, NULL, 0);
        long int len = rect.Pitch*image->m_ysize;

        memcpy(image->m_data, (char*)rect.pBits, len);
        imageFrame.pFrameTexture->UnlockRect(0);
        context->NuiImageStreamReleaseFrame(image->m_stream, &imageFrame);
    }

    return ret;
}



BOOL  CKinectDevice::wait_Depth(DWORD msec)
{
    if (depth==NULL) return FALSE;
    if (depth->m_data==NULL) return FALSE;

    BOOL ret = depth->wait(msec);
    if (ret) {
        HRESULT hrt =  context->NuiImageStreamGetNextFrame(depth->m_stream, 0, &depthFrame);
        if (FAILED(hrt)) return FALSE;

        //
        NUI_LOCKED_RECT rect;
        depthFrame.pFrameTexture->LockRect(0, &rect, NULL, 0);
        long int len = rect.Pitch*depth->m_ysize;
    
        memcpy(depth->m_data, (char*)rect.pBits, len);
        depthFrame.pFrameTexture->UnlockRect(0);
        context->NuiImageStreamReleaseFrame(depth->m_stream, &depthFrame);
    }

    return ret;
}



int  CKinectDevice::wait_Skeleton(int tuser, int duser, DWORD msec)
{
    if (skeleton==NULL) return 0;

    BOOL ret = skeleton->wait(msec);
    if (ret) {
        HRESULT hrt = context->NuiSkeletonGetNextFrame(0, &skltnFrame);
        if (smoothParam!=NULL && !FAILED(hrt)) {
            hrt = context->NuiTransformSmooth(&skltnFrame, smoothParam);
        }
        if (FAILED(hrt)) return FALSE;

        //
        if (tuser>0) {
            skeleton->m_data = skltnFrame.SkeletonData[tuser-1];
            if (skeleton->m_data.eTrackingState!=NUI_SKELETON_TRACKED) {
                tuser = 0;
            }
        }
        if (tuser==0) {
            tuser = get_TrackingUser(duser);
        }
        if (tuser==0) return 0;

        //
        for (int j=0; j<KINECT_JOINT_NUM; j++) {
            jointPosData[j] = skeleton->m_data.SkeletonPositions[j];
        }
    }
    else tuser = 0;

    return tuser;
}




///////////////////////////////////////////////////////////////////
// Detection

BOOL  CKinectDevice::start_Detection(int profile)
{
    BOOL ret = FALSE;
    if (m_state==NI_STATE_DETECT_EXEC) {
        m_err_mesg = _T("CKinectDevice::start_Detection(): WARNING: detection is already executed.");
        return ret;
    }

    m_state = NI_STATE_DETECT_STARTING;
    clear_JointsData();

    if (depth==NULL) create_Depth();
    depth->make_map();

    ret = create_Skeleton(profile);
    if (ret) {
        m_state = NI_STATE_DETECT_EXEC;
    }
    else {
        m_state = NI_STATE_DETECT_STOPPING;
        delete_Skeleton();
        m_state = NI_STATE_DETECT_STOPPED;
    }

    //
    if (ret) {
        BOOL infoff = context->NuiGetForceInfraredEmitterOff();
        if (infoff) ret = !context->NuiSetForceInfraredEmitterOff(FALSE);
        if (!ret) m_err_mesg = _T("CKinectDevice::start_Detection(): ERROR: Fail to Start Emitter.");       
    }

    return ret;
}



BOOL  CKinectDevice::stop_Detection(void)
{
    if (m_state==NI_STATE_DETECT_STOPPED) {
        m_err_mesg = _T("CKinectDevice::stop_Detection(): WARNING: detection is already stopped.");
        return FALSE;
    }

    m_state= NI_STATE_DETECT_STOPPING;
    context->NuiSetForceInfraredEmitterOff(TRUE);

    ::Sleep(NI_STOP_WAIT_TIME);
    depth->delete_map();
    delete_Skeleton();

    m_state = NI_STATE_DETECT_STOPPED;
    
    return TRUE;
}



int  CKinectDevice::get_TrackingUser(int duser)
{
    int ret = 0;

    if (depth!=NULL && skeleton!=NULL) {
        depth->get_users();
        for (int i=0; i<KINECT_USERS_NUM; i++) {
            int idx = (duser + i) % KINECT_USERS_NUM;
            if (depth->userLabel[idx]>0) {
                skeleton->m_data = skltnFrame.SkeletonData[idx];
                if (skeleton->m_data.eTrackingState==NUI_SKELETON_TRACKED) {
                    ret = idx + 1;
                    break;
                }
            }
        }
    }
    
    return ret;
}




///////////////////////////////////////////////////////////////////
// Delete

void  CKinectDevice::delete_Context(void)
{
    if (context!=NULL) {
        context->Release();
        context->NuiShutdown();
        context = NULL;
    }
    return;
}



void  CKinectDevice::delete_Image(void)
{
    if (image!=NULL) {
        delete(image);
        image = NULL;
    }
    return;
}



void  CKinectDevice::delete_Depth(void)
{
    if (depth!=NULL) {
        delete(depth);
        depth = NULL;
    }
    return;
}



void  CKinectDevice::delete_Skeleton(void)
{
    if (skeleton!=NULL) {
        if (context!=NULL) {
            context->NuiSkeletonTrackingDisable();
        }
        delete(skeleton);
        skeleton = NULL;
    }
    return;
}



void  CKinectDevice::delete_Face(void)
{
    if (face!=NULL) {       
        delete(face);
        face = NULL;
    }
    return;
}




//////////////////////////////////////////////////////////////////////////////////////////
// Image クラス

CKinectImage::CKinectImage(void)
{
    m_data      = NULL;
    m_data_len  = 0;

    m_stream    = NULL;

    m_xsize     = 0;
    m_ysize     = 0;
    m_nfps      = 0;
}



void  CKinectImage::free(void)
{
    if (m_data!=NULL) {
        ::free(m_data);
        m_data = NULL;
        m_data_len = 0;
    }
    return;
}



void  CKinectImage::make_data(void)
{
    if (m_xsize<=0 || m_ysize<=0) return;

    m_data_len = m_xsize*m_ysize*4; // 4Byte/pixcel
    m_data = (uByte*)malloc(m_data_len);

    return;
}




//////////////////////////////////////////////////////////////////////////////////////////
// Depth クラス

CKinectDepth::CKinectDepth(void)
{
    m_data     = NULL;
    m_data_len = 0;

    has_map    = FALSE;
    m_xmap_i2d = NULL;
    m_ymap_i2d = NULL;  
    m_xmap_d2i = NULL;
    m_ymap_d2i = NULL;  
    m_map_len  = 0;

    m_stream   = NULL;

    m_xsize    = 0;
    m_ysize    = 0;
    m_nfps     = 0;
}



void  CKinectDepth::free(void)
{
    if (m_data!=NULL) {
        ::free(m_data);
        m_data = NULL;
        m_data_len = 0;
    }

    delete_map();

    return;
}



void  CKinectDepth::make_data(void)
{
    if (m_xsize<=0 || m_ysize<=0) return;

    m_data_len = m_xsize*m_ysize*2; // 2Byte/pixcel
    m_data = (uByte*)malloc(m_data_len);

    return;
}



void  CKinectDepth::make_map(void)
{
    if (m_xsize<=0 || m_ysize<=0) return;
    if (has_map) delete_map();

    int sz  = m_xsize*m_ysize;
    int len = sizeof(int)*sz;

    m_xmap_i2d = (int*)malloc(len);
    m_ymap_i2d = (int*)malloc(len);
    m_xmap_d2i = (int*)malloc(len);
    m_ymap_d2i = (int*)malloc(len);

    if (m_xmap_i2d==NULL || m_ymap_i2d==NULL || m_xmap_d2i==NULL || m_ymap_d2i==NULL) {
        if (m_xmap_i2d!=NULL) { ::free(m_xmap_i2d); m_xmap_i2d = NULL;}
        if (m_ymap_i2d!=NULL) { ::free(m_ymap_i2d); m_ymap_i2d = NULL;}
        if (m_xmap_d2i!=NULL) { ::free(m_xmap_d2i); m_xmap_d2i = NULL;}
        if (m_ymap_d2i!=NULL) { ::free(m_ymap_d2i); m_ymap_d2i = NULL;}
        return;
    }

    memset(m_xmap_i2d, 0, len);
    memset(m_ymap_i2d, 0, len);
    memset(m_xmap_d2i, 0, len);
    memset(m_ymap_d2i, 0, len);

    long int ii, jj, kk;
    for (int j=0; j<m_ysize; j++) {
        for (int i=0; i<m_xsize; i++) {
            // Depth is NUI_IMAGE_RESOLUTION_320x240
            //HRESULT hdr = NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, NULL, i, j, 0, &ii, &jj);
            //ii = ii/2;
            //jj = jj/2;

            // Depth is NUI_IMAGE_RESOLUTION_640x480
            HRESULT hdr = NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution
                                    (NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_640x480, NULL, (LONG)i, (LONG)j, 0, &ii, &jj);
            //
            if (ii>=0 && ii<m_xsize && jj>=0 && jj<m_ysize) {
                kk = jj*m_xsize + ii;
                m_xmap_i2d[kk] = (int)(i + 1);
                m_ymap_i2d[kk] = (int)(j + 1);
            }
            kk = j*m_xsize + i;
            m_xmap_d2i[kk] = ii;
            m_ymap_d2i[kk] = jj;
        }
    }

    // -1 is no data
    for (int i=0; i<sz; i++) {
        m_xmap_i2d[i]--;
        m_ymap_i2d[i]--;
        if (m_xmap_i2d[i]<0 || m_ymap_i2d[i]<0) {
            m_xmap_i2d[i] = m_ymap_i2d[i] = -1;
        }
    }

    m_map_len = sz;
    has_map = TRUE;

    return;
}



void  CKinectDepth::delete_map(void)
{
    if (m_xmap_i2d!=NULL) {
        ::free(m_xmap_i2d);
        m_xmap_i2d = NULL;
    }
    if (m_ymap_i2d!=NULL) {
        ::free(m_ymap_i2d);
        m_ymap_i2d = NULL;
    }
    if (m_xmap_d2i!=NULL) {
        ::free(m_xmap_d2i);
        m_xmap_d2i = NULL;
    }
    if (m_ymap_d2i!=NULL) {
        ::free(m_ymap_d2i);
        m_ymap_d2i = NULL;
    }

    m_map_len = 0;
    has_map = FALSE;

    return;
}



void  CKinectDepth::correct_map(int cx, int cy)
{
    if (!has_map) make_map();
    if (!has_map) return;

    // Correct Map
    if (cx!=0 || cy!=0) {
        int sz = m_xsize*m_ysize;
        int i2d_x, i2d_y, d2i_x, d2i_y;

        for (int i=0; i<sz; i++) {
            if (m_xmap_i2d[i]>=0) {
                i2d_x = m_xmap_i2d[i] + cx;
                i2d_y = m_ymap_i2d[i] + cy;
                if (i2d_x>=0 && i2d_x<m_xsize) m_xmap_i2d[i] = i2d_x;
                if (i2d_y>=0 && i2d_y<m_ysize) m_ymap_i2d[i] = i2d_y;
            }
            //  
            d2i_x = m_xmap_d2i[i] - cx;
            d2i_y = m_ymap_d2i[i] - cy;
            if (d2i_x>=0 && d2i_x<m_xsize) m_xmap_d2i[i] = d2i_x;
            if (d2i_y>=0 && d2i_y<m_ysize) m_ymap_d2i[i] = d2i_y;
        }
    }

    return;
}



int  CKinectDepth::get_user_index(int i, int j)
{
    int val = 0;
    if (i<0 || j<0 || i>=m_xsize || j>=m_ysize) return val;

    if (has_map) {
        int len = j*m_xsize + i;
        int ii = m_xmap_i2d[len];
        int jj = m_ymap_i2d[len];
        if (ii>=0 && jj>=0 && ii<m_xsize && jj<m_ysize) {
            val = m_data[(jj*m_xsize + ii)*2] & 0x07;
        }
    }
    else {
        val = m_data[(j*m_xsize + i)*2] & 0x07;
    }

    if (val>KINECT_USERS_NUM) val = 0;

    return val;
}



uWord  CKinectDepth::get_depth(int i, int j)
{
    uWord  val = 0;
    if (i<0 || j<0 || i>=m_xsize || j>=m_ysize) return val;

    uWord* ptr = (uWord*)m_data;
    if (has_map) {
        int len = j*m_xsize + i;
        int ii = m_xmap_i2d[len];
        int jj = m_xmap_i2d[len];
        if (ii>=0 && jj>=0) {
            val = ::NuiDepthPixelToDepth(ptr[jj*m_xsize+ii]);
        }
        else {
            val = ::NuiDepthPixelToDepth(ptr[j*m_xsize+i]);
        }
    }
    else {
        val = ::NuiDepthPixelToDepth(ptr[j*m_xsize+i]);
    }

    return val;
}



void  CKinectDepth::get_users(void)
{
    memset(userLabel, 0, KINECT_USERS_NUM*sizeof(int));

    for (int j=0; j<m_ysize; j++) {
        int lj = j*m_xsize;
        for (int i=0; i<m_xsize; i++) {
            int val = m_data[(lj + i)*2] & 0x07;
            if (val>0 && val<=KINECT_USERS_NUM) {
                userLabel[val-1]++;
            }
        }
    }

    return;
}



void  CKinectDepth::get_image_coord(int* i, int* j)
{
    if (i==NULL || j==NULL) return;
    if ((*i)<0 || (*j)<0 || (*i)>=m_xsize || (*j)>=m_ysize) {
        *i = *j = -1;
        return;
    }

    if (has_map) {
        int len = (*j)*m_xsize + (*i);
        *i = m_xmap_d2i[len];
        *j = m_ymap_d2i[len];
    }

    return;
}




//////////////////////////////////////////////////////////////////////////////////////////
// Skeleton クラス

CKinectSkeleton::CKinectSkeleton(void)
{
    m_stream  = NULL;
}



void  CKinectSkeleton::free(void)
{
    return;
}

#endif      // ENABLE_KINECT_SDK


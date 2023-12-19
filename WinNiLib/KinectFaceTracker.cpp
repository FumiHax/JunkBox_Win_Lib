
#include  "WinBaseLib.h"

#include  "KinectFaceTracker.h"
#include  "NiDevice.h"
#include  "Graph.h"


#ifdef  ENABLE_KINECT_SDK

using namespace jbxl;
using namespace jbxwl;


//////////////////////////////////////////////////////////////////////////////////////////
// Face Tracker クラス

CKinectFaceTracker::CKinectFaceTracker(void)
{
    is_detected  = FALSE;

    m_image      = NULL;
    m_depth      = NULL;

    m_tracker    = NULL;
    m_image_data = NULL;
    m_depth_data = NULL;
    m_result     = NULL;
}



void  CKinectFaceTracker::free(void)
{
    //
    if (m_result!=NULL) {
        m_result->Release();
        m_result = NULL;
    }

    if (m_image_data!=NULL) {
        m_image_data->Release();
        m_image_data = NULL;
    }

    if (m_depth_data!=NULL) {
        m_depth_data->Release();
        m_depth_data = NULL;
    }

    if (m_tracker!=NULL) {
        m_tracker->Release();
        m_tracker = NULL;
    }
}



BOOL  CKinectFaceTracker::create(int width, int height, uByte* image, uByte* depth)
{
    if (image==NULL || depth==NULL) return FALSE;

    m_tracker = FTCreateFaceTracker();
    if (m_tracker==NULL) return FALSE;

    //
    FT_CAMERA_CONFIG image_config;
    FT_CAMERA_CONFIG depth_config;

    image_config.Width  = width;
    image_config.Height = height;
    image_config.FocalLength = NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS*(width/640.0f);

    depth_config.Width  = width;
    depth_config.Height = height;
    depth_config.FocalLength = NUI_CAMERA_DEPTH_NOMINAL_FOCAL_LENGTH_IN_PIXELS*(width/320.0f);

    HRESULT hr = m_tracker->Initialize(&image_config, &depth_config, NULL, NULL);
    if (FAILED(hr)) {
        // if hr==0x80070002 error, FaceTrackData.dll is missed
        free();
        return FALSE;
    }

    hr = m_tracker->CreateFTResult(&m_result);
    if (FAILED(hr)) {
        free();
        return FALSE;
    }

    m_image_data  = FTCreateImage();
    m_depth_data  = FTCreateImage();
    if (!m_image_data || !m_depth_data) {
        free();
        return FALSE;
    }

    m_image_data->Attach(width, height, image, FTIMAGEFORMAT_UINT8_B8G8R8X8, width*4);
    m_depth_data->Attach(width, height, depth, FTIMAGEFORMAT_UINT16_D13P3,   width*2);

    //
    m_sensor.pVideoFrame = m_image_data;
    m_sensor.pDepthFrame = m_depth_data;
    m_sensor.ZoomFactor = 1.0f;
    m_sensor.ViewOffset.x = 0;
    m_sensor.ViewOffset.y = 0;

    return TRUE;
}



BOOL  CKinectFaceTracker::detect(FT_VECTOR3D* hint)
{
    if (m_tracker==NULL) return FALSE;

    if (is_detected) {
        HRESULT hr = m_tracker->ContinueTracking(&m_sensor, hint, m_result);
        if (FAILED(hr) || FAILED(m_result->GetStatus())) is_detected = FALSE;
    }
    //
    else {
        HRESULT hr = m_tracker->StartTracking(&m_sensor, NULL, hint, m_result);
        if (SUCCEEDED(hr) && SUCCEEDED(m_result->GetStatus())) is_detected = TRUE;
    }

    return is_detected;
}



RECT   CKinectFaceTracker::getFaceRect(void)
{
    RECT rect;

    this->m_result->GetFaceRect(&rect);
    return rect;
}



Vector<float>  CKinectFaceTracker::getFaceEulerXYZ(void)
{
    Vector<float>  eul(0.0, 0.0, 0.0, 0.0, -1.0);

    FLOAT scale;
    FLOAT rotXYZ[3];
    FLOAT posXYZ[3];

    HRESULT hr = m_result->Get3DPose(&scale, rotXYZ, posXYZ);
    if (FAILED(hr)) return eul;

    eul.set((float)(rotXYZ[0]*DEGREE2RAD), (float)(rotXYZ[1]*DEGREE2RAD), (float)(rotXYZ[2]*DEGREE2RAD));

    return eul;
}



void  CKinectFaceTracker::drawFaceRect(ExCmnHead* viewdata, int scale, BOOL mirror, int col, int line)
{
    if (is_detected && viewdata!=NULL) {
        //
        MSGraph<unsigned int> vp;
        vp.xs = viewdata->xsize;
        vp.ys = viewdata->ysize;
        vp.zs = 1;
        vp.gp = (unsigned int*)viewdata->grptr;

        RECT rect = getFaceRect();
        int xs, xe, ys, ye;
        ys = rect.top/scale;
        ye = rect.bottom/scale;

        if (scale<=0) scale = 1;
        if (mirror) {
            xs = rect.left/scale;
            xe = rect.right/scale;
        }
        else {
            xs = vp.xs - rect.left/scale  - 1;
            xe = vp.xs - rect.right/scale - 1;
        }

        MSGraph_Box(vp, xs, ys, xe, ye, col);
        for (int i=1; i<line; i++) {
            MSGraph_Box(vp, xs-i, ys-i, xe+i, ye+i, col);
            MSGraph_Box(vp, xs+i, ys+i, xe-i, ye-i, col);
        }
    }
}


#endif      // ENABLE_KINECT_SDK


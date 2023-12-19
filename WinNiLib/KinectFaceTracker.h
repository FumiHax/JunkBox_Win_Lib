#pragma once


#ifdef  ENABLE_KINECT_SDK


#include  <NuiApi.h>

#include  "ExClass.h"
#include  "Vector.h"

#include  <FaceTrackLib.h>


// Lib
#pragma  comment(lib, "FaceTrackLib.lib")


//
namespace jbxwl {


//
// Face Tracker クラス
//
class  CKinectFaceTracker
{
public:
    CKinectFaceTracker(void);
    virtual ~CKinectFaceTracker(void) { free();}

public:
    BOOL            is_detected;

    uByte*          m_image;
    uByte*          m_depth;

public:
    IFTFaceTracker* m_tracker;
    IFTImage*       m_image_data;
    IFTImage*       m_depth_data;
    IFTResult*      m_result;

    FT_SENSOR_DATA  m_sensor;

public:
    void    free(void);
    BOOL    create(int width, int height, uByte* image, uByte* depth);

    BOOL    detect(FT_VECTOR3D* hint);
    RECT    getFaceRect(void);
    Vector<double>  getFaceEulerXYZ(void);

    void    drawFaceRect(ExCmnHead* view, int scale, BOOL mirror, int col, int line);
};


}       // namespace


#endif

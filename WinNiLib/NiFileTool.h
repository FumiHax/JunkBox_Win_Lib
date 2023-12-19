#pragma once


#include  "WinTools.h"

#include  "NiJTextTool.h"
#include  "NiBVHTool.h"
#include  "NiVMDTool.h"


//////////////////////////////////////////////////////////////////////////////////////////
// Kinect, Xtion 以外のファイルI/O のデバイスを扱う基本クラス
//
//      JText, BVH, VMD
//

namespace jbxwl {

using namespace jbxl;


class  CNiFileTool
{
public:
    CNiFileTool(void)          { init_data();}
    CNiFileTool(NiSDK_Lib lib) { init_data(); init(lib);}

    virtual ~CNiFileTool(void);

    void            free_data(void);
    void            clear_data(void);

private:
    NiSDK_Lib       m_lib;
    BOOL            m_enable_wfile;     // 一時ファイルに書き込み可能
    FILE*           m_wfp;
    FILE*           m_rfp;

protected:
    void            init(NiSDK_Lib lib);
    void            init_data(void);

    BOOL            open_wfile(LPCTSTR fname, BOOL force);
    BOOL            open_rfile(LPCTSTR fname);
    void            close_wfile(void);
    void            close_rfile(void);

    void            write_header(FILE* fp=NULL);

public:
    // Frame Data
    int             frame_start;
    int             frame_ptr;

    NiFrameData*    frames_data;
    int             frames_num;
    
    int             frame_timer;
    unsigned short  lap_timer;
    double          time_scale;

    double          playback_time;
    int             playback_fps;

public:
    CNiJTextTool*   JTXT_Tool;
    CNiVMDTool*     VMD_Tool;
    CNiBVHTool*     BVH_Tool;

public:
    CBaseFrameTool* frameTool;

    // Joints Text
    int         readJTextFile(LPCTSTR fname);
    BOOL        writeJTextData(FILE* fp, Vector<double>* pos, Quaternion<double>* rot, double* agl, BOOL mrr, NiSDK_Lib lib=NiSDK_Default);

    BOOL        saveJTextfromFile(LPCTSTR tempf, LPCTSTR fname, BOOL force);
    BOOL        writeTempJText(Vector<double>* pos, Quaternion<double>* rot, double* agl, BOOL mrr, NiSDK_Lib lib=NiSDK_Default);

    // BVH
    int         readBVHFile(LPCTSTR fname);
    BOOL        saveBVHfromFile(LPCTSTR tempf, LPCTSTR fname, BVHSaveParam param);
    BOOL        writeBVHFile(LPCTSTR fname, BVHSaveParam param);

    // VMD
    int         readVMDFile(LPCTSTR fname);
    BOOL        saveVMDfromFile(LPCTSTR tempf, LPCTSTR fname);

public:
    // Frame Data
    void        clearFrameData(void);

    BOOL        startFrame(int start=0);
    BOOL        nextFrame(BOOL next=TRUE);
    void        setFrame(int frame);
    BOOL        resetFrame(void) { return startFrame();}

public:
    //
    virtual void Vector2Quaternion(Vector<double>* vt, Quaternion<double>* qt) {}

    // Joints
    void        clearStartPosition(void) { startPos.init(1.0);}
    void        clearJointsData(void);

    void        getJointsDataSeq(BOOL mirror, int frame=-1);


    Vector<double> startPos;
    Vector<double> currentPos;

    Vector<double>     posVect[NI_TOTAL_JOINT_NUM];
    Quaternion<double> rotQuat[NI_TOTAL_JOINT_NUM];
};


}       // namespace


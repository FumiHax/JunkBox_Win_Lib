#pragma once

#include  "common++.h"
#include  "tools++.h"

#include  "NiJointsTool.h"


#define   VMD_FILE_HD_ID2   "Vocaloid Motion Data 0002"

#define   VMD_JOINT_NUM     39
#define   VMD_GRID_UNIT     0.08    // m/grid

#define   VMD_FARME_RATE    30      // FPS


//
#define   VMD_PARENT         0
#define   VMD_CENTER         1
#define   VMD_LOWER          2
#define   VMD_UPPER          3
#define   VMD_UPPER2         4
#define   VMD_NECK           5
#define   VMD_HEAD           6
#define   VMD_SKULL          7

#define   VMD_EYES           8
#define   VMD_L_EYE          9
#define   VMD_R_EYE         10
#define   VMD_L_BUST        11
#define   VMD_R_BUST        12
    
#define   VMD_L_SHLDR       13
#define   VMD_L_ARM         14
#define   VMD_L_ARM_TW      15
#define   VMD_L_ELBOW       16
#define   VMD_L_WRIST_TW    17
#define   VMD_L_WRIST       18
#define   VMD_L_HAND        19
    
#define   VMD_R_SHLDR       20
#define   VMD_R_ARM         21
#define   VMD_R_ARM_TW      22
#define   VMD_R_ELBOW       23
#define   VMD_R_WRIST_TW    24
#define   VMD_R_WRIST       25
#define   VMD_R_HAND        26

#define   VMD_L_HIP         27
#define   VMD_L_KNEE        28
#define   VMD_L_ANKLE_IK    29
#define   VMD_L_ANKLE       30
#define   VMD_L_TOE         31
#define   VMD_L_TOE_IK      32

#define   VMD_R_HIP         33
#define   VMD_R_KNEE        34
#define   VMD_R_ANKLE_IK    35
#define   VMD_R_ANKLE       36
#define   VMD_R_TOE         37
#define   VMD_R_TOE_IK      38



namespace jbxwl {

using namespace jbxl;


typedef struct _vmd_file_header
{
    char          header[30];
    char          name[20];
    unsigned int  data_num;
    //
} VMDFileHeader;


//
typedef struct _vmd_joint_data
{
    char          name[15];
    unsigned long frm_num;
    double        posx;
    double        posy;
    double        posz;
    double        qutx;
    double        quty;
    double        qutz;
    double        qutw;
    char          param[64];    // 補間係数
    //
} VMDJointData;



/*
// 未対応
typedef struct _vmd_face_frame
{
    char          name[15];
    unsigned long num;
    double        value;
    //
} VMDFaceFrame;
*/



class CNiVMDTool : public CBaseFrameTool
{
public:
    CNiVMDTool(void);
    virtual ~CNiVMDTool(void);

public:
    void            free_data(void);
    void            clear_data(void);

public:
    virtual unsigned int    getFramesNumber(void) { return dmy_frmnum;}
    virtual NiFrameData *   getFramesData(void);                // フレームの情報と全フレームへのポインタ
    virtual NiJointData*    getJointsData(int frmnum, int fps); // 動的にフレームデータ（補間）を計算．

    virtual BOOL            readFile(FILE* fp);

//
private:
    Quaternion<double> A2TPose;
    double             rate_frame;

    //
    VMDFileHeader   vmd_header;
    VMDJointData*   vmd_frames;
    unsigned int    vmd_datnum;

    //
    NiFrameData*    dmy_frames;
    unsigned int    dmy_frmnum;

private:
    VMDFileHeader   readFileHeader(FILE* fp);
    VMDJointData    readJointData (FILE* fp);
    VMDJointData*   readJointsData(FILE* fp, unsigned int& frmnum);

    NiFrameData*    convert2FrameData(VMDJointData* motion_data, unsigned int datnum, unsigned int& frmnum);
    void            calcJointRotation(void);
    void            calcJointIK(int fnum);

    void            calcLegIK_CCD(Vector<double>* vect, Vector<double> ik, Quaternion<double>* quat, int rpmax);
};




////////////////////////////////////////////////////////////////////////////////////////////////
//

std::string VMDJointName(int n);
int         VMDJointNum(char* name);

int         VMD2NiJointNum(int joint);


}



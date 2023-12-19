#pragma once

#include  "NiJointsTool.h"


namespace jbxwl {

using namespace jbxl;


////////////////////////////////////////////////////////////////////////////////////////
//  CNiJTextTool Class
//

class CNiJTextTool : public CBaseFrameTool
{
public:
    CNiJTextTool(void);
    virtual ~CNiJTextTool(void);

public:
    void        free_data(void);
    void        clear_data(void);

    double*     jntAngl;

public:
    virtual NiJointData*    getJointsData(int frmnum, int fps);     // 補間データ

    virtual BOOL            readFile(FILE* fp);

//
public:
    void        writeHeader(FILE* fp) { if (fp!=NULL) fprintf(fp, "%s\n", NI_JTXT_FILE_ID);}
    void        writeCurrentData(FILE* fp, unsigned short msec);
    //
    void        setPosVect(Vector<double>* pos,     NiSDK_Lib lib, BOOL mirror);
    void        setRotQuat(Quaternion<double>* rot, NiSDK_Lib lib, BOOL mirror);
    void        setJntAngl(double* agl,             NiSDK_Lib lib, BOOL mirror);
};


}   // name space


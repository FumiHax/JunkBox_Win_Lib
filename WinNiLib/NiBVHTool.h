#pragma once

#include  "NiBVHName.h"


//
namespace jbxwl {

using namespace jbxl;


typedef struct 
{
    int     format;
    int     fps;
    int     divtm;
    float   scale;
    BOOL    recalc;
    //
} BVHSaveParam;



//
class CNiBVHTool : public CBaseFrameTool
{
public:
    CNiBVHTool(void)  { init();}
    virtual ~CNiBVHTool(void);

public:
    void        free_data(void);
    void        clear_data(void);

    //
    virtual NiJointData*    getJointsData(int frmnum, int fps); // 補間データ

    virtual int             getPlayBackFPS(void);

    virtual BOOL            readFile(FILE* fp);
    virtual BOOL            writeFile(FILE* fp);

private:
    //
    CBVHTool*   bvh_tool;
    tList*      bvh_name_list;

private:
    void        init(void);
};


}


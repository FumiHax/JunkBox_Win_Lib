
#include "NiBVHTool.h"

using namespace jbxl;
using namespace jbxwl;


//////////////////////////////////////////////////////////////////////////////////////////////////////
// CNiBVHTool Class

CNiBVHTool::~CNiBVHTool(void)
{
    DEBUG_INFO("DESTRUCTOR: CNiBVHTool\n");

    free_data();
}



void  CNiBVHTool::init()
{
    bvh_tool = new CBVHTool();

    bvh_name_list = setBVHJointName();
}



void  CNiBVHTool::free_data(void)
{
    //
    if (bvh_tool!=NULL) delete(bvh_tool);
    bvh_tool = NULL;

    if (bvh_name_list!=NULL) {
        del_tList(&bvh_name_list);
        bvh_name_list = NULL;
    }

    clear_data();
}



void  CNiBVHTool::clear_data(void)
{
    if (bvh_tool!=NULL) bvh_tool->free_data();

    CBaseFrameTool::free_data();
}




/////////////////////////////////////////////////////////////////////////////////////
//

BOOL  CNiBVHTool::readFile(FILE* fp)
{
    if (fp==NULL || bvh_tool==NULL) return FALSE;

    clear_data();

    bvh_tool->readBVH(fp);
    if (bvh_tool->state!=0) return FALSE;

    //
    joints_num = bvh_tool->joint_num;
    if (jointsData!=NULL) ::free(jointsData);
    jointsData = (NiJointData*)malloc(sizeof(NiJointData)*joints_num);
    if (jointsData==NULL) {
        clear_data();
        return FALSE;
    }
    clearJointsData(joints_num);
    
    frames_num = bvh_tool->frame_num;
    framesData = makeFramesData(frames_num, joints_num, NULL);
    if (framesData==NULL) {
        clear_data();
        return FALSE;
    }

    //
    tList* namelist = addBVHJointName(BVH_ADD_JOINTNAME_TXT, bvh_name_list);
    
    //
    for (unsigned int i=0; i<frames_num; i++) {
        framesData[i].frmn = (int)i;                                // フレーム番号
        framesData[i].msec = (int)(bvh_tool->frame_time*i*1000);    // フレーム時間 ms
        framesData[i].jnum = joints_num;                            // Joint の数

        posVect = bvh_tool->getPosData(i);
        rotQuat = bvh_tool->getQuaternion(i);

        NiJointData* jdata = framesData[i].jdat;
        for (int j=0; j<bvh_tool->joint_num; j++) {
            jdata[j].joint  = BVHJoint2NiJointNum(namelist, bvh_tool->joint_name[j]);
            jdata[j].index  = i;
            jdata[j].vect.x = posVect[j].z;
            jdata[j].vect.y = posVect[j].x;
            jdata[j].vect.z = posVect[j].y;
            jdata[j].vect.c = 1.0;
            jdata[j].quat   = rotQuat[j];
            jdata[j].quat.c = 1.0;
        }

        //
        free(posVect);
        free(rotQuat);
        posVect = NULL;
        rotQuat = NULL;
    }

    /*
    tList* pp = namelist;
    while(pp!=NULL) {
        DEBUG_INFO("%d => %s\n", pp->ldat.id, pp->ldat.key.buf);
        pp = pp->next;
    }*/

    //
    start_time = framesData[0].msec;
    stop_time  = framesData[frames_num-1].msec;
    exec_time  = stop_time - start_time;

    del_tList(&namelist);

    return TRUE;
}



BOOL   CNiBVHTool::writeFile(FILE* fp)
{
    BOOL ret = FALSE;

    if (bvh_tool!=NULL) {
        ret = bvh_tool->printBVH(fp);
    }

    return ret;
}



int  CNiBVHTool::getPlayBackFPS(void)
{
    if (bvh_tool->framepsec<=0) {
        if (bvh_tool->frame_time!=0.0) bvh_tool->framepsec = (int)(1.0/bvh_tool->frame_time);
        else                           bvh_tool->framepsec = 30;
    }

    return bvh_tool->framepsec;
}



NiJointData*  CNiBVHTool::getJointsData(int frmnum, int fps) 
{ 
    if (frmnum<0) return NULL;
    
    if (fps<=0) fps = 30;
    int msec = (int)(1000./fps*frmnum);
    if (msec>exec_time) return NULL;

    unsigned int f;
    double t = 0.0;

    for (f=1; f<frames_num; f++) {
        if (framesData[f].msec >= msec + start_time) {
            t = (float)(msec - framesData[f-1].msec)/(float)(framesData[f].msec - framesData[f-1].msec);
            break;
        }
    }
    if (f>=frames_num) return NULL;

    //
    clearJointsData(joints_num);

    NiJointData* jdat1 = framesData[f-1].jdat;
    NiJointData* jdat2 = framesData[f].jdat;

    for (int j=0; j<joints_num; j++) {
        jointsData[j].joint = jdat1[j].joint;
        jointsData[j].index = frmnum;
        jointsData[j].vect  = BSplineInterp4 (jdat1[j].vect, jdat2[j].vect, t);
        jointsData[j].quat  = SlerpQuaternion(jdat1[j].quat, jdat2[j].quat, t);
    }

    return jointsData;
}


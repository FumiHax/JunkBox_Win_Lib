
#include "NiVMDTool.h"


using namespace jbxl;
using namespace jbxwl;


static std::string  _VMDJointName[] =       // VMD_JOINT_NUM
{
    // SJIS
    "全ての親", "センター", 
    "下半身", "上半身", "上半身2", "首", "頭", "頭先", 
    "両目", "左目", "右目", "左胸", "右胸",

    "左肩", "左腕", "左腕捩", "左ひじ", "左手捩", "左手首", "左手先",
    "右肩", "右腕", "右腕捩", "右ひじ", "右手捩", "右手首", "右手先",


    "左足", "左ひざ", "左足ＩＫ", "左足首", "左つま先", "左つま先ＩＫ",
    "右足", "右ひざ", "右足ＩＫ", "右足首", "右つま先", "右つま先ＩＫ"
};


//
static std::string  _VMDJointName_eng[] =   // VMD_JOINT_NUM
{
    "parent", "center", 
    "lower body", "upper body", "upper body2", "neck", "head", "skull", 
    "eyes", "eye_L", "eye_R", "bust_L", "bust_R",

    "shoulder_L", "arm_L", "arm twist_L", "elbow_L", "wrist twist_L", "wrist_L", "c wrist l",
    "shoulder_R", "arm_R", "arm twist_R", "elbow_R", "wrist twist_R", "wrist_R", "c wrist r",

    "leg_L", "knee_L", "leg_IK_L", "ankle_L", "toe_L", "toe_IK_L",
    "leg_R", "knee_R", "leg_IK_R", "ankle_R", "toe_R", "toe_IK_R"
};



/**
std::string  jbxwl::VMDJointName(int n)

VMDのジョイント番号から，MMDのジョイント名を得る
*/
std::string  jbxwl::VMDJointName(int n)
{
    std::string str = "";

    if (n>=0 && n<VMD_JOINT_NUM) {
        str = _VMDJointName[n];
    }

    return str; 
}



/**
int   jbxwl::VMDJointNum(char* name)

MMDのジョイント名から，VMDのジョイント番号を得る
*/
int   jbxwl::VMDJointNum(char* name)
{
    for (int i=0; i<VMD_JOINT_NUM; i++) {
        if (!strcmp(name, _VMDJointName[i].c_str()) || !strcmp(name, _VMDJointName_eng[i].c_str())) {
            return i;
        }
    }
    return -1;
}



/*
VMD_PARENT      ( 0)  => none
VMD_CENTER      ( 1)  => NI_PELVIS  ( 0)
VMD_LOWER       ( 2)  => NI_WAIST   ( 1)
VMD_UPPER       ( 3)  => NI_TORSO   ( 2)    
VMD_UPPER2      ( 4)  => NI_CHEST   ( 3)
VMD_NECK        ( 5)  => NI_NECK    ( 4)
VMD_HEAD        ( 6)  => NI_HEAD    ( 5)
VMD_SKULL       ( 7)  => NI_SKULL   ( 6)    

VMD_EYES        ( 8)  => none
VMD_L_EYE       ( 9)  => NI_L_EYE   ( 7)    
VMD_R_EYE       (10)  => NI_R_EYE   ( 8)
VMD_L_BUST      (11)  => NI_L_BUST  ( 9)
VMD_R_BUST      (12)  => NI_R_BUST  (10)
                    
VMD_L_SHLDR     (13)  => NI_L_COLLAR(11)
VMD_L_ARM       (14)  => NI_L_SHLDR (12)
VMD_L_ARM_TW    (15)  => none
VMD_L_ELBOW     (16)  => NI_R_ELBOW (13)
VMD_L_WRIST_TW  (17)  => none
VMD_L_WRIST     (18)  => NI_L_WRIST (14)
VMD_L_HAND      (19)  => NI_R_HAND  (15)
                    
VMD_R_SHLDR     (20)  => NI_R_COLLAR(17)
VMD_R_ARM       (21)  => NI_R_SHLDR (18)
VMD_R_ARM_TW    (22)  => none
VMD_R_ELBOW     (23)  => NI_R_ELBOW (19)
VMD_R_WRIST_TW  (24)  => none
VMD_R_WRIST     (25)  => NI_R_WRIST (20)
VMD_R_HAND      (26)  => NI_R_HAND  (21)
                    
VMD_L_HIP       (27)  => NI_L_HIP   (23)
VMD_L_KNEE      (28)  => NI_L_KNEE  (24)
VMD_L_ANKLE_IK  (29)  => NI_L_ANKLE (25)
VMD_L_ANKLE     (30)  => NI_L_FOOT  (26)
VMD_L_TOE       (31)  => NI_L_TOE   (27)
VMD_L_TOE_IK    (32)  => none
                    
VMD_R_HIP       (33)  => NI_R_HIP   (28)
VMD_R_KNEE      (34)  => NI_R_KNEE  (29)
VMD_R_ANKLE_IK  (35)  => NI_R_ANKLE (30)
VMD_R_ANKLE     (36)  => NI_R_FOOT  (31)
VMD_R_TOE       (37)  => NI_R_TOE   (32)
VMD_R_TOE_IK    (38)  => none
*/

//
// VMDのジョイント番号から，共通ジョイントの番号を得る
//
static  int  _VMD2NiJointNum[] =        // VMD_JOINT_NUM (37)
{
    -1,  0, 
     1,  2,  3,  4,  5,  6,
    -1,  7,  8,  9, 10,
    11, 12, -1, 13, -1, 14, 15,
    17, 18, -1, 19, -1, 20, 21,
    23, 24, 25, 26, 27, -1,
    28, 29, 30, 31, 32, -1
};



/**
 VMDのジョイント番号から，共通ジョイントの番号を得る
*/
int  jbxwl::VMD2NiJointNum(int joint)
{
    if (joint<0 || joint>=VMD_JOINT_NUM) return -1;

    return _VMD2NiJointNum[joint];
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
// CNiVMDTool Class

CNiVMDTool::CNiVMDTool(void)
{
    memset(&vmd_header, 0, sizeof(VMDFileHeader));

    vmd_frames = NULL;
    vmd_datnum = 0;
    dmy_frames = NULL;
    dmy_frmnum = 0;
    joints_num = VMD_JOINT_NUM;

    rate_frame = VMD_FARME_RATE/30.0;
    A2TPose.setRotation(37.0/180.0*PI, 1.0, 0.0, 0.0, 1.0);

    posVect = (Vector<double>*)malloc(sizeof(Vector<double>)*VMD_JOINT_NUM);
    rotQuat = (Quaternion<double>*)malloc(sizeof(Quaternion<double>)*VMD_JOINT_NUM);
}



CNiVMDTool::~CNiVMDTool(void)
{
    DEBUG_INFO("DESTRUCTOR: CNiVMDTool\n");

    free_data();
}



void  CNiVMDTool::free_data(void)
{
    if (dmy_frames!=NULL) ::freeFramesData(dmy_frames, dmy_frmnum);
    if (vmd_frames!=NULL) ::free(vmd_frames);

    vmd_frames = NULL;
    vmd_datnum = 0;
    dmy_frames = NULL;
    dmy_frmnum = 0;
    
    clear_data();
}



void  CNiVMDTool::clear_data(void)
{
    CBaseFrameTool::free_data();
}




////////////////////////////////////////////////////////////////////////////////////////////////////
//

/**
BOOL  CNiVMDTool::readFile(FILE* fp)

VMDファイルを読み込む
*/
BOOL  CNiVMDTool::readFile(FILE* fp)
{
    if (fp==NULL) return FALSE;

    clear_data();

    vmd_datnum = 0;
    vmd_header = readFileHeader(fp);                        // ヘッダ部
    if (vmd_header.data_num==0) return FALSE;

    vmd_frames = readJointsData(fp, vmd_header.data_num);   // 全フレームのデータ
    if (vmd_frames==NULL) return FALSE;
    vmd_datnum = vmd_header.data_num;

    //
    joints_num = VMD_JOINT_NUM;
    if (jointsData!=NULL) ::free(jointsData);
    jointsData = (NiJointData*)malloc(sizeof(NiJointData)*joints_num);
    if (jointsData==NULL) {
        clear_data();
        return FALSE;
    }
    clearJointsData(joints_num);

    if (posVect==NULL) posVect = (Vector<double>*)malloc(sizeof(Vector<double>)*VMD_JOINT_NUM);
    if (rotQuat==NULL) rotQuat = (Quaternion<double>*)malloc(sizeof(Quaternion<double>)*VMD_JOINT_NUM);
    if (posVect==NULL || rotQuat==NULL) {
        clear_data();
        return FALSE;
    }
    clearVectorData(VMD_JOINT_NUM);

    return TRUE;
}



/**
VMDFileHeader  CNiVMDTool::readFileHeader(FILE* fp)

VMDファイルのヘッダ部を読み込む
*/
VMDFileHeader  CNiVMDTool::readFileHeader(FILE* fp)
{
    VMDFileHeader fhd;

    fread(fhd.header,   30, 1, fp);
    fread(fhd.name,     20, 1, fp);
    fread(&fhd.data_num, 4, 1, fp);
    
    if (strcmp(VMD_FILE_HD_ID2, fhd.header)) {
        fhd.data_num = 0;
    }

    return fhd;
}



/**
VMDJointData  CNiVMDTool::readJointData(FILE* fp)

VMDファイルから 1フレーム（ジョイント）の VMDJointDataを読み込む．@n
sizeof(VMDJointData)==15 + (4+4x7+64)
*/
VMDJointData  CNiVMDTool::readJointData(FILE* fp)
{
    VMDJointData frame;

    fread(frame.name,     15, 1, fp);
    fread(&frame.frm_num, 96, 1, fp);   // 4 + 4x7 + 64

    return frame;
}



/**
VMDJointData*  CNiVMDTool::readJointsData(FILE* fp, unsigned int& frmnum)

全フレーム＆ジョイントの  VMDJointDataを読み込む．@n
ジョイント間でフレームの同期は取られていない．
*/
VMDJointData*  CNiVMDTool::readJointsData(FILE* fp, unsigned int& frmnum)
{
    VMDJointData* motion_data = (VMDJointData*)malloc(frmnum*sizeof(VMDJointData));
    if (motion_data==NULL) return NULL;
    memset(motion_data, 0, frmnum*sizeof(VMDJointData));

    //
    unsigned int num = 0;
    while(num<frmnum) {
        motion_data[num] = readJointData(fp);
        if (feof(fp)) break;
        num++;  
    }

    frmnum = num;
    if (frmnum==0) {
        ::free(motion_data);
        motion_data = NULL;
    }

    return motion_data;
}



/**
NiFrameData*  CNiVMDTool::getJointsFrame(void)

ジョイントの各フレームデータを計算し，静的なデータとして返す．
ただし，VMDでは全フレームのデータがないので，ここではダミーデータを返し，実際のデータは getFrameData()で得る．

@see convert2FrameData()
@see calcJointRotation()
*/
NiFrameData*  CNiVMDTool::getFramesData(void)
{
    joints_num = VMD_JOINT_NUM;

    /////////////////////////////////
    if (framesData!=NULL) clear_data();
    framesData = convert2FrameData(vmd_frames, vmd_datnum, frames_num);
    if (framesData==NULL) return NULL;

    calcJointRotation();
    /////////////////////////////////

    // ジョイント番号の書き換え
    for (unsigned int i=0; i<frames_num; i++) {
        //
        for (int j=0; j<framesData[i].jnum; j++) {
            int n = VMD2NiJointNum(framesData[i].jdat[j].joint);
            framesData[i].jdat[j].joint = n;
        }
    }

    /**/
    dmy_frmnum = (int)(framesData[frames_num-1].frmn*rate_frame);
    dmy_frames = makeFramesData(dmy_frmnum, 0, NULL);
    if (dmy_frames==NULL) {
        dmy_frmnum = frames_num;
        return framesData;
    }

    for (unsigned int i=0; i<dmy_frmnum; i++) {
        dmy_frames[i].jnum = joints_num;
        dmy_frames[i].msec = (int)(i*(100.0/(rate_frame*3.0)));
    }
    if (rate_frame!=1.0) {
        for (unsigned int i=0; i<frames_num; i++) {
            framesData[i].frmn = (int)(framesData[i].frmn*rate_frame);
            framesData[i].msec = (int)(framesData[i].msec/rate_frame);
        }
    }
    
    start_time = framesData[0].msec;
    stop_time  = framesData[frames_num-1].msec;
    exec_time  = stop_time - start_time;

    return dmy_frames;
}



/**
NiFrameData*  CNiVMDTool::convert2FrameData(VMDJointData* motion_data, unsigned int datanum, unsigned int& framenum)

VMDの全フレームデータを共通ジョイントのフレームデータに変換する．

@param[in]  datanum  データの総数
@param[out] framenum 一意的なフレームの数
@return  （ダミー）フレームデータへのポインタ
*/
NiFrameData*  CNiVMDTool::convert2FrameData(VMDJointData* motion_data, unsigned int datanum, unsigned int& framenum)
{
    framenum = 0;
    if (motion_data==NULL || datanum<=0) return NULL;

    // VMDのデータをフレーム番号でソート
    VMDJointData swap_motion;
    unsigned int k = datanum - 1;
    while (k>0) {
        unsigned int j = 0;
        for (unsigned int i=0; i<k; i++) {
            if (motion_data[i].frm_num>motion_data[i+1].frm_num) {
                swap_motion      = motion_data[i];
                motion_data[i]   = motion_data[i+1];
                motion_data[i+1] = swap_motion;
                j = i;
            }
        }
        k = j;
    }

    // 一意的なフレーム数を数える
    unsigned int uniq_frame = 1;

    unsigned int frm_num = motion_data[0].frm_num;
    for (unsigned int i=1; i<datanum; i++) {
        if (frm_num!=motion_data[i].frm_num) {
            uniq_frame++;
            frm_num = motion_data[i].frm_num;
        }
    }

    NiFrameData* ni_joints = makeFramesData((int)uniq_frame, joints_num, NULL);
    if (ni_joints==NULL) return NULL;
    framenum = uniq_frame;

    // Joints
    unsigned int datacnt = 0;
    for (unsigned int i=0; i<uniq_frame; i++) {
        frm_num = motion_data[datacnt].frm_num;
        ni_joints[i].frmn = frm_num;
        ni_joints[i].msec = (int)(frm_num*33.3333333333333);        // msec
        //
        while (frm_num==motion_data[datacnt].frm_num) {
            int joint = VMDJointNum(motion_data[datacnt].name);
            if (joint>=0) {
                NiJointData* jdat = &(ni_joints[i].jdat[joint]);
                jdat->joint  = joint;
                jdat->index  = i;
                jdat->vect.x = -motion_data[datacnt].posz*(double)VMD_GRID_UNIT;
                jdat->vect.y =  motion_data[datacnt].posx*(double)VMD_GRID_UNIT;
                jdat->vect.z =  motion_data[datacnt].posy*(double)VMD_GRID_UNIT;
                jdat->quat.x =  motion_data[datacnt].qutz;
                jdat->quat.y = -motion_data[datacnt].qutx;
                jdat->quat.z = -motion_data[datacnt].quty;
                jdat->quat.s =  motion_data[datacnt].qutw;
                if (jdat->quat.s<0.0) jdat->quat = - jdat->quat;
                jdat->quat.normalize();

                //
                if (jdat->vect.x!=0.0 || jdat->vect.y!=0.0 || jdat->vect.z!=0.0) {
                    jdat->vect.c = 1.0;
                }
                if (jdat->quat.x!=0.0 || jdat->quat.y!=0.0 || jdat->quat.z!=0.0 || jdat->quat.s!=0.0) { 
                    jdat->quat.c = 1.0;
                }
            }

            datacnt++;
            if (datacnt==datanum) break;
        }
    }

    //
    return ni_joints;
}



/**
void  CNiVMDTool::calcJointRotation(void)

全ジョイントのクオータニオンを計算し直す．
*/
void  CNiVMDTool::calcJointRotation(void)
{
    if (framesData==NULL || frames_num<=0) return;

    if (posVect==NULL) posVect = (Vector<double>*)malloc(sizeof(Vector<double>)*VMD_JOINT_NUM);
    if (rotQuat==NULL) rotQuat = (Quaternion<double>*)malloc(sizeof(Quaternion<double>)*VMD_JOINT_NUM);
    if (posVect==NULL || rotQuat==NULL) return;
    clearVectorData(VMD_JOINT_NUM);

    NiJointData  prevJoint[VMD_JOINT_NUM];  // joints_num = VMD_JOINT_NUM
    for (int j=0; j<VMD_JOINT_NUM; j++) {
        prevJoint[j].joint = -1;
        prevJoint[j].index = -1;
        prevJoint[j].vect.init(-1.0);
        prevJoint[j].quat.init(-1.0);
    }

    // 関節の変換
    for (unsigned int i=0; i<frames_num; i++) {
        //
        NiJointData* jdata = framesData[i].jdat;

        jdata[VMD_CENTER].vect.c = 1.0;
        jdata[VMD_CENTER].quat.c = 1.0;

        for (int j=0; j<framesData[i].jnum; j++) {
            //
            if (jdata[j].joint>=0) {
                posVect[j] = jdata[j].vect;
                rotQuat[j] = jdata[j].quat;
                if (posVect[j].c<0.0) posVect[j] = prevJoint[j].vect;
                if (rotQuat[j].c<0.0) rotQuat[j] = prevJoint[j].quat;
            }
            //
            else {
                posVect[j] = prevJoint[j].vect;
                rotQuat[j] = prevJoint[j].quat;
            }
        }

        ///////////////////////////////////////////////
        calcJointIK(i);     // IK
        ///////////////////////////////////////////////

        //
        for (int j=0; j<framesData[i].jnum; j++) {
            if (jdata[j].joint>=0) {
                prevJoint[j] = jdata[j];
            }
        }

        //
        rotQuat[VMD_L_WRIST] = rotQuat[VMD_L_WRIST]*rotQuat[VMD_L_WRIST_TW];
        rotQuat[VMD_L_ELBOW] = rotQuat[VMD_L_ELBOW]*rotQuat[VMD_L_ARM_TW];
        rotQuat[VMD_R_WRIST] = rotQuat[VMD_R_WRIST]*rotQuat[VMD_R_WRIST_TW];
        rotQuat[VMD_R_ELBOW] = rotQuat[VMD_R_ELBOW]*rotQuat[VMD_R_ARM_TW];

        rotQuat[VMD_L_EYE]   = rotQuat[VMD_L_EYE]*rotQuat[VMD_EYES];
        rotQuat[VMD_R_EYE]   = rotQuat[VMD_R_EYE]*rotQuat[VMD_EYES];

        /////// A -> T
        rotQuat[VMD_L_WRIST] =  A2TPose*rotQuat[VMD_L_WRIST]*~A2TPose;
        rotQuat[VMD_L_ELBOW] =  A2TPose*rotQuat[VMD_L_ELBOW]*~A2TPose;      
        rotQuat[VMD_L_ARM]   =  rotQuat[VMD_L_ARM]*~A2TPose;

        rotQuat[VMD_R_WRIST] = ~A2TPose*rotQuat[VMD_R_WRIST]*A2TPose;
        rotQuat[VMD_R_ELBOW] = ~A2TPose*rotQuat[VMD_R_ELBOW]*A2TPose;
        rotQuat[VMD_R_ARM]   =  rotQuat[VMD_R_ARM]*A2TPose;
        
        // 足首の回転は省略
        rotQuat[VMD_L_ANKLE_IK].set(1.0, 0.0, 0.0, 0.0, 1.0);
        rotQuat[VMD_R_ANKLE_IK].set(1.0, 0.0, 0.0, 0.0, 1.0);

        //rotQuat[VMD_CENTER]   = rotQuat[VMD_CENTER]*~rotQuat[VMD_PARENT];
        //posVect[VMD_CENTER]   = posVect[VMD_CENTER] + posVect[VMD_PARENT];
                
        //
        for (int j=0; j<framesData[i].jnum; j++) {
            jdata[j].joint = prevJoint[j].joint;
            jdata[j].index = prevJoint[j].index;
            jdata[j].vect  = posVect[j];
            jdata[j].quat  = rotQuat[j];
        }
    }
    
    return;
}



void  CNiVMDTool::calcJointIK(int fnum)
{
    // 軸
    Vector<double> vect_Y(0.0, 1.0, 0.0, 1.0);
    Vector<double> vect_Z(0.0, 0.0,-1.0, 1.0);
    Vector<double> cnt_vect_Y = VectorRotation(vect_Y,  rotQuat[VMD_CENTER]);
    Vector<double> cnt_vect_Z = VectorRotation(vect_Z,  rotQuat[VMD_CENTER]);
    
    // 足の方向
    Vector<double> l_leg_vect = VectorRotation(cnt_vect_Z, rotQuat[VMD_L_HIP]);
    Vector<double> r_leg_vect = VectorRotation(cnt_vect_Z, rotQuat[VMD_R_HIP]);

    // エフェクタ: センターからの初期相対位置
    Vector<double> l_ank_vect =  vect_Y*0.10  + vect_Z*0.83;
    Vector<double> r_ank_vect = -vect_Y*0.10  + vect_Z*0.83;
    
    // エフェクタ: 移動後のセンターからの相対位置
    Vector<double> l_end_vect =  cnt_vect_Y*0.10 + l_leg_vect*0.83;
    Vector<double> r_end_vect = -cnt_vect_Y*0.10 + r_leg_vect*0.83;

    // 到達目標
    Vector<double> l_ik_vect = l_ank_vect + posVect[VMD_L_ANKLE_IK] - posVect[VMD_CENTER] - l_end_vect;
    Vector<double> r_ik_vect = r_ank_vect + posVect[VMD_R_ANKLE_IK] - posVect[VMD_CENTER] - r_end_vect;

    // 到達目標（ローカル座標系）
    l_ik_vect = VectorRotation(l_ik_vect, ~rotQuat[VMD_CENTER]);
    r_ik_vect = VectorRotation(r_ik_vect, ~rotQuat[VMD_CENTER]);

    //
    Vector<double> l_vect[3], r_vect[3];
    Quaternion<double> l_quat[3], r_quat[3];

    // 膝，腰の相対位置
    l_vect[0].set(0.0, 0.0, 0.0); 
    r_vect[0].set(0.0, 0.0, 0.0); 

    l_vect[1] = -l_leg_vect*0.58;
    r_vect[1] = -r_leg_vect*0.58;
    l_vect[2] = -l_leg_vect*1.18;
    r_vect[2] = -r_leg_vect*1.18;
    
    // ローカル座標系
    l_vect[1] = VectorRotation(l_vect[1], ~rotQuat[VMD_CENTER]);
    r_vect[1] = VectorRotation(r_vect[1], ~rotQuat[VMD_CENTER]);
    l_vect[2] = VectorRotation(l_vect[2], ~rotQuat[VMD_CENTER]);
    r_vect[2] = VectorRotation(r_vect[2], ~rotQuat[VMD_CENTER]);

    ///////////////////////////////////////////////////////////////////
    calcLegIK_CCD(l_vect, l_ik_vect, l_quat, 50); 
    calcLegIK_CCD(r_vect, r_ik_vect, r_quat, 50); 
    ///////////////////////////////////////////////////////////////////

    // HIP
    if (rotQuat[VMD_L_HIP].c>0.0) rotQuat[VMD_L_HIP] = l_quat[2]*rotQuat[VMD_L_HIP];
    else                          rotQuat[VMD_L_HIP] = l_quat[2];
    if (rotQuat[VMD_R_HIP].c>0.0) rotQuat[VMD_R_HIP] = r_quat[2]*rotQuat[VMD_R_HIP];
    else                          rotQuat[VMD_R_HIP] = r_quat[2];

    framesData[fnum].jdat[VMD_L_HIP].joint = VMD_L_HIP;
    framesData[fnum].jdat[VMD_R_HIP].joint = VMD_R_HIP;
    framesData[fnum].jdat[VMD_L_HIP].index = fnum;
    framesData[fnum].jdat[VMD_R_HIP].index = fnum;

    // KNEE
    rotQuat[VMD_L_KNEE] = l_quat[1];
    rotQuat[VMD_R_KNEE] = r_quat[1];
    //
    double dot = vect_Y*rotQuat[VMD_L_KNEE].getVector();
    if (dot>=0.0) {
        framesData[fnum].jdat[VMD_L_KNEE].joint = VMD_L_KNEE;
        framesData[fnum].jdat[VMD_L_KNEE].index = fnum;
    }
    else {
        framesData[fnum].jdat[VMD_L_KNEE].joint = -1;
        framesData[fnum].jdat[VMD_L_KNEE].index = -1;
    }
    
    dot = vect_Y*rotQuat[VMD_R_KNEE].getVector();
    if (dot>=0.0) { 
        framesData[fnum].jdat[VMD_R_KNEE].joint = VMD_R_KNEE;
        framesData[fnum].jdat[VMD_R_KNEE].index = fnum;
    }
    else {
        framesData[fnum].jdat[VMD_R_KNEE].joint = -1;
        framesData[fnum].jdat[VMD_R_KNEE].index = -1;
    }
    
    return;
}



/**
HIP(2) -> KNEE(1) -> ANKLE(0) の IK をCCDで解く．
*/
void  CNiVMDTool::calcLegIK_CCD(Vector<double>* vect, Vector<double> ik_vect, Quaternion<double>* quat, int rpmax)
{
    Vector<double> vect_Y(0.0, 1.0, 0.0, 1.0);
    Vector<double> effect, target;
    Quaternion<double> rot;

    for (int j=0; j<3; j++) quat[j].init();

    for (int i=0; i<rpmax; i++) {
        //
        int cnt = 0;
        for (int j=1; j<3; j++) {
            //
            if (i==0 && j==1) {
                rot.setRotation((double)PI_DIV2, vect_Y);       // 最初にひざを曲げる
            }
            else {
                effect = vect[0] - vect[j];
                target = ik_vect - vect[j];
                rot = V2VQuaternion(effect, target);
            }

            //
            if (rot.c>0.0) {
                rot.normalize();
                if (rot.s<1.00-Zero_Eps) {
                //if (rot.s<1.00) {
                    quat[j] = quat[j]*rot;
                    for (int k=0; k<j; k++) {
                        vect[k] = VectorRotation(vect[k]-vect[j], rot) + vect[j];
                    }
                }
                else cnt++;
            }
            else cnt++;
        }
        if (cnt==2) break;
    }

    return;
}



/**
NiJointData*  CNiVMDTool::getJointsData(int frmnum, int fps)

動的にジョイントデータを計算する場合に使用する．@n
getJointsFrame で返したデータが NULLの場合，代わりにこの関数が呼ばれる．

フレーム frmnum時の 全ジョイントのデータを得る．戻り値は配列．@n
frmnumのデータが存在しない場合は，補間する．

@param frmnum  フレーム数
@param fps     FPS. 
*/
NiJointData*  CNiVMDTool::getJointsData(int frmnum, int fps)
{
    clearJointsData(joints_num);
    
    frmnum = (int)((double)frmnum*VMD_FARME_RATE/(double)fps);

    //
    for (int j=0; j<joints_num; j++) {
        //
        int n = framesData[0].jdat[j].joint;        
        if (n>=0) {
            unsigned int st_index = 0;
            unsigned int en_index = 0;

            while (en_index<frames_num && framesData[en_index].frmn<frmnum) en_index++;
            if (en_index==frames_num) break;

            if (en_index==0 || (framesData[en_index].frmn==frmnum && en_index==framesData[en_index].jdat[j].index)) {
                jointsData[j].vect = framesData[en_index].jdat[j].vect;
                jointsData[j].quat = framesData[en_index].jdat[j].quat;
            }
            else {
                st_index = framesData[en_index-1].jdat[j].index;
                while (en_index<frames_num && en_index!=framesData[en_index].jdat[j].index) en_index++;

                if (en_index==frames_num) {
                    jointsData[j].vect = framesData[st_index].jdat[j].vect;
                    jointsData[j].quat = framesData[st_index].jdat[j].quat;
                }
                else {
                    NiJointData st_joint = framesData[st_index].jdat[j];
                    NiJointData en_joint = framesData[en_index].jdat[j];
                    double tparam = ((double)(frmnum - framesData[st_index].frmn))/(framesData[en_index].frmn - framesData[st_index].frmn);
                    jointsData[j].vect = BSplineInterp4 (st_joint.vect, en_joint.vect, tparam);
                    jointsData[j].quat = SlerpQuaternion(st_joint.quat, en_joint.quat, tparam);
                }
            }

            jointsData[j].joint  = n;
            jointsData[j].index  = frmnum;
            jointsData[j].vect.c = 1.0;
            jointsData[j].quat.c = 1.0;
        }
    }

    return jointsData;
}



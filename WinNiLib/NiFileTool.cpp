
#include  "WinBaseLib.h"
#include  "NiFileTool.h"
#include  "MFCio.h"
#include  "MessageBoxDLG.h"


using namespace jbxl;
using namespace jbxwl;



CNiFileTool::~CNiFileTool(void)
{ 
    DEBUG_INFO("DESTRUCTOR: CNiFileTool\n");

    free_data();
}



////////////////////////////////////////////////////////////////////
//

void  CNiFileTool::free_data(void)
{
    clear_data();
    
    if (JTXT_Tool!=NULL) delete(JTXT_Tool);
    if (VMD_Tool!=NULL)  delete(VMD_Tool);
    if (BVH_Tool!=NULL)  delete(BVH_Tool);

    JTXT_Tool = NULL;
    VMD_Tool  = NULL;
    BVH_Tool  = NULL;
}



void  CNiFileTool::clear_data(void)
{
    close_wfile(); 
    close_rfile();
    clearFrameData();

    clearStartPosition();
    clearJointsData();

    frames_data     = NULL;
    frame_start     = 0;
    frames_num      = 0;
    frame_ptr       = 0;

    frame_timer     = 0;
    lap_timer       = 0;
    time_scale      = 1.0;

    playback_time   = 0.0;
    playback_fps    = 30;
}



void  CNiFileTool::init_data(void)
{
    m_enable_wfile  = FALSE;
    m_wfp           = NULL;
    m_rfp           = NULL;
    m_lib           = NiSDK_None;

    frames_data     = NULL;
    frame_start     = 0;
    frames_num      = 0;
    frame_ptr       = 0;

    frame_timer     = 0;
    lap_timer       = 0;
    time_scale      = 1.0;

    playback_time   = 0.0;
    playback_fps    = 30;

    clearStartPosition();
    clearJointsData();

    JTXT_Tool       = NULL;
    VMD_Tool        = NULL;
    BVH_Tool        = NULL;
}



void  CNiFileTool::init(NiSDK_Lib lib)
{
    m_lib = lib;
    
    //
    JTXT_Tool = new CNiJTextTool();
    VMD_Tool  = new CNiVMDTool();
    BVH_Tool  = new CNiBVHTool();
    
    frameTool = NULL;
}



BOOL  CNiFileTool::open_wfile(LPCTSTR fname, BOOL force)
{
    if (m_enable_wfile) return TRUE;
    if (m_wfp!=NULL) return FALSE;

    if (!force) {
        if (file_exist_t(fname)) {
            int ret = MessageBoxDLG(IDS_STR_WARN, IDS_STR_ASK_OVERWRITE, MB_YESNO, NULL);
            if (ret==IDNO) return FALSE;
        }
    }

    //
    m_wfp = tfopen(fname, _T("wb"));
    if (m_wfp==NULL) return FALSE;

    m_enable_wfile = TRUE;
    return TRUE;
}



void  CNiFileTool::write_header(FILE* fp)
{
    if (fp==NULL) fp = m_wfp;

    JTXT_Tool->writeHeader(fp);
}



BOOL  CNiFileTool::open_rfile(LPCTSTR fname)
{
    if (m_rfp!=NULL) return FALSE;

    m_rfp = tfopen(fname, _T("rb"));
    if (m_rfp==NULL) {
        return FALSE;
    }

    return TRUE;
}



void  CNiFileTool::close_wfile(void)
{
    if (m_wfp!=NULL) {
        fclose(m_wfp);
        m_wfp = NULL;
        m_enable_wfile = FALSE;
    }
}



void  CNiFileTool::close_rfile(void)
{
    if (m_rfp!=NULL) {
        fclose(m_rfp);
        m_rfp = NULL;
    }
}




///////////////////////////////////////////////////////////////////////////////////////////////////
// JointsText I/O

int  CNiFileTool::readJTextFile(LPCTSTR fname)
{
    if (fname==NULL) return -1;
    clearFrameData();
    if (JTXT_Tool==NULL) JTXT_Tool = new CNiJTextTool();

    BOOL ret = open_rfile(fname);
    if (!ret) return -2;

    ret = JTXT_Tool->readFile(m_rfp);
    close_rfile();
    if (!ret) return -3;

    frames_data = JTXT_Tool->getFramesData();
    if (frames_data==NULL) return -4;

    frames_num = JTXT_Tool->getFramesNumber();
    frameTool = JTXT_Tool;

    playback_time = (frames_data[frames_num-1].msec - frames_data[0].msec)/1000.f;
    playback_fps  = JTXT_Tool->getPlayBackFPS();

    return frames_num;
}



BOOL  CNiFileTool::writeJTextData(FILE* fp, Vector<double>* pos, Quaternion<double>* rot, double* agl, BOOL mirror, NiSDK_Lib lib)
{
    if (fp==NULL) return FALSE;

    if (JTXT_Tool==NULL) JTXT_Tool = new CNiJTextTool();

    if (lib==NiSDK_Default) lib = m_lib;
    JTXT_Tool->setPosVect(pos, lib, mirror);
    JTXT_Tool->setRotQuat(rot, lib, mirror);
    JTXT_Tool->setJntAngl(agl, lib, mirror);
    JTXT_Tool->writeCurrentData(fp, GetMsecondsTimer());

    return TRUE;
}



// 一時データを JointsTextに変換（コピー）
BOOL  CNiFileTool::saveJTextfromFile(LPCTSTR tempf, LPCTSTR fname, BOOL force)
{
    if (!force) {
        if (file_exist_t(fname)) {
            int ret = MessageBoxDLG(IDS_STR_WARN, IDS_STR_ASK_OVERWRITE, MB_YESNO, NULL);
            if (ret==IDNO) return FALSE;
        }
    }

    return copyFileWithCounter(tempf, fname);
}



// 一時データの作成
BOOL  CNiFileTool::writeTempJText(Vector<double>* pos, Quaternion<double>* rot, double* agl, BOOL mirror, NiSDK_Lib lib)
{
    return writeJTextData(m_wfp, pos, rot, agl, mirror, lib);
}




///////////////////////////////////////////////////////////////////////////////////////////////////
// BVH I/O

int  CNiFileTool::readBVHFile(LPCTSTR fname)
{
    if (fname==NULL) return -1;
    clearFrameData();
    if (BVH_Tool==NULL) BVH_Tool = new CNiBVHTool();

    BOOL ret = open_rfile(fname);
    if (!ret) return -2;

    ret = BVH_Tool->readFile(m_rfp);
    close_rfile();
    if (!ret) return -3;

    frames_data = BVH_Tool->getFramesData();
    if (frames_data==NULL) return -4;

    frames_num = BVH_Tool->getFramesNumber();
    frameTool  = BVH_Tool;

    playback_time = (frames_data[frames_num-1].msec - frames_data[0].msec)/1000.f;
    playback_fps  = BVH_Tool->getPlayBackFPS();

    return frames_num;
}



// 一時データを BVHに変換
BOOL  CNiFileTool::saveBVHfromFile(LPCTSTR tempf, LPCTSTR fname, BVHSaveParam param)
{
    int frn = readJTextFile(tempf);
    if (frn<=0) return FALSE;

    param.scale /= 0.0254f; // m -> inch
    BOOL ret = writeBVHFile(fname, param);

    return ret;
}


// 
BOOL  CNiFileTool::writeBVHFile(LPCTSTR fname, BVHSaveParam param)
{
    if (frameTool==NULL) return FALSE;
    if (frameTool->getFramesNumber()<=0) return FALSE;


    //
    // HIERARCHY
    //
    tTree* hrchy = NULL;
    if (param.format==BVH_SAVE_FORMAT_QAV) {
        hrchy = makeBVH_QAvHierarchy();
    }
    else if (param.format==BVH_SAVE_FORMAT_SL) {
        hrchy = makeBVH_SLHierarchy();
    }
    else if (param.format==BVH_SAVE_FORMAT_NI) {
        hrchy = makeBVH_NiHierarchy();
    }
    else {
        return FALSE;
    }
    if (param.fps<10)      param.fps = 10;
    else if (param.fps>60) param.fps = 60;

    double frmitm = 1000.0/param.fps;       // msec
    int    frmnum = (int)(frameTool->getPlayBackTime()/frmitm) + 1;

    //
    CBVHTool* bvh_tool = new CBVHTool();

    bvh_tool->setHierarchy(hrchy);
    bvh_tool->frame_num  = frmnum;
    bvh_tool->frame_time = frmitm/1000.0;   // sec
    bvh_tool->state      = 0;

    if (bvh_tool->frame_time!=0.0) bvh_tool->framepsec = (int)(1.0/bvh_tool->frame_time);
    else                           bvh_tool->framepsec = 30;

    //
    size_t len = sizeof(vector)*bvh_tool->joint_num;
    bvh_tool->offset = (vector*)malloc(len);
    if (bvh_tool->offset==NULL) {
        delete(bvh_tool);
        return FALSE;
    }
    memset(bvh_tool->offset, 0, len);

    len = sizeof(double)*frmnum*bvh_tool->channels;
    bvh_tool->motion = (double*)malloc(len);
    if (bvh_tool->motion==NULL) {
        delete(bvh_tool);
        return FALSE;
    }
    memset(bvh_tool->motion, 0, len);

    //
    // MOTION
    //
    double r2d = 180.0/PI;
    tList* namelist = setBVHJointName();

    Vector<double>     vt[NI_TOTAL_JOINT_NUM];
    Quaternion<double> qt[NI_TOTAL_JOINT_NUM];

    //
    for (int f=0; f<frmnum; f++) {
        //
        int frm = f*bvh_tool->channels;
        NiJointData* jdat = frameTool->getJointsData(f, param.fps);

        for (int j=0; j<NI_TOTAL_JOINT_NUM; j++) {
            vt[j].set(0.0, 0.0, 0.0, 0.0, -1.0);
            qt[j].set(1.0, 0.0, 0.0, 0.0, 0.0, -1.0);
        }
        for (int j=0; j<frameTool->getJointsNumber(); j++) {
            int n = jdat[j].joint;
            if (n>=0 && n<NI_TOTAL_JOINT_NUM) {
                vt[n] = jdat[j].vect;
                qt[n] = jdat[j].quat;
            }
        }
        //
        if (param.recalc) Vector2Quaternion(vt, qt);

        //
        for (int j=0; j<bvh_tool->joint_num; j++) {
            int   chnum = bvh_tool->channel_num[j];
            int   index = bvh_tool->channel_idx[j];
            char* tpchr = (char*)&(bvh_tool->flex_joint.buf[index*2]);

            //
            Vector<double> vect(0.0, 0.0, 0.0, 0.0, -1.0);
            Vector<double> eulr(0.0, 0.0, 0.0, 0.0, -1.0);
            
            int jnum = BVHJoint2NiJointNum(namelist, bvh_tool->joint_name[j]);  // 共通ジョイント番号
            if (jnum>=0 && jnum<NI_TOTAL_JOINT_NUM) {       
                vect = vt[jnum];
                eulr = Quaternion2ExtEulerXYZ(qt[jnum]);
            }

            //
            for (int k=0; k<chnum; k++) {
                int midx = frm + index;
                if (tpchr[k*2]=='P') {
                    if      (tpchr[k*2+1]=='X') bvh_tool->motion[midx+k] = vect.y*param.scale;
                    else if (tpchr[k*2+1]=='Y') bvh_tool->motion[midx+k] = vect.z*param.scale;
                    else if (tpchr[k*2+1]=='Z') bvh_tool->motion[midx+k] = vect.x*param.scale;
                }
                else if (tpchr[k*2]=='R') { // "RYRXRZ" -> ZXY -> XYZ
                    if      (tpchr[k*2+1]=='X') bvh_tool->motion[midx+k] = eulr.y*r2d;
                    else if (tpchr[k*2+1]=='Y') bvh_tool->motion[midx+k] = eulr.z*r2d;
                    else if (tpchr[k*2+1]=='Z') bvh_tool->motion[midx+k] = eulr.x*r2d;
                }
            }
        }
    }

    //
    // OFFSET
    //
    for (int j=0; j<bvh_tool->joint_num; j++) {
        bvh_tool->offset[j].x = 0.0;
        bvh_tool->offset[j].y = 0.0;
        bvh_tool->offset[j].z = 0.0;
    }
    
    // BVH_SAVE_FORMAT_QAV
    if (param.format==BVH_SAVE_FORMAT_QAV) {
        for (int j=0; j<bvh_tool->joint_num; j++) {
            bvh_tool->offset[j] = getQAvBVHOffset(j);
        }
    }
    
    // 0フレームのジョイントの位置
    else {
        NiJointData* jdat = frameTool->getJointsData(0, param.fps);
        //
        for (int j=0; j<NI_TOTAL_JOINT_NUM; j++) {
            vt[j].set(0.0, 0.0, 0.0, 0.0, -1.0);
            qt[j].set(1.0, 0.0, 0.0, 0.0, 0.0, -1.0);
        }
        for (int j=0; j<frameTool->getJointsNumber(); j++) {
            int n = jdat[j].joint;
            if (n>=0 && n<NI_TOTAL_JOINT_NUM) {
                vt[n] = jdat[j].vect;
                qt[n] = jdat[j].quat;
            }
        }

        //
        for (int j=0; j<bvh_tool->joint_num; j++) {
            int jnum = BVHJoint2NiJointNum(namelist, bvh_tool->joint_name[j]);
            if (jnum>=0 && jnum<NI_TOTAL_JOINT_NUM) {
                bvh_tool->offset[j].x = vt[jnum].y*param.scale;
                bvh_tool->offset[j].y = vt[jnum].z*param.scale;
                bvh_tool->offset[j].z = vt[jnum].x*param.scale;
            }
        }   
    }
    
    //
    // 書き出し
    //
    BOOL ret = FALSE;

    if (param.divtm>0) {
        char* fn = ::ts2mbs(fname);
        ret = bvh_tool->writeMultiFile(fn, param.divtm, Max(param.divtm, 10));
        ::free(fn);
    }
    else {
        //
        ret = open_wfile(fname, FALSE);
        if (ret) {
            ret = bvh_tool->printBVH(m_wfp);
            close_wfile();
        }
        else {
            DEBUG_ERROR("CNiFileTool::writeBVHFile(): ERROR: Already other file for write is opened!\n");
        }
    }

    del_tList(&namelist);
    delete(bvh_tool);

    return ret;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
// MMD VMD I/O

int  CNiFileTool::readVMDFile(LPCTSTR fname)
{
    if (fname==NULL) return -1; 
    clearFrameData();
    if (VMD_Tool==NULL) VMD_Tool = new CNiVMDTool();

    BOOL ret = open_rfile(fname);
    if (!ret) return -2;

    ret = VMD_Tool->readFile(m_rfp);
    close_rfile();
    if (!ret) return -3;

    //
    frames_data = VMD_Tool->getFramesData();
    if (frames_data==NULL) return -4;

    frames_num = VMD_Tool->getFramesNumber();
    frameTool  = VMD_Tool;

    playback_time = (frames_data[frames_num-1].msec - frames_data[0].msec)/1000.f;
    playback_fps  = VMD_Tool->getPlayBackFPS();

    return frames_num;
}



// 一時データを VMDに変換
BOOL  CNiFileTool::saveVMDfromFile(LPCTSTR tempf, LPCTSTR fname)
{
    // ここに実装... しないと思う．面倒だから．
    return FALSE;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
// Frame

void  CNiFileTool::setFrame(int frame) 
{ 
    frame_start = 0;
    if (frames_num!=0) {
        frame_start = frame%frames_num;
    }

    frame_ptr = frame_start;
}



//
// Start 位置の設定とフレームデータの準備
//
BOOL  CNiFileTool::startFrame(int start)
{
    if (frames_num==0 || start<0 || start>=frames_num) return FALSE;

    lap_timer   = GetMsecondsTimer();
    frame_timer = (int)(frames_data[start].msec*time_scale);
    setFrame(start);

    return TRUE;
}



//
// 次のフレームデータの準備
//
BOOL  CNiFileTool::nextFrame(BOOL next)     // next==TRUE フレームを進める
{
    if (next) {
        frame_ptr++;
        if (frame_ptr>frames_num) {
            // not reachable
            DEBUG_INFO("CNiFileTool::nextFrame(): frame ptr = %d, frame num = %d\n", frame_ptr, frames_num); 
            return FALSE;
        }
    }

    //
    int msec = 0;
    if (frame_ptr==frames_num) {
        if (frames_num!=1) {
            msec = (int)((2*frames_data[frame_ptr-1].msec - frames_data[frame_ptr-2].msec)*time_scale);
        }
    }
    else {
        msec = (int)(frames_data[frame_ptr].msec*time_scale);
    }

    //
    unsigned short ctime;
    frame_timer += GetMsecondsLapTimer(lap_timer, &ctime);
    lap_timer = ctime;
    
    if (frame_timer>=msec) {
        return TRUE;
    }
    else if (msec>frame_timer+5000) {
        DEBUG_INFO("CNiFileTool::nextFrame(): next frame = %d, current frame = %d\n", msec, frame_timer);
        return FALSE;   // 5sec 進?
    }
    
    // Wait
    do {
        ::DisPatcher();
        int slptm = Min(msec-frame_timer, 100);
        if (slptm>0) ::Sleep(slptm);
        frame_timer += GetMsecondsLapTimer(lap_timer, &ctime);
        lap_timer = ctime;
    } while (msec>frame_timer);
    
    return TRUE;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
// Frame Data

void  CNiFileTool::clearFrameData(void)
{
    //
    frames_data = NULL;
    frames_num  = 0;
    frame_ptr   = 0;

    if (JTXT_Tool!=NULL) JTXT_Tool->clear_data();
    if (BVH_Tool!=NULL)   BVH_Tool->clear_data();
    if (VMD_Tool!=NULL)   VMD_Tool->clear_data();

    frameTool  = NULL;

    return;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
// joints

void  CNiFileTool::clearJointsData(void)
{
    for (int i=0; i<NI_TOTAL_JOINT_NUM; i++) {
        rotQuat[i].init(-1.0);
        posVect[i].init(-1.0);
    }
    currentPos.init(-1.0);
}



void  CNiFileTool::getJointsDataSeq(BOOL mirror, int frame)
{
    if (frame>=0) setFrame(frame);

    // 静的なデータ
    NiJointData* joint_data = frames_data[frame_ptr].jdat;
    int          joint_num  = frames_data[frame_ptr].jnum;

    // joint_data が無効なら，動的にデータを計算（補間）して得る．
    if (joint_data==NULL) {
        if (frameTool!=NULL) {
            joint_data = frameTool->getJointsData(frame_ptr, playback_fps);
        }
    }
    if (joint_data==NULL) return;

    //
    for (int j=0; j<joint_num; j++) {
        //
        int n = joint_data[j].joint;
        if (n>=0 && n<NI_TOTAL_JOINT_NUM) {
            if (mirror) n = NiMirrorJointNum(n);
            //
            posVect[n] = joint_data[j].vect;
            rotQuat[n] = joint_data[j].quat;
            //
            if (mirror) {
                posVect[n].y = - posVect[n].y;
                rotQuat[n].x = - rotQuat[n].x;
                rotQuat[n].z = - rotQuat[n].z;
            }
        }
    }
    return;
}


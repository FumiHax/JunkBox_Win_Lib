
#include  "tools++.h"
#include  "NiJTextTool.h"


using namespace jbxl;
using namespace jbxwl;


//////////////////////////////////////////////////////////////////////////////////////////////////
// CNiJTextTool Class

CNiJTextTool::CNiJTextTool(void)
{
    posVect = (Vector<double>*)malloc(sizeof(Vector<double>)*NI_TOTAL_JOINT_NUM);
    rotQuat = (Quaternion<double>*)malloc(sizeof(Quaternion<double>)*NI_TOTAL_JOINT_NUM);
    jntAngl = NULL;
}



CNiJTextTool::~CNiJTextTool(void)
{
    DEBUG_INFO("DESTRUCTOR: CNiJTextTool\n");

    free_data();
}



void  CNiJTextTool::free_data(void)
{
    CBaseFrameTool::free_data();

    ::freeNull(jntAngl);
}



void  CNiJTextTool::clear_data(void)
{
    free_data();
}



///////////////////////////////////////////////////////////////////
//

BOOL  CNiJTextTool::readFile(FILE* fp)
{
    if (fp==NULL) return FALSE;

    clear_data();

    Buffer buf = make_Buffer(LDATA);    // Line 読み込み用バッファ

    // ファイルヘッダのチェック
    fgets_Buffer(&buf, fp);
    if (strncmp(NI_JTXT_FILE_ID, (char*)buf.buf, strlen(NI_JTXT_FILE_ID))) {
        free_Buffer(&buf);
        return FALSE;
    }

    // フレーム数を数える
    int frames = 0;
    fgets_Buffer(&buf, fp);
    while(!feof(fp)) {
        if (buf.buf[0]>='0' && buf.buf[0]<='9') frames++; // 行頭が数字の場合はフレーム時間
        fgets_Buffer(&buf, fp);
    }
    if (frames==0) {
        free_Buffer(&buf);
        return FALSE;
    }
    frames_num = frames;

    // 各フレームの開始時間を調べる
    int* frame_msec = (int*)malloc(frames_num*sizeof(int));
    memset(frame_msec, 0, frames_num*sizeof(int));

    fseek(fp, 0, SEEK_SET);
    fgets_Buffer(&buf, fp);
    // 最初のフレーム時間行まで移動
    while (buf.buf[0]<'0' || buf.buf[0]>'9') fgets_Buffer(&buf, fp);

    int joints = 0;
    for (unsigned int i=0; i<frames_num; i++) {
        frame_msec[i] = atoi((char*)buf.buf);
        //
        int jnum = 0;
        fgets_Buffer(&buf, fp);
        while (!feof(fp) && (buf.buf[0]<'0' || buf.buf[0]>'9')) {
            if (buf.buf[0]==' ') jnum++;
            fgets_Buffer(&buf, fp);
        }
        joints = Max(joints, jnum);
    }
    joints_num = joints;

    //
    // データ領域の確保
    //
    jointsData = (NiJointData*)malloc(sizeof(NiJointData)*joints_num);
    if (jointsData==NULL) {
        clear_data();
        return FALSE;
    }
    clearJointsData(joints_num);

    // Frame Data の格納先を作る
    framesData = makeFramesData(frames_num, joints_num, frame_msec);
    ::free(frame_msec);
    if (framesData==NULL) {
        free_Buffer(&buf);
        clear_data();
        return FALSE;
    }

    if (posVect==NULL) posVect = (Vector<double>*)malloc(sizeof(Vector<double>)*NI_TOTAL_JOINT_NUM);
    if (rotQuat==NULL) rotQuat = (Quaternion<double>*)malloc(sizeof(Quaternion<double>)*NI_TOTAL_JOINT_NUM);
    if (posVect==NULL || rotQuat==NULL) {
        free_Buffer(&buf);
        clear_data();
        return FALSE;
    }
    clearVectorData(NI_TOTAL_JOINT_NUM);

    //
    // データの読み込み
    //
    BOOL tmline = FALSE;
    int  prvms = 0;
    fseek(fp, 0, SEEK_SET);

    char jname[L_ID];
    memset(jname, 0, L_ID);

    //
    fgets_Buffer(&buf, fp);

    for (unsigned int i=0; i<frames_num; i++) {
        // 次のフレーム時間行まで移動
        while (buf.buf[0]<'0' || buf.buf[0]>'9') fgets_Buffer(&buf, fp);

        int nowms = framesData[i].msec;
        if (i==0) prvms = nowms;
        int msec = nowms - prvms;
        if (msec<0) msec += 60000;  // + 60sec
        prvms = nowms;

        if (i==0) framesData[0].msec = 0;
        else      framesData[i].msec = framesData[i-1].msec + msec;
        framesData[i].frmn = framesData[i].msec;
        
        //
        double px, py, pz;
        double qx, qy, qz, qs, th;

        NiJointData* jdat = framesData[i].jdat;

        //
        for (int j=0; j<joints_num; j++) {
            fgets_Buffer(&buf, fp);
            while (buf.buf[0]!=' ') {
                if (buf.buf[0]>='0' || buf.buf[0]<='9') {   // ジョイントデータが途中で終わった?!
                    tmline = TRUE;          
                    break;
                }
                fgets_Buffer(&buf, fp);                     // コメント行の読み飛ばし
            }
            if (tmline) {
                tmline = FALSE;
                break;
            }

            //
            jname[0] = '\0';
            px = py = pz = 0.0;
            qx = qy = qz = 0.0;
            qs = 1.0;
            sscanf((char*)buf.buf, "%s %lf %lf %lf %lf %lf %lf %lf %lf", jname, &px, &py, &pz, &qx, &qy, &qz, &qs, &th);

            jdat[j].vect.set(px, py, pz);
            jdat[j].quat.set(qs, qx, qy, qz);
            jdat[j].angl  = th;
            jdat[j].joint = NiJointNum(jname);;
            jdat[j].index = i;
        }
    }

    //
    start_time = framesData[0].msec;
    stop_time  = framesData[frames_num-1].msec;
    exec_time  = stop_time - start_time;

    free_Buffer(&buf);

    return TRUE;
}



void  CNiJTextTool::writeCurrentData(FILE* fp, unsigned short msec)
{
    if (fp==NULL) return;
    if (posVect==NULL || rotQuat==NULL) return;

    fprintf(fp, "%d\n", msec);

    for (int j=0; j<NI_TOTAL_JOINT_NUM; j++) {
        if (posVect[j].c>0 || rotQuat[j].c>0) {
            std::string jn = NiJointName(j);
            fprintf(fp, " %-10s %11.6f %11.6f %11.6f  ",    jn.c_str(),   posVect[j].x, posVect[j].y, posVect[j].z);
            fprintf(fp, "    %11.8f %11.8f %11.8f %11.8f",  rotQuat[j].x, rotQuat[j].y, rotQuat[j].z, rotQuat[j].s);
            if (jntAngl!=NULL) fprintf(fp, "    %11.6f",    jntAngl[j]*RAD2DEGREE);
            fprintf(fp, "\n");
        }
    }

    return;
}



void  CNiJTextTool::setPosVect(Vector<double>* pos, NiSDK_Lib lib, BOOL mirror)
{
    if (posVect==NULL) posVect = (Vector<double>*)malloc(sizeof(Vector<double>)*NI_TOTAL_JOINT_NUM);
    if (posVect==NULL) return;

    //
    for (int j=0; j<NI_TOTAL_JOINT_NUM; j++) {
        int n = Ni2SDKJointNum(j, lib);
        if (mirror && n>=0) n = NiSDKMirrorJointNum(n, lib);

        if (n>=0) {
            posVect[j] = pos[n];
            if (mirror) posVect[j].y = -posVect[j].y;
            //posVect[j].c = 1.0;
        }
        else {
            posVect[j].init(-1.0);
        }
    }

    return;
}



void  CNiJTextTool::setRotQuat(Quaternion<double>* rot, NiSDK_Lib lib, BOOL mirror)
{
    if (rotQuat==NULL) rotQuat = (Quaternion<double>*)malloc(sizeof(Quaternion<double>)*NI_TOTAL_JOINT_NUM);
    if (rotQuat==NULL) return;

    //
    for (int j=0; j<NI_TOTAL_JOINT_NUM; j++) {
        int n = Ni2SDKJointNum(j, lib);
        if (mirror && n>=0) n = NiSDKMirrorJointNum(n, lib);

        if (n>=0) {
            rotQuat[j] = rot[n];
            if (mirror) {
                rotQuat[j].x = -rotQuat[j].x;
                rotQuat[j].z = -rotQuat[j].z;
            }
            //rotQuat[j].c = 1.0;
        }
        else {
            rotQuat[j].init(-1.0);
        }
    }

    return;
}



void  CNiJTextTool::setJntAngl(double* agl, NiSDK_Lib lib, BOOL mirror)
{
    if (agl==NULL) {
        ::freeNull(jntAngl);
        return;
    }

    if (jntAngl==NULL) jntAngl = (double*)malloc(sizeof(double)*NI_TOTAL_JOINT_NUM);
    if (jntAngl==NULL) return;

    //
    for (int j=0; j<NI_TOTAL_JOINT_NUM; j++) {
        int n = Ni2SDKJointNum(j, lib);
        if (mirror && n>=0) n = NiSDKMirrorJointNum(n, lib);

        if (n>=0) {
            jntAngl[j] = agl[n];
        }
        else {
            jntAngl[j] = 0.0;
        }
    }

    return;
}



NiJointData*  CNiJTextTool::getJointsData(int frmnum, int fps) 
{ 
    if (frmnum<0) return NULL;

    if (fps<=0) fps = 30;
    int msec = (int)(1000./fps*frmnum);
    if (msec>exec_time) return NULL;

    unsigned int f;
    double t = 0.0;

    for (f=1; f<frames_num; f++) {
        if (framesData[f].msec >= msec + start_time) {
            t = (msec - framesData[f-1].msec)/(double)(framesData[f].msec - framesData[f-1].msec);
            break;
        }
    }
    if (f>=frames_num) return NULL;

    //
    clearJointsData(joints_num);

    NiJointData* jdat1 = framesData[f-1].jdat;
    NiJointData* jdat2 = framesData[f].jdat;

    for (int j=0; j<joints_num; j++) {
        if (jdat1[j].joint>=0) {
            for (int k=0; k<joints_num; k++) {
                if (jdat1[j].joint==jdat2[k].joint) {
                    jointsData[j].joint = jdat1[j].joint;
                    jointsData[j].index = frmnum;
                    jointsData[j].vect  = BSplineInterp4 (jdat1[j].vect, jdat2[k].vect, t);
                    jointsData[j].quat  = SlerpQuaternion(jdat1[j].quat, jdat2[k].quat, t);
                }
            }
        }
    }
    
    return jointsData;
}


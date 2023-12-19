//

#include "MFCio.h"


using namespace jbxl;
using namespace jbxwl;


/**
int  jbxwl::copyFileWithCounter(char* src, char* dst)

    戻り値： 正数 正常終了
             -1   読み込みファイルが存在しない．またはサイズが 0．
             -2   読み込みファイルをオープンできない．
             -3   書き込みファイルをオープンできない．
　　　　　
     　　　copyFPWithCounter からの戻り値
             -10  処理失敗（何も処理されていない？）
             -11  キャンセル
             -12  読み込み/書き込みに失敗
             -13  指定されたバイト数まで処理されていない
*/
int  jbxwl::copyFileWithCounter(LPCTSTR src, LPCTSTR dst)
{
    unsigned long int sz = file_size_t(src);
    if (sz<=0) return -1;

    FILE* sfp = tfopen(src, _T("rb"));
    if (sfp==NULL) return -2;

    FILE* dfp = tfopen(dst, _T("wb"));
    if (dfp==NULL) {
        fclose(sfp);
        return -3;
    }

    int ret = copyFPWithCounter(sfp, dfp, sz);

    fclose(dfp);
    fclose(sfp);

    if (ret<=0) ret -= 10;
    return ret;
}



/**
int  jbxwl::copyFPWithCounter(FILE* src, FILE* dst, unsigned long int sz)

    戻り値： 正数 正常終了
              0   処理失敗（何も処理されていない？）
             -1   キャンセル
             -2 　読み込み/書き込みに失敗
             -3   指定されたバイト数まで処理されていない
*/
int  jbxwl::copyFPWithCounter(FILE* src, FILE* dst, unsigned long int sz)
{
    //
    // グローバルカウンタの生成
    CProgressBarDLG* counter = NULL;

    int count_max = 20;
    int count_intvl = sz/MAXBUFSZ/count_max;

    if (sz>MAXBUFSZ && count_intvl>0) { // 64kByte
        counter = new CProgressBarDLG(IDD_PROGBAR, NULL, TRUE); // ディスパッチャー有効
        if (counter!=NULL) { 
            SetGlobalCounter(counter);
            CString mesg;
            mesg.LoadString(IDS_STR_WRTNG_FILE);
            //
            char* mbstr = ts2mbs(mesg);
            counter->Start(0, mbstr);
            ::free(mbstr);
        }
    }

    if (counter!=NULL) {
        counter->SetMax(count_max);
        counter->SetPos(0);
    }

    //
    unsigned char buf[MAXBUFSZ];
    int sum = 0;
    int cnt = 0;
    size_t rsz = 0;
    size_t wsz = 0;

    //
    do {
        rsz = fread(buf, 1, MAXBUFSZ, src);
        wsz = fwrite(buf, rsz, 1, dst);
        sum += (int)rsz;
        
        cnt++;
        if (counter!=NULL) {
            if (cnt%count_intvl==0) {
                counter->StepIt();
                if (counter->isCanceled()) {
                    sum = -1;
                    break;  // キャンセル
                }
            }
        }
    } while (!feof(src) && rsz>0 && wsz>0);

    // グローバルカウンタの削除
    if (counter!=NULL) {
        counter->Stop();
        ClearGlobalCounter();
        delete counter;
    }

    //
    if (sum!=0) {
        if (!feof(src) && (rsz==0||wsz==0)) sum = -2;
        else if (sum!=sz) {
            DEBUG_ERROR("jbxwl::copyFPWithCounter(): WARNING: File Size is %d, but Write Size is %d\n", sz, sum);
            sum = -3;
        }
    }

    return sum;
}



int  jbxwl::writeGraphicFileWithCounter(LPCTSTR fname, MSGraph<sWord> vp, int frmt, BOOL mlt, int fn, int tn)
{
    // グローバルカウンタの生成
    CProgressBarDLG* counter = NULL;
    if (vp.zs>=10) {
        counter = new CProgressBarDLG(IDD_PROGBAR, NULL, TRUE); // ディスパッチャー有効
        if (counter!=NULL) { 
            SetGlobalCounter(counter);
            CString mesg;
            mesg.LoadString(IDS_STR_WRTNG_FILE);
            //
            char* mbstr = ts2mbs(mesg);
            counter->Start(0, mbstr);
            ::free(mbstr);
        }
    }

    //
    char* mbstr = ts2mbs(fname);
    int ret = writeGraphicFile(mbstr, vp, frmt, mlt, fn, tn, true);
    ::free(mbstr);

    // グローバルカウンタの削除
    if (counter!=NULL) {
        counter->Stop();
        ClearGlobalCounter();
        delete counter;
    }

    return ret;
}



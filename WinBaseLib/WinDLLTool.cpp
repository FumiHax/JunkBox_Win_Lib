/**
  Windows用 Tool

*/

#include  "..\stdafx.h"

#include  "tools++.h"
#include  "WinDLLTool.h"


using namespace jbxl;
using namespace jbxwl;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DLLModule
//
//        pgetinfo: char* (*pgetinfo)(int n) の関数へのポインタ．モジュールの情報を獲得するための関数．
//                  モジュールには (*pgetinfo)(0) で情報（タイトル）を返す関数が必ず必要．
//

/**
    機能：クラスの初期化．fnameに NULL または nに 0以下を指定した場合は set_funcname() で改めて指定しなければならない．

    引数：fname -- 読み込むDLL中の情報取得関数の名前
          n     -- 読み込むDLL中の使用できる関数の数
*/
void  DLLModule::init(LPCTSTR fname, int n)
{
    status = 0;
    fnum   = n;
    
    title    = NULL;
    dllname  = _T("");
    filename = _T("");

    if (fname!=NULL) funcname = fname;
    else             funcname = _T("");
    
    hmod     = NULL;
    pgetinfo = NULL;
    pfunc    = NULL;
    if (fnum>0) pfunc = (void**)malloc(sizeof(void*)*fnum);
}



/**
    機能：クラス中のデータを解放する．
*/
void  DLLModule::free()
{
    status = 0;

    hmod = NULL;
    pgetinfo = NULL;
    if (fnum>0 && pfunc!=NULL) ::free(pfunc);
    fnum = 0;
    pfunc = NULL;
}



/**
    機能：DLL中の情報取得関数の名前と，使用できる関数の数を指定する．
          fnameに NULL または nに 0以下を指定した場合，改めて指定しなければならない．

    引数：fname -- 読み込むDLL中の情報取得関数の名前
          n     -- 読み込むDLL中の使用できる関数の数

     戻り値：TRUE  -- 指定は完了した．
             FALSE -- 指定は完了していない．続行は保障されない．
*/
BOOL  DLLModule::set_funcname(LPCTSTR fname, int n)
{
    if (fname!=NULL) funcname = fname;
    else             funcname = _T("");
    
    if (fnum>0 && pfunc!=NULL) ::free(pfunc);
    pfunc = NULL;

    fnum = n;
    if (fnum>0) pfunc = (void**)malloc(sizeof(void*)*fnum);

    if (fname==NULL || n<=0) return FALSE;
    return TRUE;
}



/**
    機能：モジュールの一覧を格納したリストlp から，モジュール（一つだけ）の情報を獲得する．
          関数名fname で示した関数 fname(0) からタイトル名を取得する．
          fname(1) 〜 fname(fnum-1) からは使用できる関数名を得る．使用できる関数の型は，
          外部で決める．

    引数：lp -- モジュールの情報が入ったリストへのポインタ．
                lp->ldat.key : DLL名
                lp->ldat.val : DLLファイル名（ディレクトリを含む）
                lp->ldat.ptr : モジュールハンドラ

    戻り値：TRUE  -- 正常に情報を取得．
            FALSE -- 情報取得関数から，正しい情報を読み取れない．

*/
BOOL  DLLModule::get_module_info(tList* lp)
{
    if (lp==NULL) return FALSE;
    if (funcname==_T("")) return FALSE;

    status   = 0;
    hmod = (HMODULE)(lp->ldat.ptr);
    char* mbstr = ts2mbs(funcname);
    pgetinfo = (FUNC_STRING)GetProcAddress(hmod, mbstr);
    ::free(mbstr);
    if (pgetinfo==NULL) return FALSE;
    
    dllname  = mbs2ts((char*)(lp->ldat.key.buf));
    filename = mbs2ts((char*)(lp->ldat.val.buf));
    title    = (*pgetinfo)(0);

    pfunc[0] = (void*)pgetinfo;
    for (int i=1; i<fnum; i++) {
        char* nfn = (*pgetinfo)(i);
        void* pfn = (void*)GetProcAddress(hmod, nfn);
        pfunc[i] = pfn;
        //if (pfn==NULL) {
        //  status = -1;
        //  return FALSE;
        //}
    }

    status = 1;
    return TRUE;
}



/**
    機能：DLL中の n番目の関数へのポインタを得る．関数の型は外部で決める

    引数：求める関数の番号．0 〜 fnum-1

    戻り値：関数へのポインタ．
*/
void*  DLLModule::get_funcptr(int n)
{   
    if (status<=0) return NULL;

    if (n>=fnum) n = 0;
    return pfunc[n];
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DLLModuleTBL
//

/**
    機能：クラスの初期化．ディレクトリdirnにある DLLを自動的に読み込む．
          dirn に NULLを指定した場合は DLLを読み込まないので，load_module() を改めて実行しなければならない．

    引数：dirn -- 読み込むDLLがあるディレクトリ．通常はアプリケーションからの相対パス．

*/
void  DLLModuleTBL::init(LPCTSTR dirn)
{   
    tnum = vnum = 0;
    loaded  = false;
    lp      = NULL;
    dllmod  = NULL;
    dirname = _T("");
    
    if (dirn!=NULL) load_module(dirn);
}



/**
    機能：クラス中のデータを解放する．
*/
void DLLModuleTBL::free()
{
    
    if (dllmod!=NULL) {
        for (int i=0; i<vnum; i++) {
            if (dllmod[i]!=NULL) {
                dllmod[i]->free();
                delete(dllmod[i]);
                dllmod[i] = NULL;
            }
        }
    }

    tList* lt = lp;
    while (lt!=NULL) {
        if (lt->ldat.ptr!=NULL) {
            FreeLibrary((HMODULE)(lt->ldat.ptr));       // 関数へのハンドル
            lt->ldat.ptr = NULL;
            lt->ldat.sz  = 0;
        }
        lt = lt->next;
    }
    del_all_tList(&lp); 

    tnum = vnum = 0;
    if (dllmod!=NULL) ::free(dllmod);
    dllmod = NULL;
    loaded = false;
}



/**
    機能：ディレクトリdirnにある DLLを読み込む．DLLのチェック（使用できるものかどうか）は行わない．
          dirn に NULLを指定した場合は カレントディレクトリのDLLを読み込む．

    引数：dirn -- 読み込むDLLがあるディレクトリ．通常はアプリケーションからの相対パス．
                  NULL の場合は，カレント "."を指定したことになる．

    戻り値：TRUE  -- 読み込み完了．tnum に読み込んだDLLの数が入る．
            FLASE -- DLLは存在しない．または読み込み失敗．
*/
BOOL  DLLModuleTBL::load_module(LPCTSTR dirn)
{
    if (dirn!=NULL) dirname = dirn;
    else            dirname = _T(".");

    lp = get_dir_module((LPCTSTR)dirname);
    if (lp==NULL) return FALSE;

    tnum = 0;
    tList* lt = lp;
    while (lt!=NULL) {
        tnum++;
        lt = lt->next;
    }
    loaded = true;

    return TRUE;
}



/**
    機能：ディレクトリ dirn にあるファイルを検索して，DLLファイルなら読み込みを行いリストに格納する．
          DLLのチェックは行わない．

    引数：dirn -- 読み込むDLLがあるディレクトリ．通常はアプリケーションからの相対パス．
                  NULL の場合は，読み込みを行わない．
    
    戻り値：情報を格納したリストへのポインタ．エラーの場合は NULL．
                lp->ldat.key.buf : DLL名
                lp->ldat.val.buf : DLLファイル名（ディレクトリを含む）
                lp->ldat.ptr     : モジュールハンドラ
*/
tList*  DLLModuleTBL::get_dir_module(LPCTSTR dirn)
{
    tList* lm;
    tList* lt;

    if (dirn==NULL) return NULL;
    lm = lt = get_dir_files_t(dirn);
    if (lm==NULL) return NULL;

    while (lt!=NULL) {
        HMODULE hmod = LoadLibrary((LPCTSTR)mbs2ts((char*)(lt->ldat.val.buf)));
        lt->ldat.ptr = (void*)hmod;
        char* fn = get_file_name((char*)(lt->ldat.val.buf));
        lt->ldat.key = make_Buffer_bystr(fn);
        lt = lt->next;
    }

    // ハンドラが NULLの場合は削除
    lt = lm;
    while (lt!=NULL) {
        if (lt->ldat.ptr==NULL) {
            tList* ln = lt;
            if (lm==lt) lm = lt->next;
            lt = lt->next;
            del_tList_node(&ln);
        }
        else lt = lt->next;
    }

    return lm;
}



/**
    機能：モジュールの一覧を格納したリストlp から，モジュールテーブル（配列）を作成する．
          関数名fname で示した関数 fname(0) からタイトル名が獲得できない場合は，テーブル
          から削除する．

    引数：fname -- モジュールの情報を返す関数名．引数 (int)0 でタイトル(char*)を返えさなければならない．
          m     -- 読み込むDLL中の使用できる関数の数
*/
void  DLLModuleTBL::make_module_tbl(LPCTSTR fname, int m)
{
    tList* lm = lp;

    vnum = 0;
    if (fname==NULL) return;

    dllmod = (DLLModule**)malloc(sizeof(DLLModule*)*tnum);
    if (dllmod==NULL) return;

    int n = 0;
    if (m<=0) m = 1;
    while (lm!=NULL) {
        dllmod[n] = this->new_module(fname, m); 
        int ret = dllmod[n]->get_module_info(lm);
        if (ret) {
            n++;
            if (n>=tnum) break;
        }
        else {
            delete(dllmod[n]);
            dllmod[n] = NULL;
        }
        lm = lm->next;
    }
    vnum = n;

    this->set_module_function();
    return;
}




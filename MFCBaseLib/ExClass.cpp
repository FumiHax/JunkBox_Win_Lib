
#include  "ExClass.h"


using namespace jbxl;
using namespace jbxwl;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

/**
bool  jbxwl::SetExLink(CExDocument* pdoc, CExFrame* pfrm, CExView* pviw, CAppCallBack* papp)

各オブジェクト間の関連付けを行う．
*/
bool  jbxwl::SetExLink(CExDocument* pdoc, CExFrame* pfrm, CExView* pviw, CAppCallBack* papp)
{
    if (pdoc==NULL) return  false;
    if (pfrm==NULL) return  false;
    if (pviw==NULL) return  false;
    
    pdoc->pView  = pfrm->pView  = pviw;
    pdoc->pFrame = pviw->pFrame = pfrm;
    pviw->pDoc   = pfrm->pDoc   = pdoc;
    pviw->pApp   = pfrm->pApp   = pdoc->pApp = papp;
    return  true;
}


/**
CExFrame*  jbxwl::CreateDocFrmView(CMultiDocTemplate* pDocTempl, CAppCallBack* papp)

Document, Frame, View の組み合わせを作成する．

*/
CExFrame*  jbxwl::CreateDocFrmView(CMultiDocTemplate* pDocTempl, CAppCallBack* papp)
{
    // クラスの取得
    CExDocument* pdoc = (CExDocument*)pDocTempl->CreateNewDocument();
    CExFrame*    pfrm = (CExFrame*)pDocTempl->CreateNewFrame(pdoc, NULL);
    CExView*     pviw = (CExView*)pdoc->GetView();

    bool rslt = SetExLink(pdoc, pfrm, pviw, papp);
    if (!rslt) {
        if (pdoc!=NULL) {
            DEBUG_ERROR("jbxwl::CreateDocFrmView(): ERROR: Document is NULL\n");
            deleteNull(pdoc);
        }
        if (pviw!=NULL) {
            DEBUG_ERROR("jbxwl::CreateDocFrmView(): ERROR: View is NULL\n");
            deleteNull(pviw);
        }
        if (pfrm!=NULL) {
            DEBUG_ERROR("jbxwl::CreateDocFrmView(): ERROR: Frame is NULL\n");
            deleteNull(pfrm);
        }
        return  NULL;
    }
    pfrm->pTempl = pDocTempl;   

    return pfrm;
}


/**
BOOL  jbxwl::InitialDocView(CExFrame* pfrm, LPCTSTR fname)

Document, View クラスの前処理．@n
ファイルをオープンし，表示の準備をする．
*/
BOOL  jbxwl::InitialDocView(CExFrame* pfrm, LPCTSTR fname)
{
    BOOL  rslt;
    
    // Document クラスの前処理
    if (fname==NULL) {
        rslt = pfrm->pDoc->OnNewDocument();
    }
    else {
        rslt = pfrm->pDoc->OnOpenDocument(fname);
    }
    if (!rslt) return FALSE;

    // View クラスの前処理
    pfrm->pView->OnInitialUpdate();
    if (!pfrm->pView->hasViewData) return FALSE;
    return  TRUE;
}


/**
int  jbxwl::ExecDocFrmView(CExFrame* pfrm, LPCTSTR fname)

フレームに関連付けれたクラスを使用して，データ読み込み，表示を行う．@n
データの読み込みには InitialDocView() を使用する
*/
int  jbxwl::ExecDocFrmView(CExFrame* pfrm, LPCTSTR fname)
{
    if (pfrm ==NULL) return 1;
    if (fname!=NULL) pfrm->pDoc->Title = get_file_name_t(fname);

    //
    BOOL rslt = InitialDocView(pfrm, fname);
    if (!rslt) {
        int ret;
        // この場所でのメッセージ表示はしない．
        if      ( pfrm->doneErrorMessage)   ret = MSG_DFV_NOT_DISP;
        else if ( pfrm->cancelOperation)    ret = MSG_DFV_CANCEL;
        else if (!pfrm->pDoc->hasReadData)  ret = MSG_DFV_ERR_READ;
        else if (!pfrm->pDoc->hasViewData)  ret = MSG_DFV_UNVISIBLE;
        else if (!pfrm->pView->hasViewData) ret = MSG_DFV_FAIL_VIEW;

        delete pfrm;    // Debug構成でコンパイルすると，CDxSRenderViewで頂点数0の場合，ここでセグメンテーションエラーを起こす？ 
        return ret;
    }
    pfrm->ShowWindow(SW_SHOW);
    pfrm->SetFocus();
    pfrm->pView->SetFocus();

    // 
    CString title = pfrm->pDoc->preTitle + pfrm->pDoc->Title + pfrm->pDoc->pstTitle;
    if (title.IsEmpty()) {
        pfrm->pDoc->preTitle = _T("Title: ");
        pfrm->pDoc->Title    = _T("Temporary Blank");
        pfrm->pDoc->pstTitle = _T("");
        title = pfrm->pDoc->preTitle + pfrm->pDoc->Title + pfrm->pDoc->pstTitle;
    }
    if (pfrm->preTitle.IsEmpty()) pfrm->preTitle = pfrm->pDoc->preTitle;
    if (pfrm->Title.IsEmpty()) pfrm->Title = title;
    if (pfrm->pView->Title.IsEmpty()) pfrm->pView->Title = title;
    //
    pfrm->pView->SetTitle(title);
    pfrm->pDoc->SetTitle(title);
    pfrm->SetTitle(title);
    pfrm->SetWindowText(title);

    return 0;
}


/**
void  jbxwl::ExecDocFrmViewError(HWND hwnd, int ret)

jbxwl::ExecDocFrmView() のエラーを処理する．
*/
void  jbxwl::ExecDocFrmViewError(HWND hwnd, int ret)
{
    if (ret>0) {  // !=MSG_DFV_NOT_DISP (-1)
        CString mesg;
        int err_flag = false;

        if (ret==MSG_DFV_FR_NULL) {
            mesg.LoadString(IDS_STR_NULLPT_FRM);    // Frameへのポインタが NULLです
            err_flag = true;
        }
        else if (ret==MSG_DFV_CANCEL){
            mesg.LoadString(IDS_STR_CANCEL_OPRTN);  // 操作がキャンセルされました
            err_flag = true;
        }
        else if (ret==MSG_DFV_ERR_READ) {
            mesg.LoadString(IDS_STR_ERR_OPEN_FILE); // ファイルの読み込みエラーです
            err_flag = true;
        }
        else if (ret==MSG_DFV_UNVISIBLE) {
            mesg.LoadString(IDS_STR_ERR_UNDISP);    // 表示不可能なファイルです
            err_flag = true;
        }
        else if (ret==MSG_DFV_FAIL_VIEW) {
            mesg.LoadString(IDS_STR_ERR_PREDISP);   // 表示の準備に失敗しました
            err_flag = true;
        }
        if (err_flag) MessageBox(hwnd, mesg, NULL, MB_OK);
    }
    return;
}


/**
int  jbxwl::ExecTemplate(CMultiDocTemplate* ptemp, ExMSGraph<sWord>* pmsGraph, ExCmnHead* pcmnHead, CExFrame* prntFrm, int viewPoint)

テンプレートを起動する．@n
渡すデータに対してメモリ管理機能あり．@n
メモリ管理していないデータを渡した場合は，クリアされる．
*/
int  jbxwl::ExecTemplate(CMultiDocTemplate* ptemp, ExMSGraph<sWord>* pmsGraph, ExCmnHead* pcmnHead, CExFrame* prntFrm, int viewPoint)
{
    int   ret,  color = GRAPH_COLOR_MONO;
    BOOL  hasReadData = FALSE;
    BOOL  hasViewData = FALSE;
    HWND  hWnd = NULL;
    CExFrame* pfrm = NULL;
    CExDocument* prntDoc = NULL;

    if (prntFrm!=NULL) {
        hWnd    = prntFrm->m_hWnd;
        prntDoc = prntFrm->pDoc;
        color   = prntFrm->colorMode;
    }

    if (pmsGraph!=NULL) {
        if (!pmsGraph->isNull()) hasReadData = TRUE;
    }   
    if (pcmnHead!=NULL) {
        if (!pcmnHead->isNull()) hasViewData = TRUE;
    }
    
    if (hasViewData) {
        pfrm = CreateDocFrmView(ptemp, NULL);
        pfrm->pDoc->hasReadData = TRUE;
        pfrm->pDoc->hasViewData = TRUE;
        pfrm->pDoc->cmnHead     = *pcmnHead;
        pfrm->pDoc->colorMode   = color;
        pfrm->pDoc->preTitle    = _T("");
        pfrm->pDoc->Title       = prntFrm->pDoc->Title;
        pfrm->pDoc->pstTitle    = prntFrm->pDoc->pstTitle;
        if (prntDoc!=NULL) pfrm->pDoc->ctrlCntrst = prntDoc->ctrlCntrst;
        pfrm->pView->vSBpos     = viewPoint;
        ret = ExecDocFrmView(pfrm);
    }

    else if (hasReadData) {
        pfrm = CreateDocFrmView(ptemp, NULL);
        pfrm->pDoc->hasReadData = TRUE;
        pfrm->pDoc->msGraph     = *pmsGraph;
        pfrm->pDoc->colorMode   = color;
        pfrm->pDoc->preTitle    = _T("");
        pfrm->pDoc->Title       = prntFrm->pDoc->Title;
        pfrm->pDoc->pstTitle    = prntFrm->pDoc->pstTitle;
        if (prntDoc!=NULL) pfrm->pDoc->ctrlCntrst = prntDoc->ctrlCntrst;
        pfrm->pView->vSBpos     = viewPoint;
        ret = ExecDocFrmView(pfrm);
    }   

    else {
        CString mesg;
        mesg.LoadString(IDS_STR_OPEN_FILE);
        CString fname = EasyGetOpenFileName(mesg, hWnd);
        if (fname.IsEmpty()) return -1;

        pfrm = CreateDocFrmView(ptemp, NULL);
        pfrm->pDoc->multiSliceData = TRUE;
        pfrm->pDoc->colorMode = color;
        if (prntDoc!=NULL) pfrm->pDoc->ctrlCntrst = prntDoc->ctrlCntrst;
        ret = ExecDocFrmView(pfrm, fname);
    } 

    ExecDocFrmViewError(hWnd, ret);

    return ret;
}


/**
CString  jbxwl::EasyGetOpenFileName(LPCWSTR title, HWND hWnd) : UNICODE用
CString  jbxwl::EasyGetOpenFileName(LPCTSTR title, HWND hWnd) : マルチバイト用

読み込み用ファイル名を得るためのダイアログを表示する．

@param title : ダイアログのタイトル．
@param hWnd  : ウィンドウハンドル
@retval CString : ファイルへのパス
*/

#ifdef _UNICODE
CString  jbxwl::EasyGetOpenFileName(LPCWSTR title, HWND hWnd)
{
    WCHAR fn[LPATH];
#else
CString  jbxwl::EasyGetOpenFileName(LPCTSTR title, HWND hWnd)
{
    TCHAR fn[LPATH];
#endif
    OPENFILENAME ofn;

    bzero(fn, sizeof(fn));
    bzero(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.Flags = OFN_HIDEREADONLY;
    ofn.lpstrFile = fn;
    ofn.nMaxFile = sizeof(fn) / sizeof(fn[0]);
    if (title != NULL) ofn.lpstrTitle = title;

    GetOpenFileName(&ofn);
    return fn;

    /*
    CFileDialog fileDialog(true);

    if (fileDialog.DoModal() == IDOK) {
        return fileDialog.GetPathName();
    }
    return _T("");
    */
}


/**
CString  jbxwl::EasyGetSaveFileName(LPCWSTR title, LPCWSTR extnt, HWND hWnd) : UNICODE用
CString  jbxwl::EasyGetSaveFileName(LPCTSTR title, LPCTSTR extnt, HWND hWnd) : マルチバイト用

保存用ファイル名を得るためのダイアログを表示する．

@param title : ダイアログのタイトル．
@param extnt : 保存用ファイルの拡張子
@param hWnd  : ウィンドウハンドル
@retval CString : ファイルへのパス
*/
#ifdef _UNICODE
CString  jbxwl::EasyGetSaveFileName(LPCWSTR title, LPCWSTR extnt, HWND hWnd)
{
    WCHAR fn[LPATH];
#else    
CString  jbxwl::EasyGetSaveFileName(LPCTSTR title, LPCTSTR extnt, HWND hWnd)
{
    TCHAR fn[LPATH];
#endif
    OPENFILENAME  ofn;

    bzero(fn, sizeof(fn));
    bzero(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.Flags = 0;
    ofn.lpstrFile = fn;
    //ofn.nMaxFile = LPATH;
    ofn.nMaxFile = sizeof(fn) / sizeof(fn[0]);
    ofn.lpstrDefExt = extnt;
    if (title != NULL) ofn.lpstrTitle = title;

    GetSaveFileName(&ofn);
    return fn;
}


/**
CString  jbxwl::EasyGetSaveFolderName(LPCTSTR folder, LPCTSTR title, HWND hWnd)

保存用フォルダ名を得るためのダイアログを表示する．

@bug おそらく UNICODEでは動かない？
*/
#ifdef _UNICODE
CString  jbxwl::EasyGetSaveFolderName(LPCWSTR folder, LPCWSTR title, HWND hWnd)
{
    WCHAR fldr[LPATH];
#else
CString  jbxwl::EasyGetSaveFolderName(LPCTSTR folder, LPCTSTR title, HWND hWnd)
{
    TCHAR fldr[LPATH];
#endif
    BROWSEINFO   bri;
    LPITEMIDLIST pilst;

    CString  str = _T(""); 

    bzero(fldr, LPATH);
    bzero(&bri, sizeof(bri));

    bri.hwndOwner = hWnd;
    bri.lpszTitle = title;
    bri.ulFlags = BIF_RETURNONLYFSDIRS;

    bri.lpfn = &EasyGetSaveFolderNameCallBack;  // コールバック関数
    bri.lParam = (LPARAM)folder;                // コールバック関数の引数

    pilst = SHBrowseForFolder(&bri);
    if (pilst!=NULL) {
        SHGetPathFromIDList(pilst, fldr);
        str = fldr;
        CoTaskMemFree(pilst);
    }

    return str;
}


/**
int  CALLBACK jbxwl::EasyGetSaveFolderNameCallBack(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)

EasyGetSaveFolderName() 用コールバック関数．
*/
int  CALLBACK jbxwl::EasyGetSaveFolderNameCallBack(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if(uMsg==BFFM_INITIALIZED){
        SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
    }
    return 0;
}


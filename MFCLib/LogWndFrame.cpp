//
// LogWndFrame.cpp : インプリメンテーション ファイル
//

#include  "MFCBase.h"
#include  "LogWndFrame.h"
#include  "MessageBoxDLG.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CLogWndFrame

IMPLEMENT_DYNCREATE(CLogWndFrame, CExTextFrame)

CLogWndFrame::CLogWndFrame()
{
    //"DEBUG_INFO("CONSTRUCTOR: CLogWndFrame\n");

    pDoc  = NULL;
    pView = NULL;
    pApp  = NULL;
}


//
CLogWndFrame::~CLogWndFrame()
{
    DEBUG_INFO("DESTRUCTOR: CLogWndFrame: START\n");

    DEBUG_INFO("DESTRUCTOR: CLogWndFrame: END\n");
}


BEGIN_MESSAGE_MAP(CLogWndFrame, CExTextFrame)
    //{{AFX_MSG_MAP(CLogWndFrame)
    ON_COMMAND(ID_LOG_COPY,  OnLogCopy)
    ON_COMMAND(ID_LOG_SAVE,  OnLogSave)
    ON_COMMAND(ID_LOG_CLEAR, OnLogClear)
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
//  ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CLogWndFrame メッセージ ハンドラ
//
int CLogWndFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CExTextFrame::OnCreate(lpCreateStruct) == -1) return -1;

    // ツールバーの作成
//  toolBar = new CExToolBar(this);
    toolBar = new CExToolBar();
    if (!toolBar->CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP 
        | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | WS_CLIPCHILDREN) ||
        !toolBar->LoadToolBar(IDR_LOG_WND_TOOLBAR))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // 作成に失敗
    }
    
    // TODO: ツール バーをドッキング可能にしない場合は以下の３行を削除してください．     
    toolBar->EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(toolBar);
    
    return 0;
}


void  CLogWndFrame::OnLogCopy()
{
    CString data = pView->getCopyData();
    
    char* mbstr = ts2mbs((LPCTSTR)data); 
    int size = (int)strlen(mbstr);
    if (size<=0) {
        ::free(mbstr);  
        return;
    }

    HGLOBAL hMem = ::GlobalAlloc(GHND, size+1);
    char* pszptr = (char*)::GlobalLock(hMem);
    memcpy(pszptr, mbstr, size+1);
    ::GlobalUnlock(hMem);
    ::free(mbstr);

    if (!OpenClipboard()) {
        ::GlobalFree(hMem);
        DEBUG_ERROR("CLogWndFrame::OnLogCopy(): ERROR: Clipboad open error!!\n");
        return;
    }

    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
    ::GlobalFree(hMem);

    return;
}


void  CLogWndFrame::OnLogSave()
{
    if (pDoc->save_fname==_T("")) {
        CString mesg;
        mesg.LoadString(IDS_STR_SPECIFY_SAVE_FILE);     // 保存用ファイルを指定する
        pDoc->save_fname = pDoc->easyGetSaveFileName(mesg, pView->m_hWnd);
    }
    if (pDoc->save_fname==_T("")) return;
    int ret = pDoc->writeLogFile();

    if (ret<0) MessageBoxDLG(IDS_STR_ERROR, IDS_STR_ERR_WRITE_FILE, MB_OK, NULL);   // ファイルの書き込みに失敗しました
}


void  CLogWndFrame::OnLogClear()
{
    CString mesg, noti;
    mesg.LoadString(IDS_STR_ASK_LOG_CLEAR);     // ログをクリアしますか？
    noti.LoadString(IDS_STR_CNFRM);             // 確認
    int ret = MessageBox(mesg, noti, MB_YESNO | MB_ICONQUESTION);
    if (ret==IDYES) pView->clearViewDoc();
}



/////////////////////////////////////////////////////////////////////
//

CLogWndFrame*  jbxwl::ExecLogWnd(CMultiDocTemplate* pDocTempl, LPCTSTR title, CAppCallBack* app)
{   
    // クラスの取得
    CLogWndDoc*   pdoc = (CLogWndDoc*)pDocTempl->CreateNewDocument();
    CLogWndFrame* pfrm = (CLogWndFrame*)pDocTempl->CreateNewFrame((CDocument*)pdoc, NULL);
    CLogWndView*  pviw = pdoc->GetView();
    pfrm->pTempl = pDocTempl;

    if (pdoc==NULL || pfrm==NULL || pviw==NULL) return NULL;

    pdoc->pView  = pfrm->pView  = pviw;
    pdoc->pFrame = pviw->pFrame = pfrm;
    pviw->pDoc   = pfrm->pDoc   = pdoc;
    pfrm->pApp   = pviw->pApp   = app;
    pfrm->Title  = title;

    pfrm->CExTextFrame::pView = (CExTextView*) pviw;
    pfrm->CExTextFrame::pDoc  = (CDocument*)   pdoc;
    pviw->CExTextView::pFrame = (CExTextFrame*)pfrm;
    pviw->CExTextView::pDoc   = (CDocument*)   pdoc;

    //
    pfrm->ShowWindow(SW_SHOW);
    pfrm->SetFocus();
    pfrm->pView->SetFocus();

    if (!pfrm->Title.IsEmpty()) {
        pfrm->pView->SetTitle(pfrm->Title);
    }

    return pfrm;
}

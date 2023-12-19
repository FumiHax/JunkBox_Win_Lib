//
// MessageBoxDLG.cpp : インプリメンテーション ファイル
//

#include  "MessageBoxDLG.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxwl;



/////////////////////////////////////////////////////////////////////////////
// CMessageBoxDLG ダイアログ

CMessageBoxDLG::CMessageBoxDLG(LPCTSTR tn, CWnd* pParent) 
    : CDialog(CMessageBoxDLG::IDD, pParent)
{
    //{{AFX_DATA_INIT(CMessageBoxDLG)
    //}}AFX_DATA_INIT

    dialogID = CMessageBoxDLG::IDD;
    pWnd     = pParent;
    title    = tn;
    message  = NULL;
    cancel   = false;
    //
    Create(dialogID, pWnd);
}


//
CMessageBoxDLG::CMessageBoxDLG(UINT nIDTemplate, LPCTSTR tn, CWnd* pParent) 
        : CDialog(nIDTemplate, pParent)
{
    dialogID = nIDTemplate;
    pWnd     = pParent;
    title    = tn;
    message  = NULL;
    cancel   = false;
    //
    Create(dialogID, pWnd);
}


//
CMessageBoxDLG::~CMessageBoxDLG()
{
    Destory();
}


//
void CMessageBoxDLG::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMessageBoxDLG)
    //}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CMessageBoxDLG, CDialog)
    //{{AFX_MSG_MAP(CMessageBoxDLG)
    //}}AFX_MSG_MAP
    ON_STN_CLICKED(IDC_MESG_BOX_TEXT, &CMessageBoxDLG::OnStnClickedMesgBoxText)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CMessageBoxDLG メッセージ ハンドラ

BOOL  CMessageBoxDLG::OnInitDialog()
{
    message = (CStatic*)GetDlgItem(IDC_MESG_BOX_TEXT);
    return TRUE;
}


//
// Progress Dialogの開始．Windowを表示させる．．
//
void  CMessageBoxDLG::Display(LPCTSTR mesg)
{
    if (pWnd!=NULL) {
        RECT rect;
        pWnd->GetWindowRect(&rect);
        int sx = (rect.left+rect.right)/2;
        int sy = (rect.top+rect.bottom)/2;
        this->GetWindowRect(&rect);
        sx -= (rect.right-rect.left)/2;
        sy -= (rect.bottom-rect.top)/2;
        this->SetWindowPos(NULL, sx, sy, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
    }
    ShowWindow(SW_SHOW);

    if (!title.IsEmpty()) SetWindowText(title);
    if (message!=NULL && mesg!=NULL) message->SetWindowText(mesg);
}


//
// Progress Dialogの停止．Windowを消し，変数を初期化する．
//
void  CMessageBoxDLG::Destory()
{
    DestroyWindow();
}


//
void jbxwl::CMessageBoxDLG::OnStnClickedMesgBoxText()
{
    // TODO: ここにコントロール通知ハンドラー コードを追加します。
}


//
// ウィンドウがクローズされた場合．
//
BOOL jbxwl::CMessageBoxDLG::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if (wParam==IDCANCEL) cancel = true;

    return CDialog::OnCommand(wParam, lParam);
}




///////////////////////////////////////////////////////////////////////////////
// CMessageBoxDLG (デフォルト:IDD_MESG_BOX) を使用したメッセージダイアログ．要 delete
//

CMessageBoxDLG*  jbxwl::MessageBoxDLG(int ttl, int msg, CWnd* wnd)
{
    CString mesg, noti;
    noti.LoadString(ttl);
    mesg.LoadString(msg);

    CMessageBoxDLG* mesgbox = new CMessageBoxDLG(noti, wnd);
    if (mesgbox!=NULL) mesgbox->Display(mesg);
    return mesgbox;
}


//
CMessageBoxDLG*  jbxwl::MessageBoxDLG(LPCTSTR ttl, int msg, CWnd* wnd)
{
    CString mesg;
    mesg.LoadString(msg);

    CMessageBoxDLG* mesgbox = new CMessageBoxDLG(ttl, wnd);
    if (mesgbox!=NULL) mesgbox->Display(mesg);
    return mesgbox;
}


//
CMessageBoxDLG*  jbxwl::MessageBoxDLG(int ttl, LPCTSTR msg, CWnd* wnd)
{
    CString noti;
    noti.LoadString(ttl);

    CMessageBoxDLG* mesgbox = new CMessageBoxDLG(noti, wnd);
    if (mesgbox!=NULL) mesgbox->Display(msg);
    return mesgbox;
}


//
CMessageBoxDLG*  jbxwl::MessageBoxDLG(LPCTSTR ttl, LPCTSTR msg, CWnd* wnd)
{
    CMessageBoxDLG* mesgbox = new CMessageBoxDLG(ttl, wnd);
    if (mesgbox!=NULL) mesgbox->Display(msg);
    return mesgbox;
}




///////////////////////////////////////////////////////////////////////////////
// MessageBox() のラッパー
// 

int  jbxwl::MessageBoxDLG(int ttl, int msg, UINT type, CWnd* wnd)
{
    CString mesg, noti;
    noti.LoadString(ttl);
    mesg.LoadString(msg);
    
    int ret; 
    if (wnd!=NULL) ret = ::MessageDLG(noti, mesg, type, wnd->m_hWnd);
    else           ret = ::MessageDLG(noti, mesg, type, (HWND)NULL);
    return ret;
}


//
int  jbxwl::MessageBoxDLG(LPCTSTR ttl, int msg, UINT type, CWnd* wnd)
{
    CString mesg;
    mesg.LoadString(msg);
    
    int ret;
    if (wnd!=NULL) ret = ::MessageDLG(ttl, mesg, type, wnd->m_hWnd);
    else           ret = ::MessageDLG(ttl, mesg, type, (HWND)NULL);
    return ret;
}


//
int  jbxwl::MessageBoxDLG(int ttl, LPCTSTR msg, UINT type, CWnd* wnd)
{
    CString noti;
    noti.LoadString(ttl);

    int ret;
    if (wnd!=NULL) ret = ::MessageDLG(noti, msg, type, wnd->m_hWnd);
    else           ret = ::MessageDLG(noti, msg, type, (HWND)NULL);
    return ret;
}


//
int  jbxwl::MessageBoxDLG(LPCTSTR ttl, LPCTSTR msg, UINT type, CWnd* wnd)
{
    int ret;
    if (wnd!=NULL) ret = ::MessageDLG(ttl, msg, type, wnd->m_hWnd);
    else           ret = ::MessageDLG(ttl, msg, type, (HWND)NULL);
    return ret;
}


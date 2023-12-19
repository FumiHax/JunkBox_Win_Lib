//
// ProgressTextDLG.cpp : インプリメンテーション ファイル
//

#include  "ProgressTextDLG.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CProgressTextDLG ダイアログ

CProgressTextDLG::CProgressTextDLG(LPCTSTR tn, BOOL edp, CWnd* pParent) 
    : CDialog(CProgressTextDLG::IDD, pParent)
{
    //{{AFX_DATA_INIT(CProgressTextDLG)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_DATA_INIT

    dialogID = CProgressTextDLG::IDD;
    pWnd     = pParent;
    title    = tn;
    enableDisPatcher = edp;
    dCounter = NULL;
}


//
CProgressTextDLG::CProgressTextDLG(UINT nIDTemplate, LPCTSTR tn, BOOL edp, CWnd* pParent) 
        : CDialog(nIDTemplate, pParent)
{
    dialogID = nIDTemplate;
    pWnd     = pParent;
    title    = tn;
    enableDisPatcher = edp;
    dCounter = NULL;
}


//
void CProgressTextDLG::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CProgressTextDLG)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CProgressTextDLG, CDialog)
    //{{AFX_MSG_MAP(CProgressTextDLG)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CProgressTextDLG メッセージ ハンドラ

BOOL  CProgressTextDLG::OnInitDialog()
{
    dCounter = (CStatic*)GetDlgItem(IDC_PROGTEXT_MSG);
    return TRUE;
}


//
// Progress Dialogの開始．Windowを表示させる．
// m は目盛の最大値になる．
//
void  CProgressTextDLG::Start(int m, char* tn)
{
    cancel = false;
    max = m;

    Create(dialogID, pWnd);
    ShowWindow(SW_SHOW);
    if (tn!=NULL) title = mbs2ts(tn);
    if (!title.IsEmpty()) SetWindowText(title);
    start = true;

//  DispCounter(0);
}


//
// Progress Dialogの停止．Windowを消し，変数を初期化する．
//
void  CProgressTextDLG::Stop()
{
    pos   = 0;
    title = _T("");
    cancel = false;
    start  = false;

    DestroyWindow();
}


//
void  CProgressTextDLG::StepIt(int n)
{
    pos += n;
    DispCounter(pos);

    if (enableDisPatcher) DisPatcher();
}


//
void  CProgressTextDLG::SetPos(int num)
{
    pos = num;
    DispCounter(pos);

    if (enableDisPatcher) DisPatcher();
}


//
// ダイアログのタイトルを設定する．
//
void  CProgressTextDLG::SetTitle(char* tn)
{
    if (tn!=NULL) title = mbs2ts(tn);
    if (start) SetWindowText(title);
}


/*
void CProgressTextDLG::OnCancel() 
{
    cancel = true;
    SetWindowText("キャンセル中");

//  CDialog::OnCancel();
}
*/


//
// ウィンドウがクローズされた場合．
//
BOOL CProgressTextDLG::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    if (wParam==IDCANCEL) {
        cancel = true;
        CString mesg;
        mesg.LoadString(IDS_STR_CANCELING);
        SetWindowText(mesg);
    }

    return TRUE;

//  return CDialog::OnCommand(wParam, lParam);
}


//
void  CProgressTextDLG::DispCounter(int n)
{
    if (n<0) dCounter->SetWindowText(_T("-----"));
    else {
        TCHAR buf[LNAME];
//      memset(buf, 0, LNAME);
        sntprintf(buf, LNAME, _T("%d") , n);
        dCounter->SetWindowText(buf);
    }
}



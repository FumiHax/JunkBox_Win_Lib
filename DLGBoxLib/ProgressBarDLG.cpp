//
// 再帰定義可能なプログレスバーカウンタ
//

#include  "ProgressBarDLG.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxwl;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CProgressBarDLG ダイアログ （再帰定義可能なビジュアルプログレスバー[カウンタ] for VC++）
//

CProgressBarDLG::CProgressBarDLG(LPCTSTR tname, BOOL dispatch, CWnd* pParent) 
        : CDialog(CProgressBarDLG::IDD, pParent)
{
    //{{AFX_DATA_INIT(CProgressBarDLG)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_DATA_INIT

    Init();

    dialogID = CProgressBarDLG::IDD;
    pWnd     = pParent;
    progBar  = NULL;
    gmax     = PROGRESS_COUNTERMAX;
    title    = tname;
    enableDisPatcher = dispatch;
}


//
CProgressBarDLG::CProgressBarDLG(UINT nIDTemplate, LPCTSTR tname, BOOL dispatch, CWnd* pParent) 
        : CDialog(nIDTemplate, pParent)
{
    Init();

    dialogID = nIDTemplate;
    pWnd     = pParent;
    progBar  = NULL;
    gmax     = PROGRESS_COUNTERMAX;
    title    = tname;
    enableDisPatcher = dispatch;
}


// 
CProgressBarDLG::CProgressBarDLG(CProgressBarDLG* counter, LPCTSTR tname, BOOL dispatch)
{
    Init();

    parent = counter;
    title  = tname;
    enableDisPatcher = dispatch;
}


//
CProgressBarDLG::~CProgressBarDLG()
{
    if (child !=NULL) delete  child;        // 子供がいれば先に死んでもらう．
    if (parent!=NULL) parent->child = NULL; // 親がいれば縁を切る．
    Stop();                                 // 停止．
}


//
void CProgressBarDLG::Init()
{
    dialogID = 0;
    pWnd    = NULL;
    progBar = NULL;
    
    max    = 0;
    fill   = 0;
    pos    = 0;

    gmax   = 0;
    base   = 0;
    grate  = 1.0;
    lrate  = 1.0;
    prate  = 1.0;
    parent = NULL;
    child  = NULL;
    title  = _T("");
    enableDisPatcher = TRUE;
}



void  CProgressBarDLG::Clear()
{
    title  = _T("");
    base   = 0;
    grate  = 1.0;
    lrate  = 1.0;
    prate  = 1.0;

    max    = 0;
    fill   = 0;
    pos    = 0;

    if (child!=NULL) delete child;
    child = NULL;

    if (progBar!=NULL) progBar->SetPos(0);
}



BOOL  CProgressBarDLG::OnInitDialog()
{
    if (parent==NULL) {
        //RECT r;
        //r.left=10; r.top=10; r.right=300; r.bottom=30;
        //progBar->Create(WS_VISIBLE|WS_CHILD|WS_BORDER, r, this, IDD_PROGBAR);
        if (progBar==NULL) progBar = (CProgressCtrl*)GetDlgItem(IDC_PROGBAR_M);

    }
    return TRUE;
}



void CProgressBarDLG::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CProgressBarDLG)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CProgressBarDLG, CDialog)
    //{{AFX_MSG_MAP(CProgressBarDLG)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CProgressBarDLG メッセージ ハンドラ

//
// カウンタの目盛の「現在位置」から「現在位置+m」の間を
// 新しいカウンタとして作り直す（チャイルドカウンタ）．
//
// この機能は関数の中でカウンタをローカライズするために用いられる．
// ローカライズされたカウンタは，通常のカウンタと同様に使用できる．
//          →  新しくチャイルドカウンタを作ったり，SetMax()やResetRate()で
//              目盛の比率を変えることが可能．
//
CVCounter* CProgressBarDLG::MakeChildCounter(int m)
{
    if (m<=0) return NULL;
    ResetRate(m, m);                // 該当区間の lrateを 1.0に戻す．

    child = new CProgressBarDLG(this, (LPCTSTR)title, enableDisPatcher);
    child->SetMax(m);
    return (CVCounter*)child;
}


//
// チャイルドカウンタとその子供たちを消去
//
void  CProgressBarDLG::DeleteChildCounter()
{
    PutFill();
    if (child!=NULL) delete child;
    child = NULL;
}


//
// 現在のアクティブ（使用可能）なカウンタ（一番下の子カウンタ）を得る．
//
// この機能は関数中のローカライズされたカウンタ環境で，現在有効なカウンタを
// 得るための用いられる．
//
CVCounter* CProgressBarDLG::GetUsableCounter()
{
    if (child==NULL) return (CVCounter*)this;
    else  return child->GetUsableCounter();
}


//
// Progress Dialogに表示できる最大値を指定する．
//
void  CProgressBarDLG::SetMax(int num)
{
    if (num<=0)  return;

    if (gmax==0) gmax = num;
    grate = (double)gmax/(double)num;
    fill = max = num;
}


//
// Progress Dialogの開始．Windowを表示させる．
// m は目盛の最大値になる．
//
void  CProgressBarDLG::Start(int m, char* tn)
{
    cancel = false;

    if (parent==NULL) {
        this->Create(dialogID, pWnd);
        this->ShowWindow(SW_SHOW);

        if (tn!=NULL) title = mbs2ts(tn);
        if (!title.IsEmpty()) SetWindowText(title);
        start = true;

        if (progBar!=NULL) {
            progBar->SetPos(0);
            progBar->SetRange((short)0, (short)PROGRESS_COUNTERMAX);
            progBar->SetStep(1);
        }
    }
    SetMax(m);
}


//
// Progress Dialogの停止．Windowを消し，変数を初期化する．
//
void  CProgressBarDLG::Stop()
{
    max   = 0;
    pos   = 0;
    fill  = 0;
    base  = 0;
    grate = 1.0;
    lrate = 1.0;
    title = _T("");
    cancel = false;
    start  = false;

    if (parent==NULL) {
        DestroyWindow();
//      deleteNull(progBar);
    }
}


//
// ダイアログのタイトルを設定する．
//
void  CProgressBarDLG::SetTitle(char* tn)
{
    if (tn!=NULL) title = mbs2ts(tn);

    if (parent!=NULL) {
        parent->SetTitle(tn);
    }
    else {
        if (start) SetWindowText(title);
    }
}


//
// Progress Dialogの目盛を，目標値(fill)まで埋める．
// デフォルトでは目標値(fill)は maxになる．つまり（ローカライズされた）
// 全目盛を埋める．
// ResetRate()関数を呼ぶと，第２引数が目標値fillとなる．
//
void  CProgressBarDLG::PutFill()
{
    pos = fill;
    int mem = base + (int)(grate*lrate*pos + 0.5);

    if (parent!=NULL) {
        parent->SetPos(mem);
    }
    else {
        progBar->SetPos(mem);
    }

    if (enableDisPatcher) DisPatcher();
}


//
// Progress Dialogの目盛の「現在位置」から「現在位置+rng」の間を
// num でマップ（リサイズ）し直す．numはその状態での目標値fillとなる．
// rng は正, numは非負でなければならない．
//
void  CProgressBarDLG::ResetRate(int rng, int num)
{
    if (rng<=0 || num<0) return;
    if (num==0) num = 1;

    base  = base + (int)(pos*lrate*grate + 0.5);
    lrate = (double)rng/(double)num;
    pos   = 0;
    fill  = num;
}


//
void  CProgressBarDLG::StepIt(int n)
{
    pos += n;
    int mem = base + (int)(grate*lrate*pos + 0.5);

    if (parent!=NULL) {
        parent->SetPos(mem);
    }
    else {
        progBar->SetPos(mem);
    }

    if (enableDisPatcher) DisPatcher();
}


//
void  CProgressBarDLG::SetPos(int num)
{
    if ((int)(lrate*num)>fill) return;

    pos = num;
    int mem = base + (int)(grate*lrate*pos + 0.5);

    if (parent!=NULL) {
        parent->SetPos(mem);
    }
    else {
        progBar->SetPos(mem);
    }

    if (enableDisPatcher) DisPatcher();
}


//
bool  CProgressBarDLG::isCanceled()
{
    if (parent==NULL) return cancel;
    else return parent->isCanceled();
}


//
// ウィンドウがクローズされた場合．
//
BOOL CProgressBarDLG::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    if (wParam==IDCANCEL) cancel = true;

    return CDialog::OnCommand(wParam, lParam);
}


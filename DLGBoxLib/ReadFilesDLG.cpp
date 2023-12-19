//
// マルチスライス読み込み設定ダイアログ
//

#include  "ReadFilesDLG.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxwl;


CSetHeaderDLG  jbxwl::SetHeaderDLG;


/////////////////////////////////////////////////////////////////////////////
// CReadFilesDLG ダイアログ

CReadFilesDLG::CReadFilesDLG(LPCTSTR fname, CWnd* pParent /*=NULL*/)
    : CDialog(CReadFilesDLG::IDD, pParent) 
{
    //{{AFX_DATA_INIT(CReadFilesDLG)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_DATA_INIT

    fnameEBox   = NULL;
    fromNumEBox = NULL;
    toNumEBox   = NULL;

    pWnd    = pParent;
    fName   = fname;
    fromNum = 0;
    toNum   = 0;

//  hSize   = 0;
//  xSize   = 512;
//  ySize   = 512;
//  zSize   = 1;
//  dSize   = 16;
//  ltEnd   = FALSE;
}


//
CReadFilesDLG::~CReadFilesDLG()
{
//  free(fName);
}


//
void  CReadFilesDLG::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CReadFilesDLG)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CReadFilesDLG, CDialog)
    //{{AFX_MSG_MAP(CReadFilesDLG)
    ON_BN_CLICKED(IDSET, OnSetHead)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CReadFilesDLG メッセージ ハンドラ

BOOL  CReadFilesDLG::OnInitDialog()
{
    fnameEBox   = (CEdit*)GetDlgItem(IDC_RF_FNAME);
    fromNumEBox = (CEdit*)GetDlgItem(IDC_RF_FROMNUM);
    toNumEBox   = (CEdit*)GetDlgItem(IDC_RF_TONUM);
    if (fName!=_T("")) fnameEBox->SetWindowText(fName);

    return TRUE;
}


/*
メモ
DoModalのリーターン
    -1          エラー
    IDABORT
    IDOK
    IDCANCEL
/**/


//
void  CReadFilesDLG::OnOK() 
{   
    TCHAR buf[LNAME];

    fnameEBox->GetWindowText(buf, LNAME);
    fName = buf;

    fromNumEBox->GetWindowText(buf, LNAME);
    fromNum = ttoi(buf);

    toNumEBox->GetWindowText(buf, LNAME);
    toNum = ttoi(buf);

/*  if (SetHeaderDLG.sPara) {
        hSize = SetHeaderDLG.hSize;
        xSize = SetHeaderDLG.xSize;
        ySize = SetHeaderDLG.ySize;
        zSize = SetHeaderDLG.zSize;
        dSize = SetHeaderDLG.dSize;
        ltEnd = SetHeaderDLG.ltEnd;
    }
*/
    CDialog::OnOK();
}


//
void  CReadFilesDLG::OnSetHead() 
{
//  CSetHeaderDLG* shdlg = new CSetHeaderDLG(hSize, xSize, ySize, zSize, dSize, ltEnd);
//  if (shdlg==NULL) return;
    
//  SetHeaderDLG.setParameter(hSize, xSize, ySize, zSize, dSize, ltEnd);
    if (SetHeaderDLG.DoModal()!=IDOK) {
        //delete (shdlg);
        return;
    }

//  hSize = SetHeaderDLG.hSize;
//  xSize = SetHeaderDLG.xSize;
//  ySize = SetHeaderDLG.ySize;
//  zSize = SetHeaderDLG.zSize;
//  dSize = SetHeaderDLG.dSize;
//  ltEnd = SetHeaderDLG.ltEnd;

//  delete (shdlg);
    return;
}


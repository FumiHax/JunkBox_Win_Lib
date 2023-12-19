//
// SetHeaderDLG.cpp : インプリメンテーション ファイル
//

#include  "SetHeaderDLG.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CSetHeaderDLG ダイアログ


//CSetHeaderDLG::CSetHeaderDLG(int h, int x, int y, int z, int d, BOOL ltend, BOOL savep, CWnd* pParent)
CSetHeaderDLG::CSetHeaderDLG(CWnd* pParent) : CDialog(CSetHeaderDLG::IDD, pParent) 
{
    //{{AFX_DATA_INIT(CSetHeaderDLG)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_DATA_INIT

    hSize = 0;
    xSize = 512;
    ySize = 512;
    zSize = 1;
    dSize = 16;
    zBase = 0;

    ltEnd  = FALSE;
//  dicomF = FALSE;
    sPara  = FALSE;

    hEBox = NULL; 
    xEBox = NULL; 
    yEBox = NULL; 
    zEBox = NULL; 
    dEBox = NULL; 
    eCBox = NULL;
//  dCBox = NULL;
    sCBox = NULL;
    bEBox = NULL;
}


//
void CSetHeaderDLG::DoDataExchange(CDataExchange* pDX) 
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSetHeaderDLG)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_DATA_MAP

}



BEGIN_MESSAGE_MAP(CSetHeaderDLG, CDialog)
    //{{AFX_MSG_MAP(CSetHeaderDLG)
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_BN_CLICKED(IDC_FH_SAVEP, &CSetHeaderDLG::OnBnClickedFhSavep)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CSetHeaderDLG メッセージ ハンドラ

void  CSetHeaderDLG::setParameter(int h, int x, int y, int z, int d, int b, BOOL ltend, BOOL dicom)
{
    if (!sPara) {   // パラメータを保存しない設定なら
        hSize = h;
        xSize = x;
        ySize = y;
        zSize = z;
        dSize = d;
    }
    zBase  = b; 
    ltEnd  = ltend;
    //dicomF = dicom;
}


//
BOOL  CSetHeaderDLG::OnInitDialog()
{
    TCHAR buf[LNAME];
//  memset(buf, 0, LNAME);

    hEBox = (CEdit*)GetDlgItem(IDC_FH_HSIZE);
    xEBox = (CEdit*)GetDlgItem(IDC_FH_XSIZE);
    yEBox = (CEdit*)GetDlgItem(IDC_FH_YSIZE);
    zEBox = (CEdit*)GetDlgItem(IDC_FH_ZSIZE);
    dEBox = (CEdit*)GetDlgItem(IDC_FH_DEPTH);
    sCBox = (CButton*)GetDlgItem(IDC_FH_SAVEP);

    eCBox = (CButton*)GetDlgItem(IDC_FH_LTLEND);
    bEBox = (CEdit*)GetDlgItem(IDC_FH_BASE);

    sntprintf(buf, LNAME, _T("%d"), hSize);
    hEBox->SetWindowText(buf);

    sntprintf(buf, LNAME, _T("%d"), xSize);
    xEBox->SetWindowText(buf);

    sntprintf(buf, LNAME, _T("%d"), ySize);
    yEBox->SetWindowText(buf);

    sntprintf(buf, LNAME, _T("%d"), zSize);
    zEBox->SetWindowText(buf);

    sntprintf(buf, LNAME, _T("%d"), dSize);
    dEBox->SetWindowText(buf);

    sntprintf(buf, LNAME, _T("%d"), zBase);
    bEBox->SetWindowText(buf);

    if (sPara) sCBox->SetCheck(1);
    else       sCBox->SetCheck(0);

//  if (dicomF)dCBox->SetCheck(1);
//  else       dCBox->SetCheck(0);

    if (ltEnd) eCBox->SetCheck(1);
    else       eCBox->SetCheck(0);

    return TRUE;
}


//
void CSetHeaderDLG::OnOK() 
{
    TCHAR buf[LNAME];
//  memset(buf, 0, LNAME);

    hEBox->GetWindowText(buf, LNAME);
    hSize = ttoi(buf);

    xEBox->GetWindowText(buf, LNAME);
    xSize = ttoi(buf);

    yEBox->GetWindowText(buf, LNAME);
    ySize = ttoi(buf);

    zEBox->GetWindowText(buf, LNAME);
    zSize = ttoi(buf);

    dEBox->GetWindowText(buf, LNAME);
    dSize = ttoi(buf);

    bEBox->GetWindowText(buf, LNAME);
    zBase = ttoi(buf);

    if (sCBox->GetCheck()==1) sPara = TRUE;
    else                      sPara = FALSE;

    if (eCBox->GetCheck()==1) ltEnd = TRUE;
    else                      ltEnd = FALSE;

/*  if (dCBox->GetCheck()==1) {
        dicomF = TRUE;
        sPara  = FALSE;
    }
    else dicomF = FALSE;
*/
    CDialog::OnOK();
    return;
}


//
CmnHead  CSetHeaderDLG::getCmnHead() 
{
    CmnHead  hd;
    
    init_CmnHead(&hd);
    hd.kind = 0;

    if (zBase!=0) {
        hd.kind |= HAS_BASE;
        ZeroBase = zBase;
    }
    if (ltEnd)  hd.kind |= HAS_LENDIAN;

/*  if (dicomF) {
        hd.kind |= DICOM_DATA;
        hd.xsize = 512;
        hd.ysize = 512;
        hd.depth = 16;
    }
*/
    if (sPara) {
        hd.kind |= USERSET_DATA;
        hd.xsize = xSize;
        hd.ysize = ySize;
        hd.zsize = zSize;
        hd.depth = dSize;
        hd.bsize = hSize;
        hd.lsize = xSize*ySize*zSize*((dSize+7)/8);
    }
    
    return hd;
}


//
void CSetHeaderDLG::OnBnClickedOk()
{
    // TODO : ここにコントロール通知ハンドラ コードを追加します．
    OnOK();
}


//
void CSetHeaderDLG::OnBnClickedFhSavep()
{
    // TODO: ここにコントロール通知ハンドラ コードを追加します．
}


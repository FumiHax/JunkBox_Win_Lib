//
// WriteFileDLG.cpp : インプリメンテーション ファイル
//

#include  "WriteFilesDLG.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CWriteFilesDLG ダイアログ

CWriteFilesDLG::CWriteFilesDLG(LPCTSTR fname, int fmt, int fnum, int tnum, CWnd* pParent /*=NULL*/)
    : CDialog(CWriteFilesDLG::IDD, pParent)
{
    //{{AFX_DATA_INIT(CWriteFilesDLG)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_DATA_INIT

    fmtsCMBBox  = NULL;
    fnameEBox   = NULL;
    fromNumEBox = NULL;
    toNumEBox   = NULL;
    mltSaveCBtn = NULL;

    pWnd    = pParent;
    fName   = fname;
    format  = fmt;
    fromNum = fnum;
    toNum   = tnum;
    mltSave = FALSE;
    zSize   = abs(tnum - fnum) + 1;
}


//
CWriteFilesDLG::~CWriteFilesDLG()
{
}


//
void CWriteFilesDLG::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWriteFilesDLG)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CWriteFilesDLG, CDialog)
    //{{AFX_MSG_MAP(CWriteFilesDLG)
    //}}AFX_MSG_MAP
    ON_CBN_SELCHANGE(IDC_WF_FORMATS, &CWriteFilesDLG::OnCbnSelchangeWfFormats)
    ON_BN_CLICKED(IDC_WF_MFL, &CWriteFilesDLG::OnBnClickedWfMfl)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CWriteFilesDLG メッセージ ハンドラ

BOOL  CWriteFilesDLG::OnInitDialog()
{
    TCHAR buf[LNAME];
//  memset(buf, 0, LNAME);

    fmtsCMBBox  = (CComboBox*)GetDlgItem(IDC_WF_FORMATS);
    fnameEBox   = (CEdit*)GetDlgItem(IDC_WF_FNAME);
    fromNumEBox = (CEdit*)GetDlgItem(IDC_WF_FROMNUM);
    toNumEBox   = (CEdit*)GetDlgItem(IDC_WF_TONUM);
    mltSaveCBtn = (CButton*)GetDlgItem(IDC_WF_MFL);
    
    // ComboBox
    fmtsCMBBox->AddString(_T(CT_DATA_STR));
    fmtsCMBBox->AddString(_T(MOON_DATA_STR));
    fmtsCMBBox->AddString(_T(JPEG_RGB_DATA_STR));
    fmtsCMBBox->AddString(_T(JPEG_MONO_DATA_STR));
    fmtsCMBBox->AddString(_T(RAS_DATA_STR));

    CString fmtStr = _T(CT_DATA_STR);                                   //
    if      (format==CT_DATA)        fmtStr = _T(CT_DATA_STR);          //
    else if (format==CT_3DM)         fmtStr = _T(CT_DATA_STR);          //
    else if (format==MOON_DATA)      fmtStr = _T(MOON_DATA_STR);        //
    else if (format==JPEG_RGB_DATA)  fmtStr = _T(JPEG_RGB_DATA_STR);
    else if (format==JPEG_MONO_DATA) fmtStr = _T(JPEG_MONO_DATA_STR);
    else if (format==RAS_DATA)       fmtStr = _T(RAS_DATA_STR);
    //else if (format==TIFF_DATA)    fmtStr = _T(TIFF_DATA_STR);
    //else if (format==PNG_DATA)     fmtStr = _T(PNG_DATA_STR);
    
    fmtsCMBBox->SelectString(-1, fmtStr);
    setDLGState((LPCTSTR)fmtStr);
    setNumBoxState();
    
    // 
    if (fName!=_T("")) fnameEBox->SetWindowText(fName);

    sntprintf(buf, LNAME, _T("%d"), fromNum);
    fromNumEBox->SetWindowText(buf);

    sntprintf(buf, LNAME, _T("%d"), toNum);
    toNumEBox->SetWindowText(buf);
    
    mltSaveCBtn->SetCheck(0);
    
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
void CWriteFilesDLG::OnOK() 
{
    TCHAR buf[LNAME];
//  memset(buf, 0, LNAME);

    fnameEBox->GetWindowText(buf, LNAME);
    fName = buf;

    fromNumEBox->GetWindowText(buf, LNAME);
    fromNum = ttoi(buf);

    toNumEBox->GetWindowText(buf, LNAME);
    toNum = ttoi(buf);

    // Format
    this->OnCbnSelchangeWfFormats();

    // 3D
    if (!mltSave && zSize>1) {
        if (format==CT_DATA) format = CT_3DM;
    }

    CDialog::OnOK();    
}


//
void  CWriteFilesDLG::OnCbnSelchangeWfFormats()
{
    TCHAR buf[LNAME];
    fmtsCMBBox->GetWindowText(buf, LNAME);

    setDLGState(buf);
    setNumBoxState();
}


//
void  CWriteFilesDLG::setDLGState(LPCTSTR str)
{
    if (zSize==1) {
        mltSaveCBtn->EnableWindow(FALSE);
        mltSaveCBtn->SetCheck(0);
        mltSave = FALSE;
    }

    if (!tcscmp(str,_T(CT_DATA_STR))) {
        if (zSize>1) {
            mltSaveCBtn->EnableWindow(TRUE);
        }
        format = CT_DATA;
    }

    // 以降3次元データとしては保存不可
    else if (!tcscmp(str,_T(MOON_DATA_STR))) {
        if (zSize>1) {
            mltSaveCBtn->EnableWindow(FALSE);
            mltSaveCBtn->SetCheck(1);
            mltSave = TRUE;
        }
        format = MOON_DATA;
    }
    else if (!tcscmp(str,_T(RAS_DATA_STR))) {
        if (zSize>1) {
            mltSaveCBtn->EnableWindow(FALSE);
            mltSaveCBtn->SetCheck(1);
            mltSave = TRUE;
        }
        format = RAS_DATA;
    }
    else if (!tcscmp(str,_T(JPEG_MONO_DATA_STR))) {
        if (zSize>1) {
            mltSaveCBtn->EnableWindow(FALSE);
            mltSave = TRUE;
            mltSaveCBtn->SetCheck(1);
        }
        format = JPEG_MONO_DATA;
    }
    else if (!tcscmp(str,_T(JPEG_RGB_DATA_STR))){
        mltSaveCBtn->EnableWindow(FALSE);
        mltSaveCBtn->SetCheck(0);
        mltSave = FALSE;
        format = JPEG_RGB_DATA;
    }
    else {
        if (zSize>1) {
            mltSaveCBtn->EnableWindow(FALSE);
            mltSaveCBtn->SetCheck(1);
            mltSave = FALSE;
        }
        format = UN_KNOWN_DATA;
    }
}


//
void  CWriteFilesDLG::setNumBoxState(){
    // ナンバーBOXの状態
    if (mltSaveCBtn->GetCheck()==1) {
        fromNumEBox->EnableWindow(TRUE);
        toNumEBox->EnableWindow(TRUE);
        mltSave = TRUE;
    }
    else {
        fromNumEBox->EnableWindow(FALSE);
        toNumEBox->EnableWindow(FALSE);
        mltSave = FALSE;
    }
}


//
void  CWriteFilesDLG::OnBnClickedWfMfl()
{
    setNumBoxState();
}


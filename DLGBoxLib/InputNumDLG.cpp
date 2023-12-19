//
// InputNumDLG.cpp : 実装ファイル
//


#include  "InputNumDLG.h"


using namespace jbxwl;



//////////////////////////////////////////////////////////////////////////////////////////
// CInputNumDLG ダイアログ
//

IMPLEMENT_DYNAMIC(CInputNumDLG, CDialog)

CInputNumDLG::CInputNumDLG(CWnd* pParent /*=NULL*/)
    : CDialog(CInputNumDLG::IDD, pParent)
{
    valtlBox = NULL;
    valBox   = NULL;

    valtl = _T("");
    val   = 0;
}



CInputNumDLG::CInputNumDLG(LPCTSTR vt, int v, CWnd* pParent /*=NULL*/)
    : CDialog(CInputNumDLG::IDD, pParent)
{
    valtlBox = NULL;
    valBox   = NULL;

    valtl = vt;
    val   = v;
}



CInputNumDLG::~CInputNumDLG()
{
}



void  CInputNumDLG::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(CInputNumDLG, CDialog)
END_MESSAGE_MAP()


// CInputNumDLG メッセージ ハンドラ


BOOL  CInputNumDLG::OnInitDialog()
{
    TCHAR buf[LMNAME];

    valtlBox = (CStatic*)GetDlgItem(IDC_INM_VALTTL);
    valBox   = (CEdit*)GetDlgItem(IDC_INM_VAL);

    if (valtl!=_T("")) valtlBox->SetWindowText(valtl);

    sntprintf(buf, LMNAME, _T("%d"), val);
    valBox->SetWindowText(buf);

    return TRUE;
}



void  CInputNumDLG::OnOK() 
{
    TCHAR buf[LMNAME];

    valBox->GetWindowText(buf, LMNAME);
    val = ttoi(buf);

    CDialog::OnOK();
    return;
}




//////////////////////////////////////////////////////////////////////////////////////////
// CInputFloatNumDLG ダイアログ
//

IMPLEMENT_DYNAMIC(CInputFloatNumDLG, CDialog)

CInputFloatNumDLG::CInputFloatNumDLG(CWnd* pParent /*=NULL*/)
    : CDialog(CInputFloatNumDLG::IDD, pParent)
{
    valtlBox = NULL;
    valBox   = NULL;

    valtl = _T("");
    val   = 0.0;
}



CInputFloatNumDLG::CInputFloatNumDLG(LPCTSTR vt, double v, CWnd* pParent /*=NULL*/)
    : CDialog(CInputFloatNumDLG::IDD, pParent)
{
    valtlBox = NULL;
    valBox   = NULL;

    valtl = vt;
    val   = v;
}



CInputFloatNumDLG::~CInputFloatNumDLG()
{
}



void  CInputFloatNumDLG::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(CInputFloatNumDLG, CDialog)
END_MESSAGE_MAP()


// CInputFloatNumDLG メッセージ ハンドラ


BOOL  CInputFloatNumDLG::OnInitDialog()
{
    TCHAR buf[LMNAME];

    valtlBox = (CStatic*)GetDlgItem(IDC_INM_FLTTTL);
    valBox   = (CEdit*)GetDlgItem(IDC_INM_FLT);

    if (valtl!=_T("")) valtlBox->SetWindowText(valtl);

    sntprintf(buf, LMNAME, _T("%8f"), val);
    valBox->SetWindowText(buf);

    return TRUE;
}



void  CInputFloatNumDLG::OnOK() 
{
    TCHAR buf[LMNAME];

    valBox->GetWindowText(buf, LMNAME);
    val = ttof(buf);

    CDialog::OnOK();
    return;
}




//////////////////////////////////////////////////////////////////////////////////////////
// CInputMultiFloatNumDLG ダイアログ
//

IMPLEMENT_DYNAMIC(CInputMultiFloatNumDLG, CDialog)

CInputMultiFloatNumDLG::CInputMultiFloatNumDLG(CWnd* pParent /*=NULL*/)
    : CDialog(CInputMultiFloatNumDLG::IDD, pParent)
{
    for (int i=0; i<6; i++) {
        valtlBox[i] = NULL;
        valBox[i]   = NULL;
        valtl[i]    = _T("");
        val[i]      = 0.0;
    }
}



CInputMultiFloatNumDLG::CInputMultiFloatNumDLG(LPCTSTR t0, double v0, LPCTSTR t1, double v1, LPCTSTR t2, double v2, 
                                               LPCTSTR t3, double v3, LPCTSTR t4, double v4, LPCTSTR t5, double v5, CWnd* pParent)
    : CDialog(CInputMultiFloatNumDLG::IDD, pParent)
{
    for (int i=0; i<6; i++) {
        valtlBox[i] = NULL;
        valBox[i]   = NULL;
    }

    valtl[0] = t0;
    valtl[1] = t1;
    valtl[2] = t2;
    valtl[3] = t3;
    valtl[4] = t4;
    valtl[5] = t5;
    val[0]   = v0;
    val[1]   = v1;
    val[2]   = v2;
    val[3]   = v3;
    val[4]   = v4;
    val[5]   = v5;
}



CInputMultiFloatNumDLG::~CInputMultiFloatNumDLG()
{
}



void  CInputMultiFloatNumDLG::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(CInputMultiFloatNumDLG, CDialog)
END_MESSAGE_MAP()



// CInputNumDLG メッセージ ハンドラ


BOOL  CInputMultiFloatNumDLG::OnInitDialog()
{
    valtlBox[0] = (CStatic*)GetDlgItem(IDC_INM_MLT_VALTTL0);
    valtlBox[1] = (CStatic*)GetDlgItem(IDC_INM_MLT_VALTTL1);
    valtlBox[2] = (CStatic*)GetDlgItem(IDC_INM_MLT_VALTTL2);
    valtlBox[3] = (CStatic*)GetDlgItem(IDC_INM_MLT_VALTTL3);
    valtlBox[4] = (CStatic*)GetDlgItem(IDC_INM_MLT_VALTTL4);
    valtlBox[5] = (CStatic*)GetDlgItem(IDC_INM_MLT_VALTTL5);

    valBox[0]   = (CEdit*)GetDlgItem(IDC_INM_MLT_VAL0);
    valBox[1]   = (CEdit*)GetDlgItem(IDC_INM_MLT_VAL1);
    valBox[2]   = (CEdit*)GetDlgItem(IDC_INM_MLT_VAL2);
    valBox[3]   = (CEdit*)GetDlgItem(IDC_INM_MLT_VAL3);
    valBox[4]   = (CEdit*)GetDlgItem(IDC_INM_MLT_VAL4);
    valBox[5]   = (CEdit*)GetDlgItem(IDC_INM_MLT_VAL5);

    TCHAR buf[LMNAME];
//  memset(buf, 0, LNAME);

    for (int i=0; i<6; i++) {
        if (valtl[i]!=_T("")) {
            valtlBox[i]->SetWindowText(valtl[i]);
            sntprintf(buf, LMNAME, _T("%8f"), val[i]);
            valBox[i]->SetWindowText(buf);
        }
        else {
            CString mesg;
            mesg.LoadString(IDS_STR_UNUSED);
            valtlBox[i]->SetWindowText(mesg);
            valtlBox[i]->EnableWindow(FALSE);
            valBox[i]->SetWindowText(_T("0.0"));
            valBox[i]->EnableWindow(FALSE);
        }
    }

    return TRUE;
}



void  CInputMultiFloatNumDLG::OnOK() 
{
    TCHAR buf[LMNAME];
//  memset(buf, 0, LNAME);

    for (int i=0; i<6; i++) {
        valBox[i]->GetWindowText(buf, LMNAME);
        val[i] = ttof(buf);
    }

    CDialog::OnOK();
    return;
}



/////////////////////////////////////////////////////////////////////////////////////////

BOOL  jbxwl::InputNumDLG(LPCTSTR title, int* val)
{
    CInputNumDLG* inpdlg = new CInputNumDLG(title, *val);
    if (inpdlg->DoModal()!=IDOK) { 
        delete(inpdlg);
        return FALSE;
    }

    *val = inpdlg->val;
    delete(inpdlg);
    return TRUE;
}



BOOL  jbxwl::InputFloatNumDLG(LPCTSTR title, double* val)
{
    CInputFloatNumDLG* inpdlg = new CInputFloatNumDLG(title, *val);
    if (inpdlg->DoModal()!=IDOK) { 
        delete(inpdlg);
        return FALSE;
    }

    *val = inpdlg->val;
    delete(inpdlg);
    return TRUE;
}



BOOL  jbxwl::InputMultiFloatNumDLG(LPCTSTR t0, double* v0, LPCTSTR t1, double* v1, LPCTSTR t2, double* v2,
                                   LPCTSTR t3, double* v3, LPCTSTR t4, double* v4, LPCTSTR t5, double* v5)
{
    CString  ttl[6];
    double   val[6];

    for (int i=0; i<6; i++) {
        ttl[i] = _T("");
        val[i] = 0.0;
    }

    if (t0!=NULL && v0!=NULL) {
        ttl[0] = t0;
        val[0] = *v0;
    }
    if (t1!=NULL && v1!=NULL) {
        ttl[1] = t1;
        val[1] = *v1;
    }
    if (t2!=NULL && v2!=NULL) {
        ttl[2] = t2;
        val[2] = *v2;
    }
    if (t3!=NULL && v3!=NULL) {
        ttl[3] = t3;
        val[3] = *v3;
    }
    if (t4!=NULL && v4!=NULL) {
        ttl[4] = t4;
        val[4] = *v4;
    }
    if (t5!=NULL && v5!=NULL) {
        ttl[5] = t5;
        val[5] = *v5;
    }

    CInputMultiFloatNumDLG* inpdlg = new CInputMultiFloatNumDLG((LPCTSTR)ttl[0], val[0], (LPCTSTR)ttl[1], val[1], 
                                                                (LPCTSTR)ttl[2], val[2], (LPCTSTR)ttl[3], val[3], 
                                                                (LPCTSTR)ttl[4], val[4], (LPCTSTR)ttl[5], val[5]);
    if (inpdlg->DoModal()!=IDOK) { 
        delete(inpdlg);
        return FALSE;
    }

    if (v0!=NULL) *v0 = inpdlg->val[0];
    if (v1!=NULL) *v1 = inpdlg->val[1];
    if (v2!=NULL) *v2 = inpdlg->val[2];
    if (v3!=NULL) *v3 = inpdlg->val[3];
    if (v4!=NULL) *v4 = inpdlg->val[4];
    if (v5!=NULL) *v5 = inpdlg->val[5];

    delete(inpdlg);
    return TRUE;
}


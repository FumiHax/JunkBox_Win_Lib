#pragma once

#include  "DLGBoxBase.h"


//
namespace jbxwl {


BOOL  InputNumDLG(LPCTSTR title, int* val);
BOOL  InputFloatNumDLG(LPCTSTR title, double* val);

BOOL  InputMultiFloatNumDLG(LPCTSTR t0, double* v0, LPCTSTR t1=NULL, double* v1=NULL, LPCTSTR t2=NULL, double* v2=NULL,
                  LPCTSTR t3=NULL, double* v3=NULL, LPCTSTR t4=NULL, double* v4=NULL, LPCTSTR t5=NULL, double* v5=NULL);



////////////////////////////////////////////////////////////////////////////////////
// CInputNumDLG ダイアログ

class CInputNumDLG : public CDialog
{
    DECLARE_DYNAMIC(CInputNumDLG)

public:
    CInputNumDLG(CWnd* pParent=NULL);   // 標準コンストラクタ
    CInputNumDLG(LPCTSTR tn, int v=0, CWnd* pParent=NULL);
    virtual ~CInputNumDLG();

    virtual BOOL  OnInitDialog();

// ダイアログ データ
    enum { IDD = IDD_INPNUM_DLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
    virtual void OnOK();

public:
    CStatic*    valtlBox;
    CEdit*      valBox;

    CString     valtl;
    int         val;

    DECLARE_MESSAGE_MAP()
};




////////////////////////////////////////////////////////////////////////////////////
// CInputFloatNumDLG ダイアログ

class CInputFloatNumDLG : public CDialog
{
    DECLARE_DYNAMIC(CInputFloatNumDLG)

public:
    CInputFloatNumDLG(CWnd* pParent=NULL); 
    CInputFloatNumDLG(LPCTSTR tn, double v=0, CWnd* pParent=NULL);
    virtual ~CInputFloatNumDLG();

    virtual BOOL  OnInitDialog();

// ダイアログ データ
    enum { IDD = IDD_INPFLT_DLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
    virtual void OnOK();

public:
    CStatic*    valtlBox;
    CEdit*      valBox;

    CString     valtl;
    double      val;

    DECLARE_MESSAGE_MAP()
};




////////////////////////////////////////////////////////////////////////////////////
// CInputMultiFloatNumDLG ダイアログ

class CInputMultiFloatNumDLG : public CDialog
{
    DECLARE_DYNAMIC(CInputMultiFloatNumDLG)

public:
    CInputMultiFloatNumDLG(CWnd* pParent=NULL);
    CInputMultiFloatNumDLG(LPCTSTR t0, double v0=0., LPCTSTR t1=NULL, double v1=0., LPCTSTR t2=NULL, double v2=0., 
                      LPCTSTR t3=NULL, double v3=0., LPCTSTR t4=NULL, double v4=0., LPCTSTR t5=NULL, double v5=0., CWnd* pParent=NULL);
    virtual ~CInputMultiFloatNumDLG();

    virtual BOOL  OnInitDialog();

// ダイアログ データ
    enum { IDD = IDD_MLT_INPNUM_DLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();

public:
    CStatic*    valtlBox[6];
    CEdit*      valBox[6];

    CString     valtl[6];
    double      val[6];

    DECLARE_MESSAGE_MAP()
};



}       // namespace


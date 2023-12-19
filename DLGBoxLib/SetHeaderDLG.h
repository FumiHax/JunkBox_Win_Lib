#pragma once

#include  "DLGBoxBase.h"


//
namespace jbxwl {


/////////////////////////////////////////////////////////////////////////////
// CSetHeaderDLG ダイアログ

class CSetHeaderDLG : public CDialog
{
public:

    CEdit*   hEBox;
    CEdit*   xEBox;
    CEdit*   yEBox;
    CEdit*   zEBox;
    CEdit*   dEBox;

    CEdit*   bEBox;

    CButton* eCBox;
    //CButton* dCBox;
    CButton* sCBox;

    int      hSize;
    int      xSize;
    int      ySize;
    int      zSize;
    int      dSize;
    int      zBase;

    BOOL     ltEnd;     // リトルエンディアン
//  BOOL     dicomF;    // DICOMファイル
    BOOL     sPara;     // パラメータを保存

// コンストラクション
public:
    CSetHeaderDLG(CWnd* pParent=NULL);

    void  setParameter(int h, int x, int y, int z, int d, int b=0, BOOL ltend=FALSE, BOOL dicom=FALSE);
    virtual  BOOL  OnInitDialog();

    CmnHead  getCmnHead();
    
    // ダイアログ データ
    //{{AFX_DATA(CSetHeaderDLG)
    enum { IDD = IDD_FHDLG };
    //}}AFX_DATA

// オーバーライド
protected:
    //{{AFX_VIRTUAL(CSetHeaderDLG)
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

// インプリメンテーション
protected:
    //{{AFX_MSG(CSetHeaderDLG)
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedFhSavep();
};


//{{AFX_INSERT_LOCATION}}


}       // namespace

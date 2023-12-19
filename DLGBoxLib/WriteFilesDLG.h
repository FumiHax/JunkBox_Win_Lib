#pragma once

//
// WriteFileDLG.h : ヘッダー ファイル
//

#include  "DLGBoxBase.h"


//
namespace jbxwl {


/////////////////////////////////////////////////////////////////////////////
// CWriteFilesDLG ダイアログ

class CWriteFilesDLG : public CDialog
{
public:
    CWnd*       pWnd;

    CComboBox*  fmtsCMBBox;
    CEdit*      fnameEBox;
    CEdit*      fromNumEBox;
    CEdit*      toNumEBox;
    CButton*    mltSaveCBtn;

    CString     fName;
    int         format;
    int         fromNum;
    int         toNum;
    int         zSize;
    BOOL        mltSave;

// コンストラクション
public:
    CWriteFilesDLG(LPCTSTR fname, int fmtm, int fnum, int tnum, CWnd* pParent=NULL);
    virtual ~CWriteFilesDLG();

    void    setDLGState(LPCTSTR fmtstr);
    void    setNumBoxState();

    virtual BOOL    OnInitDialog();

// ダイアログ データ
    //{{AFX_DATA(CWriteFilesDLG)
    enum { IDD = IDD_WFDLG };
    //}}AFX_DATA

// オーバーライド
protected:
    //{{AFX_VIRTUAL(CWriteFilesDLG)
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

// インプリメンテーション
protected:
    //{{AFX_MSG(CWriteFilesDLG)
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnCbnSelchangeWfFormats();
    afx_msg void OnBnClickedWfMfl();
};


//{{AFX_INSERT_LOCATION}}


}       // namespace

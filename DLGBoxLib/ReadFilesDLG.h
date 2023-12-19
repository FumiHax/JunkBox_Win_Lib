#pragma once

//
// ReadFilesDLG.h : ヘッダー ファイル
//

/**
マルチスライス読み込み設定ダイアログ

*/


#include  "SetHeaderDLG.h"


//
namespace jbxwl {


extern  CSetHeaderDLG  SetHeaderDLG;


/////////////////////////////////////////////////////////////////////////////
// CReadFilesDLG ダイアログ

class CReadFilesDLG : public CDialog
{
public:
    CWnd*       pWnd;

    CEdit*      fnameEBox;
    CEdit*      fromNumEBox;
    CEdit*      toNumEBox;

    CString     fName;
    int         fromNum;
    int         toNum;

public:
    // ヘッダ設定用
//  int     hSize;  // ヘッダサイズ
//  int     xSize;  // X サイズ
//  int     ySize;  // Y サイズ
//  int     zSize;  // Z サイズ
//  int     dSize;  // カラーデプス
    
//  BOOL    ltEnd;  // リトルエンディアン

// コンストラクション
public:
    CReadFilesDLG(LPCTSTR fname, CWnd* pParent = NULL);
    virtual ~CReadFilesDLG();

    virtual BOOL    OnInitDialog();

// ダイアログ データ
    //{{AFX_DATA(CReadFilesDLG)
    enum { IDD = IDD_RFDLG };
    //}}AFX_DATA

// オーバーライド
protected:
    //{{AFX_VIRTUAL(CReadFilesDLG)
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

// インプリメンテーション
protected:
    //{{AFX_MSG(CReadFilesDLG)
    virtual void OnOK();
    afx_msg void OnSetHead();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}


}       // namespace


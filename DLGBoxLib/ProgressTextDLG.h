#pragma once

//
// ProgressTextDLG.h : ヘッダー ファイル
//

#include  "DLGBoxBase.h"


//
namespace jbxwl {

using namespace jbxl;


/**
テキストカウンタ： 
        
    n<0 で SetPos(n) は "-----" を表示

*/


/////////////////////////////////////////////////////////////////////////////
// CProgressTextDLG ダイアログ

class CProgressTextDLG : public CDialog, public CVCounter
{
public:
    CWnd*       pWnd;
    UINT        dialogID;
    CString     title;
    BOOL        enableDisPatcher;   

    CStatic*    dCounter;

// コンストラクション
public:
    CProgressTextDLG(                  LPCTSTR tn=NULL, BOOL edp=FALSE, CWnd* pParent=NULL);
    CProgressTextDLG(UINT nIDTemplate, LPCTSTR tn=NULL, BOOL edp=FALSE, CWnd* pParent=NULL); 
    virtual ~CProgressTextDLG() { Stop();}
    
    BOOL  OnInitDialog();
    
    virtual void  Start(int m=100, char* tn=NULL);  // カウンタスタート
    virtual void  Stop();                               // 一時停止

    virtual void  SetPos(int pos);
    virtual void  StepIt(int n=1);
    virtual void  SetTitle(char* tn);

    void  DispCounter(int n);

// ダイアログ データ
    //{{AFX_DATA(CProgressTextDLG)
    enum { IDD = IDD_PROGTEXT };
    //}}AFX_DATA

// オーバーライド
protected:
    //{{AFX_VIRTUAL(CProgressTextDLG)
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    //}}AFX_VIRTUAL

// インプリメンテーション
protected:
    //{{AFX_MSG(CProgressTextDLG)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}


}       // namespace



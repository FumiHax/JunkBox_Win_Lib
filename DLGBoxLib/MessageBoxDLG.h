#pragma once

//
// MessageBoxDLG.h : ヘッダー ファイル
//

#include  "DLGBoxBase.h"


//
namespace jbxwl {


/////////////////////////////////////////////////////////////////////////////
// CMessageBoxDLG ダイアログ

class CMessageBoxDLG : public CDialog
{
public:
    CWnd*       pWnd;
    UINT        dialogID;
    CString     title;
    CStatic*    message;
    bool        cancel;

// コンストラクション
public:
    CMessageBoxDLG(                  LPCTSTR tn=NULL, CWnd* pParent=NULL);
    CMessageBoxDLG(UINT nIDTemplate, LPCTSTR tn=NULL, CWnd* pParent=NULL); 

    virtual ~CMessageBoxDLG();
    
    BOOL    OnInitDialog();
    
    void    Display(LPCTSTR mesg=NULL);
    void    Destory(void);
    bool    isCanceled() {return cancel;}

// ダイアログ データ
    //{{AFX_DATA(CMessageBoxDLG)
    enum { IDD = IDD_MESG_BOX };
    //}}AFX_DATA

// オーバーライド
protected:
    //{{AFX_VIRTUAL(CMessageBoxDLG)
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

// インプリメンテーション
protected:
    //{{AFX_MSG(CMessageBoxDLG)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnStnClickedMesgBoxText();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};


//{{AFX_INSERT_LOCATION}}



/////////////////////////////////////////////////////////////////////////////

// CMessageBoxDLG (デフォルト:IDD_MESG_BOX) を使用したメッセージダイアログ．要 delete
CMessageBoxDLG*   MessageBoxDLG(int     ttl, int     msg, CWnd* wnd=NULL);
CMessageBoxDLG*   MessageBoxDLG(LPCTSTR ttl, int     msg, CWnd* wnd=NULL);
CMessageBoxDLG*   MessageBoxDLG(int     ttl, LPCTSTR msg, CWnd* wnd=NULL);
CMessageBoxDLG*   MessageBoxDLG(LPCTSTR ttl, LPCTSTR msg, CWnd* wnd=NULL);

// MessageBox() のラッパー
int  MessageBoxDLG(int     ttl, int     msg, UINT type, CWnd* wnd=NULL);
int  MessageBoxDLG(LPCTSTR ttl, int     msg, UINT type, CWnd* wnd=NULL);
int  MessageBoxDLG(int     ttl, LPCTSTR msg, UINT type, CWnd* wnd=NULL);
int  MessageBoxDLG(LPCTSTR ttl, LPCTSTR msg, UINT type, CWnd* wnd=NULL);


// type ex.  see MessageBox() and WjnUser.h 
/*
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
*/


/////////////////////////////////////////////////////////////////////////////


}       // namespace

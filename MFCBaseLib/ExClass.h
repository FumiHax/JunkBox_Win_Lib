#pragma once

/**
拡張  ビュー，フレーム，ドキュメント管理ライブラリ

*/

#ifdef WIN32
#pragma warning(disable:4005)
#pragma warning(disable:4996)
#endif

#include  "ExView.h"
#include  "ExDocument.h"
#include  "ExFrame.h"
#include  "ExTextView.h"
#include  "ExTextFrame.h"

#define    MSG_DEFAULT_WINDOW_SIZE     512

#define    MSG_DFV_NOT_DISP            -1
#define    MSG_DFV_FR_NULL             1
#define    MSG_DFV_CANCEL              2
#define    MSG_DFV_ERR_READ            10
#define    MSG_DFV_UNVISIBLE           11
#define    MSG_DFV_FAIL_VIEW           12


//
namespace jbxwl {

using namespace jbxl;


class   CExDocument;
class   CExView;
class   CExScrollTextView;
class   CExFrame;
class   CExToolBar;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  上位アプリケーションへのコールバック用
//
class  CAppCallBack
{
protected:
    CAppCallBack()  { callMSGraph.init();}
    virtual ~CAppCallBack() {}

public:
    // 作業用
    MSGraph<sWord>  callMSGraph;    // メモリ管理無し

public:
    virtual CExFrame* TemplateCallBack(CExFrame* pfrm, int mode, LPCTSTR title=NULL) { return NULL;}
    virtual CMultiDocTemplate*  GetMultiDocTemplate(int mode) { return NULL;}

    virtual void DocumentDestructor(CExDocument* doc) {return;}

    virtual void FrameDestructor(CExFrame* frm) {return;}
    virtual void ViewDestructor (CExView*   vw) {return;}

    virtual void FrameDestructor(CExTextFrame* fm) {return;}
    virtual void ViewDestructor (CExTextView*  vw) {return;}
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
int   ExecTemplate(CMultiDocTemplate* ptemp, ExMSGraph<sWord>* pmsGraph=NULL, ExCmnHead* pcmnHead=NULL, CExFrame* prntFrm=NULL, int vPoint=0);

CExFrame*  CreateDocFrmView(CMultiDocTemplate* pDocTemp, CAppCallBack* papp=NULL);

int   ExecDocFrmView(CExFrame* pfrm, LPCTSTR fname=NULL);
void  ExecDocFrmViewError(HWND hwnd, int ret);

// Document, View クラスの前処理 (ファイルをオープンし，表示の準備をする)
BOOL  InitialDocView(CExFrame* pfrm, LPCTSTR fname);

// 各オブジェクト間の関連付け
bool  SetExLink(CExDocument* pdoc, CExFrame* pfrm, CExView* pview, CAppCallBack* papp);


//
#ifdef _UNICODE
CString  EasyGetOpenFileName(LPCWSTR title = NULL, HWND hWnd = NULL);
CString  EasyGetSaveFileName(LPCWSTR title = NULL, LPCWSTR extnt = NULL, HWND hWnd = NULL);
CString  EasyGetSaveFolderName(LPCWSTR folder = _T(""), LPCWSTR title = NULL, HWND hWnd = NULL);
#else
CString  EasyGetOpenFileName(LPCTSTR title=NULL, HWND hWnd=NULL);
CString  EasyGetSaveFileName(LPCTSTR title=NULL, LPCTSTR extnt=NULL, HWND hWnd=NULL);
CString  EasyGetSaveFolderName(LPCTSTR folder=_T(""), LPCTSTR title=NULL, HWND hWnd=NULL);
#endif

int CALLBACK EasyGetSaveFolderNameCallBack(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

#define  EasyGetOpenFolderName   EasyGetSaveFolderName

}       // namespace

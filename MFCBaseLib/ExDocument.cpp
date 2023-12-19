//
// ExDocument.cpp : インプリメンテーション ファイル
//

#include  "ExDocument.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxwl;



/////////////////////////////////////////////////////////////////////////////
// CExDocument

IMPLEMENT_DYNCREATE(CExDocument, CDocument)


CExDocument::CExDocument()
{
    //DEBUG_WARN("CExDocument::CExDocument Start\n");

    init_CmnHead(&infHead);
    msGraph.init();
    cmnHead.init();

    hasReadData    = FALSE; // データを正常に読み込み，保持しているか．
    hasViewData    = FALSE; // 読み込んだデータから表示用データを作成したか．
    ctrlCntrst     = FALSE; // 表示用データ作成時に（画素値が255以下でも）強制的にコントラストを調整を行う．
    multiSliceData = FALSE; // TRUE ならマルチスライス読み込みモード
    notFreeMSGraph = FALSE; // メモリ管理無しの場合，このオブジェクトが msGraph のメモリを開放するのは禁止．
    notFreeCmnHead = FALSE; // メモリ管理無しの場合，このオブジェクトが cmnHead のメモリを開放するのは禁止．
    
    pView       = NULL;
    pFrame      = NULL;
    pApp        = NULL;
    anyData     = NULL;

    Title       = _T("");
    preTitle    = _T("");
    pstTitle    = _T("");

    startNo     = 0;
    endNo       = 0;
    vMax        = 0;
    vMin        = 0;
    colorMode   = GRAPH_COLOR_MONO;
    base        = 0;

    //DEBUG_WARN("CExDocument::CExDocument End\n");
}



CExDocument::~CExDocument()
{
    DEBUG_INFO("DESTRUCTOR: CExDocument\n");

    if (pApp!=NULL) pApp->DocumentDestructor(this);         // 上位アプリケーションに通知

    if (notFreeCmnHead) cmnHead.mfree();
    else                cmnHead.free();
    if (notFreeMSGraph) msGraph.mfree();
    else                msGraph.free();
    
    if (!isNull(pView))  pView->pDoc  = NULL;
    if (!isNull(pFrame)) pFrame->pDoc = NULL;
    pView  = NULL;
    pFrame = NULL;
}



BEGIN_MESSAGE_MAP(CExDocument, CDocument)
    //{{AFX_MSG_MAP(CExDocument)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CExDocument 診断

#ifdef _DEBUG
void CExDocument::AssertValid() const
{
    CDocument::AssertValid();
}


//
void CExDocument::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CExDocument シリアライズ

void CExDocument::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: この位置に保存用のコードを追加してください
    }
    else
    {
        // TODO: この位置に読み込み用のコードを追加してください
    }
}




/////////////////////////////////////////////////////////////////////////////
// CExDocument コマンド

BOOL CExDocument::OnNewDocument()
{
    //DEBUG_INFO("CExDocument::OnNewDocument(): START\n");

    if (!hasReadData) return FALSE;

    if (!hasViewData) hasViewData = MakeViewData();
    if (!hasViewData) return FALSE;

    //DEBUG_INFO("CExDocument::OnNewDocument(): END\n");
    return  TRUE;
}


//
BOOL CExDocument::OnOpenDocument(LPCTSTR lpszPathName) 
{
    //DEBUG_INFO("CExDocument::OnOpenDocument(): START\n");

    if (!hasReadData) hasReadData = ReadDataFile(lpszPathName);
    if (!hasReadData) return FALSE;

    if (!hasViewData) hasViewData = MakeViewData();
    if (!hasViewData) return FALSE;

    //DEBUG_INFO("CExDocument::OnOpenDocument(): END\n");
    return TRUE;
}


//
CExView*  CExDocument::GetView()
{
    if (pView==NULL) {
        POSITION pos = GetFirstViewPosition();
        while (pos!=NULL) {
            CExView* pview = (CExView*)GetNextView(pos);
            if (this==pview->GetDocument()) return pview;
        }
        return NULL;
    }
    return pView;
}



/**
    機能：  画像データを読み込む．
    引数：  ファイル名
    戻値：  TRUE: 成功，FALSE: 失敗
    説明：  読み込んだグラフィックデータ(msGraphメンバ)
*/
BOOL   CExDocument::ReadDataFile(LPCTSTR str)
{
    return  TRUE;
}


//
//  読み込んだデータから表示用のデータを作る．
//          オーバーライド用
//
BOOL  CExDocument::MakeViewData()
{
    return TRUE;
}


//
//  データを書き込む．
//          オーバーライド用
//
BOOL  CExDocument::WriteDataFile(LPCTSTR fname)
{
    return TRUE;
}



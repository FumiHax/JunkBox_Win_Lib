//
// vThumbNailFrame.cpp : インプリメンテーション ファイル
//

#include  "MFCBase.h"
#include  "vThumbNailFrame.h"
#include  "vThumbNailDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CvThumbNailFrame

IMPLEMENT_DYNCREATE(CvThumbNailFrame, CExFrame)

CvThumbNailFrame::CvThumbNailFrame()
{
}


//
CvThumbNailFrame::~CvThumbNailFrame()
{
}



BEGIN_MESSAGE_MAP(CvThumbNailFrame, CExFrame)
    //{{AFX_MSG_MAP(CvThumbNailFrame)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CvThumbNailFrame メッセージ ハンドラ

//
// サムネイル画像をクリック
//          下位のテンプレートを獲得して，実行する．
//
void  CvThumbNailFrame::onLButtonDBLClick() 
{
    POINT  pt = pView->GetMousePos();
    if (pt.x<0 || pt.y<0) return;
    pt.y = pt.y + pView->vSBpos;

    int n = ((CvThumbNailDoc*)pDoc)->GetTNailNum(pt) - 1;

    if (n>=0) {
        colorMode = pDoc->colorMode;

        // Plane毎に色が分かれている場合は Plane（8bit）毎に表示
        colorMode = pDoc->colorMode;
        if (colorMode==GRAPH_COLOR_RGB  || colorMode==GRAPH_COLOR_BGR  || 
            colorMode==GRAPH_COLOR_XRGB || colorMode==GRAPH_COLOR_ARGB || colorMode==GRAPH_COLOR_RGBA || 
            colorMode==GRAPH_COLOR_XBGR || colorMode==GRAPH_COLOR_ABGR || colorMode==GRAPH_COLOR_BGRA) {
            colorMode = GRAPH_COLOR_MONO;   // Plane毎に色が分かれている場合は Plane（8bit）毎に表示
        }

        CMultiDocTemplate* ptemp = pApp->GetMultiDocTemplate(APP_TEMPL_VTHUMBNAILFRAME);
        ExecTemplate(ptemp, &pDoc->msGraph, NULL, this, n);
        
        /*
        pApp->callMSGraph.setup(pDoc->msGraph.xs, pDoc->msGraph.ys, 1);
        if (pApp->callMSGraph.isNull()) return;
        for (int i=0; i<pDoc->msGraph.xs*pDoc->msGraph.ys; i++) {
            pApp->callMSGraph.gp[i] = pDoc->msGraph.gp[(n-1)*pDoc->msGraph.xs*pDoc->msGraph.ys + i];
        }
        pApp->TemplateCallBack(this, APP_CALLBACK_2D);
        pApp->callMSGraph.init();
        */
    }
}


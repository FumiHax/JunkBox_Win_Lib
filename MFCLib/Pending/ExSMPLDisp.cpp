//
// ExSMPLDisp.cpp : インプリメンテーション ファイル
//

#include  "MFCBase.h"
#include  "ExSMPLDisp.h"

#include  "Graph.h"
#include  "appCallBack.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CExSMPLDisp

IMPLEMENT_DYNCREATE(CExSMPLDisp, CExFrame)


/*
CExSMPLDisp::CExSMPLDisp()
{
}
*/


//
CExSMPLDisp::~CExSMPLDisp()
{
}


void  CExSMPLDisp::displayNew(MSGraph<sWord> vp)
{
    if (pApp!=NULL) { 
        copy_MSGraph(vp, msGraph);
        if (msGraph.gp==NULL) return;
//      for (int i=0; i<msGraph.xs*msGraph.ys*msGraph.zs; i++) {
//          if (msGraph.gp[i]>=EMAXCT) msGraph.gp[i] = -100;
//      }
        pApp->callMSGraph = msGraph;
        if (!pApp->callMSGraph.isNull()) {
            pFrame = pApp->TemplateCallBack(this, APP_CALLBACK_2D);
            CString mesg;
            mesg.LoadString(IDS_STR_NEXT_QUSTN);
            MessageBox(mesg, _T(""), MB_OK);
            //pApp->callMSGraph.mfree();
            if (!isNull(pFrame->m_hWnd)) pFrame->DestroyWindow();
            msGraph.init();     // msGraphのデータ(gp)は破棄されているはず
        }
    }
}


//
void  CExSMPLDisp::displayNext(MSGraph<sWord> vp)
{
//  if (pFrame!=NULL) {
//      cat_MSGraph(vp, msGraph);
//
//  }
}



BEGIN_MESSAGE_MAP(CExSMPLDisp, CExFrame)
    //{{AFX_MSG_MAP(CExSMPLDisp)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CExSMPLDisp メッセージ ハンドラ


//
// Dx3DDirectView.cpp : 実装ファイル
//

#include  "MFCBase.h"
#include  "Dx3DDirectView.h"


using namespace jbxwl;


// CDx3DDirectView

IMPLEMENT_DYNAMIC(CDx3DDirectView, CExView)


CDx3DDirectView::CDx3DDirectView()
{

}



CDx3DDirectView::~CDx3DDirectView()
{
}



BEGIN_MESSAGE_MAP(CDx3DDirectView, CDxVTXBaseView)
END_MESSAGE_MAP()



// CDx3DDirectView メッセージ ハンドラ


void CDx3DDirectView::OnDraw(CDC* /*pDC*/)
{
    // TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください．
}


//
void CDx3DDirectView::OnInitialUpdate()
{
    CDxVTXBaseView::OnInitialUpdate();

    // TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください．
}


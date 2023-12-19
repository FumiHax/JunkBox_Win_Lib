//
// CvThumbNailDoc.cpp : インプリメンテーション ファイル
//

#include  "MFCBase.h"
#include  "vThumbNailDoc.h"
#include  "ProgressBarDLG.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CvThumbNailDoc

IMPLEMENT_DYNCREATE(CvThumbNailDoc, CExDocument)


CvThumbNailDoc::CvThumbNailDoc()
{
    SetSize(THUMBNAIL_SIZE, THUMBNAIL_SIZE, 4, 2, 4, 12);
}



CvThumbNailDoc::~CvThumbNailDoc()
{
}



BEGIN_MESSAGE_MAP(CvThumbNailDoc, CRwGRDoc)
    //{{AFX_MSG_MAP(CvThumbNailDoc)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CvThumbNailDoc 診断

#ifdef _DEBUG
void CvThumbNailDoc::AssertValid() const
{
    CRwGRDoc::AssertValid();
}


//
void CvThumbNailDoc::Dump(CDumpContext& dc) const
{
    CRwGRDoc::Dump(dc);
}
#endif //_DEBUG




/////////////////////////////////////////////////////////////////////////////
// CvThumbNailDoc シリアライズ

void CvThumbNailDoc::Serialize(CArchive& ar)
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
// CvThumbNailDoc コマンド

CmnHead  CvThumbNailDoc::TranslateData()
{
    int   i,  j, k;
    CmnHead  chd;

    // サムネイルのサイズの再計算
    if (msGraph.xs>0 && msGraph.ys>0) {
        if (msGraph.xs>msGraph.ys) {
            int ysize = THUMBNAIL_SIZE*msGraph.ys/msGraph.xs;       
            SetSize(0, ysize, 0, 0, 0, 0);      // 0 の箇所は変化なし
        }
        else {
            int xsize = THUMBNAIL_SIZE*msGraph.xs/msGraph.ys;       
            SetSize(xsize, 0, 0, 0, 0, 0);      // 0 の箇所は変化なし
        }
    }

    colorMode = msGraph.color;
    chd.kind  = CT_DATA;
    chd.xsize = xClientSize;
    chd.ysize = Max(((msGraph.zs+1)/2+1)*(yTNailSize+tNailBorder), yClientSize);
    chd.zsize = yClientSize; 
    chd.depth = 8;
    chd.bsize = 0;
    chd.buf   = NULL;
    chd.lsize = chd.xsize*chd.ysize;
    chd.grptr = (uByte*)malloc(chd.lsize);
    if (chd.grptr==NULL) {
        freeCmnHead(&chd);
        chd.xsize = -2;
        return chd;
    }
    for (i=0; i<chd.xsize*chd.ysize; i++) chd.grptr[i] = VIEW_8_MAX;    // バックを白にする

    // カウンタ
    CVCounter* counter = NULL;  // カウンタはここでは，delete禁止
    if (chd.zsize>10) {
        counter = GetUsableGlobalCounter();
        if (counter!=NULL) counter->SetMax(msGraph.zs*2/10);
    }

    // コントラスト調整のための最大値と最小値
    int min = SINTMAX;
    int max = SINTMIN;
    int  ks, js;
    for (k=0; k<msGraph.zs; k++) {
        ks = k*msGraph.xs*msGraph.ys;
        for (j=0; j<msGraph.ys; j++) {
            js = j*msGraph.xs;
            for (i=0; i<msGraph.xs; i++) {
                min = Min((int)msGraph.gp[i+js+ks], min);
                max = Max((int)msGraph.gp[i+js+ks], max);
            }
        }
        if (counter!=NULL && k%10==0) {
            counter->StepIt();
            if (counter->isCanceled()) {        //  キャンセル
                freeCmnHead(&chd);
                chd.xsize = -3;
                return chd;
            }
        }
    }

    if (min>=0 && max<=VIEW_8_MAX && !ctrlCntrst) {
        max = VIEW_8_MAX;
        min = 0;
    }
    msGraph.max = (sWord)max;
    msGraph.min = (sWord)min;

    RECT rect;
    int  ii, jj;
    for (k=0; k<msGraph.zs; k++) {
        rect.left   = (k%2)*(xTNailSize + tNailBorder) + 1;
        rect.right  = rect.left + xTNailSize - 1;
        rect.top    = (k/2)*(yTNailSize + tNailBorder) + 1;
        rect.bottom = rect.top  + yTNailSize - 1;

        for (i=rect.left-1; i<=rect.right +1; i++) CmnHeadBytePoint(chd, i, rect.top-1)    = 0;
        for (i=rect.left-1; i<=rect.right +1; i++) CmnHeadBytePoint(chd, i, rect.bottom+1) = 0;
        for (j=rect.top-1;  j<=rect.bottom+1; j++) CmnHeadBytePoint(chd, rect.left -1, j)  = 0;
        for (j=rect.top-1;  j<=rect.bottom+1; j++) CmnHeadBytePoint(chd, rect.right+1, j)  = 0;

        for (j=0; j<yTNailSize; j++) {
            jj = j*(msGraph.ys-1)/(yTNailSize-1);
            for (i=0; i<xTNailSize; i++) {
                ii = i*(msGraph.xs-1)/(xTNailSize-1);
                CmnHeadBytePoint(chd, rect.left+i, rect.top+j) = (uByte)((float)(msGraph.point(ii, jj, k)-min)/(max-min)*VIEW_8_MAX);
            }
        }
        if (counter!=NULL && k%10==0) {
            counter->StepIt();
            if (counter->isCanceled()) {        // キャンセル
                freeCmnHead(&chd);
                chd.xsize = -3;
                return chd;
            }
        }
    }

    // 表示はモノクロ固定
    pView->colorMode = GRAPH_COLOR_MONO;
    pView->maxYSize  = yMaxClientSize;

    return  chd;
}



/**
    txs  サムネイルの Xサイズ（ピクセル）
    tys  サムネイルの Yサイズ（ピクセル）
    bds  サムネイル間のボーダサイズ（ピクセル）
    hrs  ウィンドウの初期横サイズ（サムネイルの個数で指定）
    vrs  ウィンドウの初期縦サイズ（サムネイルの個数で指定）

    0以下 が指定された場合は，値を変更しない．
*/
void  CvThumbNailDoc::SetSize(int txs, int tys, int bds, int hrs, int vrs, int ymx)
{
    if (txs>0) xTNailSize  = txs;
    if (tys>0) yTNailSize  = tys;
    if (bds>0) tNailBorder = bds;
    if (hrs>0) xTNailOrder = hrs;
    if (vrs>0) yTNailOrder = vrs;
    if (ymx>0) yMaxOrder   = Max(yTNailOrder, ymx);
            
    xClientSize    = (xTNailSize + tNailBorder)*xTNailOrder;
    yClientSize    = (yTNailSize + tNailBorder)*yTNailOrder;
    yMaxClientSize = (yTNailSize + tNailBorder)*yMaxOrder;
}



int   CvThumbNailDoc::GetTNailNum(POINT pt)
{
    int xx  = pt.x/(xTNailSize+ tNailBorder) + 1;  
    int yy  = pt.y/(yTNailSize+ tNailBorder) + 1;
    int ret = (yy - 1)*2 + xx; 

    if (ret<0 || ret>msGraph.zs) ret = -1;
    return ret;
}



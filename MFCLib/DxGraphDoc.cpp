//
// CDxGraphDoc.cpp : インプリメンテーション ファイル
//

#include  "MFCBase.h"
#include  "DxGraphDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CDxGraphDoc

IMPLEMENT_DYNCREATE(CDxGraphDoc, CExDocument)

//
CDxGraphDoc::CDxGraphDoc()
{
}


//
CDxGraphDoc::~CDxGraphDoc()
{
}



BEGIN_MESSAGE_MAP(CDxGraphDoc, CExDocument)
    //{{AFX_MSG_MAP(CDxGraphDoc)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CDxGraphDoc 診断

#ifdef _DEBUG
void CDxGraphDoc::AssertValid() const
{
    CExDocument::AssertValid();
}

//
void CDxGraphDoc::Dump(CDumpContext& dc) const
{
    CExDocument::Dump(dc);
}
#endif //_DEBUG




/////////////////////////////////////////////////////////////////////////////
// CDxGraphDoc シリアライズ

void CDxGraphDoc::Serialize(CArchive& ar)
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
// CDxGraphDoc コマンド

/**
    機能：  画像データを読み込む．
    引数：  ファイル名
    戻値：  TRUE: 成功，FALSE: 失敗
    説明：  読み込んだグラフィックデータ(msGraphメンバ)と ヘッダ情報(cmnHeadメンバ)
            cmnHead.kind：
                HEADER_ERR →  データ読み込み失敗．
                その他 → CmnHead hd にデータが入る．
                    UN_KNOWN → ヘッダが解析できないのでファイルイメージのまま hd->grptrに格納される．
                                hd->kind, hd->lsize, hd->gptrのみ有効．MSGraph<sWord>.gp はNULL．
                    その他   → MSGraph<sWord>.gp にデータが入る．
                                cmnHead にはヘッダ情報のみが入り，cmnHead->buf, cmnHead->gptr はNULL．
                                CT_DATA, RAS_DATA, CT_3DX など．
*/
BOOL   CDxGraphDoc::ReadDataFile(LPCTSTR str)
{
    char* fname = ts2mbs(str);
    msGraph = readGraphicFile<sWord>(fname, &cmnHead);
    ::free(fname);
    infHead = getinfo_CmnHead(cmnHead);

    if (cmnHead.kind==HEADER_NONE) return  FALSE;
    return  TRUE;
}



BOOL  CDxGraphDoc::MakeViewData()
{
//  int kind = cmnHead.kind;
//  int chk  = kind & 0x00ff;

    if (!msGraph.isNull()) {
        freeCmnHead(&cmnHead);

        int mode = 0;
        if (msGraph.color==GRAPH_COLOR_MONO || msGraph.color==GRAPH_COLOR_MONO16 ||  // 表示は 8bit
            msGraph.color==GRAPH_COLOR_RGB  || msGraph.color==GRAPH_COLOR_BGR  || 
            msGraph.color==GRAPH_COLOR_XRGB || msGraph.color==GRAPH_COLOR_ARGB || msGraph.color==GRAPH_COLOR_RGBA ||
            msGraph.color==GRAPH_COLOR_XBGR || msGraph.color==GRAPH_COLOR_ABGR || msGraph.color==GRAPH_COLOR_BGRA) {
            // 8bit/pixcel
            mode = MG2CH_OCTET;
        }
        if (ctrlCntrst || msGraph.color==GRAPH_COLOR_MONO16) {
            mode |= MG2CH_CONTRAST;
        }

        //
        cmnHead = copyMSGraph2CmnHead(msGraph, mode);
        
        //
        cMax = vMax = msGraph.max;
        cMin = vMin = msGraph.min;
        if (msGraph.color==GRAPH_COLOR_RGB16 || msGraph.color==GRAPH_COLOR_ARGB16) {
            // 16bit/pixcel
            //vMax = cMax = UWORDMAX;
            //vMin = cMin = 0;
        }
        else if (!ctrlCntrst && cMin>=0 && cMax<=VIEW_8_MAX) {
            vMax = cMax = VIEW_8_MAX;
            vMin = cMin = 0;
        }
    }

    else {  //  DirectX9 サポートデータ（DirectX9に任せる）
        D3DXIMAGE_INFO imgInfo;
        HRESULT hr = D3DXGetImageInfoFromFileInMemory(cmnHead.grptr, cmnHead.lsize, &imgInfo);
        if (FAILED(hr)) {   
            freeNull(cmnHead.grptr);
            cmnHead.kind  = HEADER_NONE;
            return FALSE;
        }
        cmnHead.xsize = imgInfo.Width;
        cmnHead.ysize = imgInfo.Height;
        cmnHead.zsize = 1;
        cmnHead.depth = imgInfo.Format;
        cmnHead.bsize = sizeof(D3DXIMAGE_INFO);
        cmnHead.buf   = (uByte*)malloc(cmnHead.bsize);
        if (cmnHead.buf==NULL) {
            freeCmnHead(&cmnHead);
            return FALSE;
        }
        bcopy((char*)&imgInfo, (char*)cmnHead.buf, cmnHead.bsize);
    }
    return  TRUE;
}


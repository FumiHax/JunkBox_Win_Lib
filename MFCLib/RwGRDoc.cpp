//
// CRwGRDoc.cpp : インプリメンテーション ファイル
//

#include  "MFCBase.h"
#include  "RwGRDoc.h"

#include  "MessageBoxDLG.h"
#include  "ReadFilesDLG.h"
#include  "ProgressBarDLG.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CRwGRDoc

IMPLEMENT_DYNCREATE(CRwGRDoc, CExDocument)

CRwGRDoc::CRwGRDoc()
{
//  multiSliceData = FALSE;
//  nonCmnHead     = FALSE;
}


CRwGRDoc::~CRwGRDoc()
{
}


BEGIN_MESSAGE_MAP(CRwGRDoc, CExDocument)
    //{{AFX_MSG_MAP(CRwGRDoc)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CRwGRDoc 診断

#ifdef _DEBUG
void CRwGRDoc::AssertValid() const
{
    CExDocument::AssertValid();
}


//
void CRwGRDoc::Dump(CDumpContext& dc) const
{
    CExDocument::Dump(dc);
}
#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CRwGRDoc シリアライズ
//
void CRwGRDoc::Serialize(CArchive& ar)
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
// CRwGRDoc コマンド
//
BOOL  CRwGRDoc::ReadDataFile(LPCTSTR fname)
{
    CString message = _T("");
    CString fnm = get_file_name_t(fname);       // ファイル名のみ
    CString err_fname = _T("");                 // エラーを起こしたファイル名
    CmnHead hd;

    // グローバルカウンタの設定
    CProgressBarDLG* counter = NULL;
    counter = new CProgressBarDLG(IDD_PROGBAR, NULL, TRUE); // ディスパッチャー有効
    if (counter!=NULL) { 
        SetGlobalCounter(counter);
    }

    init_CmnHead(&hd);

    // マルチスライス読み込み
    if (multiSliceData) {
        // ファイル読み込み設定
        CReadFilesDLG* rsdlg = new CReadFilesDLG(fnm);
        if (rsdlg==NULL) return FALSE;
        if (rsdlg->DoModal()!=IDOK) {   // キャンセルボタン
            pFrame->cancelOperation  = TRUE;
            pFrame->doneErrorMessage = TRUE;
            delete (rsdlg);
            return FALSE;
        }
        startNo = rsdlg->fromNum;
        endNo   = rsdlg->toNum;
        CString mfname = rsdlg->fName;
        delete (rsdlg);
            
        if (counter!=NULL) {
            CString mesg;
            mesg.LoadString(IDS_STR_RDNG_FILE);
            char* mbstr = ts2mbs(mesg);
            counter->Start(0, mbstr);
            ::free(mbstr);
        }

        Title = mfname;
        char* fname_str = ts2mbs(mfname);

        // データの読み込み
        hd = SetHeaderDLG.getCmnHead();

        DEBUG_INFO("CRwGRDoc::ReadDataFile(): Multi Slice: file = %s, start = %d, end = %d\n", fname_str, startNo, endNo);
        msGraph = readGraphicSlices<sWord>(fname_str, startNo, endNo, &hd, true);
        
        if (msGraph.state>=0 && msGraph.state!=Xabs(endNo-startNo)+1) {
            char* numf = numbering_name(fname_str, startNo+msGraph.state-1+Sign(endNo-startNo));
            err_fname = mbs2ts(numf);
            ::free(numf);
            
            CString mesg, noti;
            noti.LoadString(IDS_STR_CNFRM_CNTNUE);
            mesg.LoadString(IDS_STR_ERR_OPEN_FILE);
            message = _T("CRwGRDoc::ReadDataFile():\n\n") + mesg + _T("  ") + err_fname;
            MessageBox(pFrame->m_hWnd, message, noti, MB_OK);
        }
        if (msGraph.state>0) msGraph.state = 0;

        if (msGraph.isNull() && msGraph.state==JBXL_GRAPH_HEADER_ERROR) {
            err_fname = numbering_name_t(mfname, msGraph.zs);
        }

        ::free(fname_str);
    }

    //  通常の読み込み．    
    else {
        CString mesg;
        mesg.LoadString(IDS_STR_RDNG_FILE);
        char* mbstr = ts2mbs(mesg);
        if (counter!=NULL) counter->Start(0, mbstr);
        ::free(mbstr);
        Title = fnm;
        hd = SetHeaderDLG.getCmnHead();

        char* fname_str = ts2mbs(fname);
        msGraph = readGraphicFile<sWord>(fname_str, &hd, true);
        ::free(fname_str);
    }

    //
    this->infHead   = getinfo_CmnHead(hd);
    this->colorMode = msGraph.color;
    free_CmnHead(&hd);

    // グローバルカウンタの削除
    if (counter!=NULL) {
        counter->Stop();
        ClearGlobalCounter();
        delete counter;
    }

    // エラー処理
    if (msGraph.isNull()) {
        CString mesg, noti;

        if (msGraph.state==JBXL_GRAPH_OPFILE_ERROR) {
            mesg.LoadString(IDS_STR_ERR_OPEN_FILE);
            noti.LoadString(IDS_STR_ERROR);
            if (multiSliceData) {
                message = _T("CRwGRDoc::ReadDataFile():\n\n") + mesg + _T("  ") + err_fname;
                MessageBox(pFrame->m_hWnd, message, noti, MB_OK);
            }
            else {
                MessageBox(pFrame->m_hWnd, _T("CRwGRDoc::ReadDataFile():\n\n") + mesg, noti, MB_OK);
            }
        }
        else if (msGraph.state==JBXL_GRAPH_MEMORY_ERROR) {
            mesg.LoadString(IDS_STR_ERR_MEMORY);
            noti.LoadString(IDS_STR_ERROR);
            MessageBox(pFrame->m_hWnd, _T("CRwGRDoc::ReadDataFile():\n\n") + mesg, noti,  MB_OK);
        }
        else if (msGraph.state==JBXL_GRAPH_CANCEL) {
            mesg.LoadString(IDS_STR_CANCEL_OPNFILE);
            noti.LoadString(IDS_STR_CANCEL);
            MessageBox(pFrame->m_hWnd, _T("CRwGRDoc::ReadDataFile():\n\n") + mesg, noti,  MB_OK);
            pFrame->cancelOperation = TRUE;
        }
        else {
            mesg.LoadString(IDS_STR_ERR_UNKWN);
            noti.LoadString(IDS_STR_ERROR);
            MessageBox(pFrame->m_hWnd, _T("CRwGRDoc::ReadDataFile():\n\n") + mesg, noti, MB_OK);
        }
        pFrame->doneErrorMessage = TRUE;

        return FALSE;
    }

    return  TRUE;
}


//
// hasViewData==TRUE なら MakeViewData()は実行されない．
//
BOOL  CRwGRDoc::MakeViewData()
{
    CString mesg, noti;

    // カウンタの設定
    CProgressBarDLG* counter = NULL;
    if (msGraph.zs > 10) { 
        //counter = new CProgressBarDLG(IDD_PROGBAR, NULL, NULL, FALSE);
        counter = new CProgressBarDLG(IDD_PROGBAR, NULL, TRUE); // ディスパッチャー有効
        if (counter!=NULL) {
            mesg.LoadString(IDS_STR_TRNS_DISPDATA);
            SetGlobalCounter(counter);
            char* mbstr = ts2mbs(mesg);
            counter->Start(0, mbstr);
            ::free(mbstr);
        }
    }

    // データ変換
    cmnHead = TranslateData();

    // カウンタの停止と削除
    if (counter!=NULL) {
        counter->Stop();
        ClearGlobalCounter();
        delete counter;
    }

    // エラー処理
    if (cmnHead.kind==HEADER_NONE) {
        if (cmnHead.xsize==JBXL_GRAPH_MEMORY_ERROR) {
            mesg.LoadString(IDS_STR_ERR_MEMORY);
            noti.LoadString(IDS_STR_ERROR);
            MessageBox(pFrame->m_hWnd, _T("CRwGRDoc::MakeViewData():\n\n") + mesg, noti, MB_OK); 
        }
        else if (cmnHead.xsize==JBXL_GRAPH_CANCEL) {
            mesg.LoadString(IDS_STR_CANCEL_TRNS_DATA);
            noti.LoadString(IDS_STR_CANCEL);
            MessageBox(pFrame->m_hWnd, _T("CRwGRDoc::MakeViewData():\n\n") + mesg, noti,  MB_OK);
            pFrame->cancelOperation = TRUE;
        }
        else {
            mesg.LoadString(IDS_STR_ERR_UNKWN);
            noti.LoadString(IDS_STR_ERROR);
            MessageBox(pFrame->m_hWnd, _T("CRwGRDoc::MakeViewData():\n\n") + mesg, noti,  MB_OK);
        }
        pFrame->doneErrorMessage = TRUE;
        return FALSE;
    }

    return  TRUE;
}


//
CmnHead CRwGRDoc::TranslateData()
{
    CmnHead chd;

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
    chd = copyMSGraph2CmnHead(msGraph, mode, true); // msGraphは参照渡し
    //
    cMax = vMax = msGraph.max;
    cMin = vMin = msGraph.min;

    if (msGraph.color==GRAPH_COLOR_RGB16 || msGraph.color==GRAPH_COLOR_ARGB16 
                                         || msGraph.color==GRAPH_COLOR_RGBA16) {
        // 16bit/pixcel
        //vMax = cMax = UWORDMAX;
        //vMin = cMin = 0;
    }
    else if (!ctrlCntrst && cMin>=0 && cMax<=VIEW_8_MAX) {
        vMax = cMax = VIEW_8_MAX;
        vMin = cMin = 0;
    }

    base = msGraph.base;
    
    return chd;
}

//
// ContrastDLG.cpp : インプリメンテーション ファイル
//

/**
@brief     コントラスト調整ダイアログ
@file      ContrastDLG.cpp
@author    Fumi.Iseki (C)
*/


#include  "ContrastDLG.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CContrastDLG ダイアログ

CContrastDLG::CContrastDLG(CExView* pview, BOOL rndr, CWnd* pParent /*=NULL*/)
    : CDialog(CContrastDLG::IDD, pParent)
{
    //{{AFX_DATA_INIT(CContrastDLG)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_DATA_INIT

    pView  = pview;
    render = rndr;

    oMin = cMin = pView->cMin;
    oMax = cMax = pView->cMax;
    vMin = pView->vMin;
    vMax = pView->vMax;
    base = pView->pDoc->base + TempBase;
}


void CContrastDLG::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CContrastDLG)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CContrastDLG, CDialog)
    //{{AFX_MSG_MAP(CContrastDLG)
    ON_EN_CHANGE(IDC_CNT_CNTMIN, OnChangeCntCntMin)
    ON_EN_CHANGE(IDC_CNT_CNTMAX, OnChangeCntCntMax)
    ON_WM_HSCROLL()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CContrastDLG メッセージ ハンドラ

BOOL   CContrastDLG::OnInitDialog()
{
    TCHAR buf[LNAME];

    cMinEBox = (CEdit*)GetDlgItem(IDC_CNT_CNTMIN);
    cMaxEBox = (CEdit*)GetDlgItem(IDC_CNT_CNTMAX);
    cMinSldr = (CSliderCtrl*)GetDlgItem(IDC_CNT_SLIDER_CNTMIN);
    cMaxSldr = (CSliderCtrl*)GetDlgItem(IDC_CNT_SLIDER_CNTMAX);

    cMinSldr->SetRange(vMin-base, vMax-base);
    cMaxSldr->SetRange(vMin-base, vMax-base);
    if (cMin-base==0) cMinSldr->SetPos(1);  // for スライダーバグ
    if (cMax-base==0) cMaxSldr->SetPos(1);  //     値が0の場合にスライダーの位置がおかしくなるバグ
    cMinSldr->SetPos(cMin-base);
    cMaxSldr->SetPos(cMax-base);

    sntprintf(buf, LNAME, _T("%d"), oMin-base);
    cMinEBox->SetWindowText(buf);

    sntprintf(buf, LNAME, _T("%d"), oMax-base);
    cMaxEBox->SetWindowText(buf);

    return TRUE;
}



void CContrastDLG::OnChangeCntCntMin() 
{
    TCHAR buf[LNAME];

    cMinEBox->GetWindowText(buf, LNAME);
    cMin = ttoi(buf) + base;

//  if (cMin<vMin || cMin>=vMax) {
//      if (cMin<vMin)  cMin = vMin;
//      if (cMin>=vMax) cMin = vMax - 1;
//      if (cMin==cMax) cMin = cMax - 1;
    if (cMin<vMin || cMin>vMax) {
        if (cMin<vMin)  cMin = vMin;
        if (cMin>vMax)  cMin = vMax;
        if (cMin==cMax) cMin = cMax - 1;
        sntprintf(buf, LNAME, _T("%d"), cMin-base);
        cMinEBox->SetWindowText(buf);
        cMinEBox->UpdateWindow();
    }

    cMinSldr->SetPos(cMin-base);
    pView->cMin = cMin;

    if (render) {
        pView->prevSBpos = -1;
        pView->SetNewSurface(0);
        pView->ExecRender();
    }
}



void CContrastDLG::OnChangeCntCntMax() 
{
    TCHAR buf[LNAME];

    cMaxEBox->GetWindowText(buf, LNAME);
    cMax = ttoi(buf) + base;

    //if (cMax<=vMin || cMax>vMax) {
    //  if (cMax>vMax)  cMax = vMax;
    //  if (cMax<=vMin) cMax = vMin + 1;
    //  if (cMax==cMin) cMax = cMin + 1;
    if (cMax<vMin || cMax>vMax) {
        if (cMax>vMax)  cMax = vMax;
        if (cMax<vMin)  cMax = vMin;
        if (cMax==cMin) cMax = cMin + 1;
        sntprintf(buf, LNAME, _T("%d"), cMax-base);
        cMaxEBox->SetWindowText(buf);
        cMaxEBox->UpdateWindow();
    }

    cMaxSldr->SetPos(cMax-base);
    pView->cMax = cMax;
    
    if (render) {
        pView->prevSBpos = -1;
        pView->SetNewSurface(0);
        pView->ExecRender();
    }
}



void CContrastDLG::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    TCHAR buf[LNAME];

    if (cMaxSldr==(CSliderCtrl*)pScrollBar) {
        cMax= cMaxSldr->GetPos() + base;
        pView->cMax = cMax;
        sntprintf(buf, LNAME, _T("%d"), cMax-base);
        cMaxEBox->SetWindowText(buf);
        cMaxEBox->UpdateWindow();
    }
    else {  //if (cMaxSldr==(CSliderCtrl*)pScrollBar) {
        cMin = cMinSldr->GetPos() + base;
        pView->cMin = cMin;
        sntprintf(buf, LNAME, _T("%d"), cMin-base);
        cMinEBox->SetWindowText(buf);
        cMinEBox->UpdateWindow();
    }

    /*
    if (render) {
        pView->prevSBpos = -1;
        pView->SetNewSurface(0);
        pView->ExecRender();
    }*/
    
//  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}



void CContrastDLG::OnOK() 
{
    // TODO: この位置にその他の検証用のコードを追加してください
    CDialog::OnOK();

    if (render) {
        pView->prevSBpos = -1;
        pView->SetNewSurface(0);
        pView->ExecRender();
    }
}



void CContrastDLG::OnCancel() 
{
    CDialog::OnCancel();

    pView->cMax = oMax;
    pView->cMin = oMin;

    if (render) {
        pView->prevSBpos = -1;
        pView->SetNewSurface(0);
        pView->ExecRender();
    }
}


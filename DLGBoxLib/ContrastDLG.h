#pragma once

/**
@brief     コントラスト調整ダイアログ
@file      ContrastDLG.h
@author    Fumi.Iseki (C)
*/


/**
pview->vMin 〜 pview->vMax の間で，pview->cMin, pview->cMin を変更する．

呼び出しウィンドウをレンダリングする場合は，pview->cMin, pview->cMinの値が変更される度に
呼び出しウィンドウを再描画する．

*/

#include  "DLGBoxBase.h"



//
namespace jbxwl {


/////////////////////////////////////////////////////////////////////////////
// CContrastDLG ダイアログ

class CContrastDLG : public CDialog
{
public:
	CExView*	 pView;
	BOOL		 render;
	
	int			 vMin, vMax;		// コントラストの上限と下限（固定）
	int			 oMin, oMax;		// 前のインスタンスの cMin, cMax
	int			 cMin, cMax;		// スライダーで調整する値（可変）
	int			 base;

	CEdit*		 cMaxEBox;
	CEdit*		 cMinEBox;

	CSliderCtrl* cMaxSldr;
	CSliderCtrl* cMinSldr;

// コンストラクション
public:

	// rndr : 呼び出しウィンドウをレンダリングするか？
	CContrastDLG(CExView* pview, BOOL rndr=TRUE, CWnd* pParent=NULL);

	virtual BOOL  OnInitDialog();

// ダイアログ データ
	//{{AFX_DATA(CContrastDLG)
	enum { IDD = IDD_CNTDLG };
	//}}AFX_DATA

// オーバーライド
protected:
	//{{AFX_VIRTUAL(CContrastDLG)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	//{{AFX_MSG(CContrastDLG)
	afx_msg void OnChangeCntCntMin();
	afx_msg void OnChangeCntCntMax();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}


}		// namespace

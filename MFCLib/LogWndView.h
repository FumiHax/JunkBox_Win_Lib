#pragma once

#include  "ExTextView.h"


#define  DISPLAY_MARGIN  5
#define  LINES_PER_PAGE  30


//
namespace jbxwl {


class  CLogWndDoc;
class  CLogWndFrame;



class CLogWndView : public CExTextView
{
protected:
    CLogWndView();
    DECLARE_DYNCREATE(CLogWndView)

private:
    BOOL    m_locked;

public:
    CLogWndDoc*    GetDocument() const;
    CLogWndDoc*    pDoc;
    CLogWndFrame*  pFrame;
    
    CString Title;
    void    SetTitle(LPCTSTR title); 

    void    lock(void)   { m_locked = TRUE;}
    void    unlock(void) { m_locked = FALSE;}

// オーバーライド
    //{{AFX_VIRTUAL(CLogWndView)
    virtual  BOOL   PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

protected:
    virtual  void   OnInitialUpdate();

public:
    virtual ~CLogWndView();

#ifdef _DEBUG
    virtual  void   AssertValid() const;
    virtual  void   Dump(CDumpContext& dc) const;
#endif

protected:
    int     m_mpress;
    int     m_msx;
    int     m_msy;
    int     m_mex;
    int     m_mey;
    int     m_dragsx;
    int     m_dragsy;
    int     m_dragex;
    int     m_dragey;

public:
    CFont   m_ScreenFont;
    int     m_FontSizeX;
    int     m_FontSizeY;

    int     m_ScrollSizeX;
    int     m_ScrollSizeY;
    int     m_DocLastPos;

    int     m_copysy;
    int     m_copyey;


    void    resetScrollSize(void);
    void    setScrollPosition(CDC* pDC, CLogRingBuffer* pBR);
    void    clearViewDoc(void);

    CString getCopyData(void);

    BOOL    OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll);

protected:
    //{{AFX_MSG(CLogWndView)
    afx_msg int  OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnDestroy(void);
    afx_msg void OnDraw(CDC* pDC);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:

};



#ifndef _DEBUG 

inline CLogWndDoc* CLogWndView::GetDocument() const { return reinterpret_cast<CLogWndDoc*>(m_pDocument); }

#endif



}       // namespace

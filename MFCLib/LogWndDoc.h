#pragma once

#include  <afxmt.h> 

#include  "ClassBox.h"
#include  "LogRingBuffer.h"
#include  "LogWndView.h"


#define  MAXBUFFERLINE 5000


//
namespace jbxwl {

using namespace jbxl;


class  CLogWndView;
class  CLogWndFrame;


class CLogWndDoc : public CDocument, public CVTextDocument
{
protected:
    CLogWndDoc();
    CLogWndDoc(int bufsz, BOOL binhex=TRUE);

    DECLARE_DYNCREATE(CLogWndDoc)

public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

    CLogWndFrame*  pFrame;
    CLogWndView*   pView;

    CString Title;
    BOOL    binHexMode;

protected:
//  BOOL    m_locked; 

public:
    virtual void  lprintBuffer(Buffer buf, int input=LOG_RB_MESG);
    virtual void  lprintString(char* str,  int input=LOG_RB_MESG);
    virtual void  lprintFormat(char* fmt, ...);
    virtual void  lfprintFormat(int input,  char* fmt, ...);

    virtual void  printBuffer(Buffer buf, int input=LOG_RB_MESG);
    virtual void  printString(char* str,  int input=LOG_RB_MESG);
    virtual void  printFormat(char* fmt, ...);
    virtual void  fprintFormat(int input, char* fmt, ...);

    virtual void  lock(void)   { m_locked = TRUE;  if (pView!=NULL) pView->lock();}
    virtual void  unlock(void) { m_locked = FALSE; if (pView!=NULL) pView->unlock();}

public:
    virtual ~CLogWndDoc();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    DECLARE_MESSAGE_MAP()


public:
    CString           save_fname;
    CLogRingBuffer*   ringBuffer;
    CCriticalSection  criticalKey;

public:
    void        free(void);
    void        clear(void);
    int         writeLogFile(void);
    CString     easyGetSaveFileName(LPCTSTR title, HWND hWnd);

    void        DeleteContents(void);
    CLogWndView*    GetView(void);
};


}       // namespace

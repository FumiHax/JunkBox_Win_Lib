//
// LogWndDoc.cpp : CLogWndDoc クラスの実装
//

#include  "MFCBase.h"
#include  "LogWndDoc.h"
#include  "LogWndFrame.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace jbxl;
using namespace jbxwl;


// CLogWndDoc

IMPLEMENT_DYNCREATE(CLogWndDoc, CDocument)

BEGIN_MESSAGE_MAP(CLogWndDoc, CDocument)
END_MESSAGE_MAP()


// CLogWndDoc コンストラクション/デストラクション

CLogWndDoc::CLogWndDoc()
{
    //DEBUG_INFO("CONSTRUCTOR: CLogWndDoc\n");

    pFrame     = NULL;
    pView      = NULL;
    Title      = _T("");
    save_fname = _T("");
    binHexMode = TRUE;
//  m_locked   = FALSE;

    ringBuffer = new CLogRingBuffer(MAXBUFFERLINE);
}



CLogWndDoc::CLogWndDoc(int bufsz, BOOL binhex)
{
    //DEBUG_INFO("CONSTRUCTOR: CLogWndDoc 2\n");

    pFrame     = NULL;
    pView      = NULL;
    Title      = _T("");
    save_fname = _T("");
    binHexMode = binhex;

    if (bufsz<=0) bufsz = MAXBUFFERLINE;
    ringBuffer = new CLogRingBuffer(bufsz);
}



CLogWndDoc::~CLogWndDoc()
{
    DEBUG_INFO("DESTRUCTOR: CLogWndDoc: START\n");

    while(m_locked) ::Sleep(10);
    if (pView!=NULL) pView->unlock();

    CLogWndDoc::free();

    DEBUG_INFO("DESTRUCTOR: CLogWndDoc: END\n");
}



void  CLogWndDoc::free(void)
{
    if (ringBuffer!=NULL) {
        delete(ringBuffer);
        ringBuffer = NULL;
    }
}



BOOL CLogWndDoc::OnNewDocument()
{
    //DEBUG_INFO("CLogWndDoc::OnNewDocument():\n");
    if (!CDocument::OnNewDocument()) return FALSE;
    return TRUE;
}




///////////////////////////////////////////////////////////////////////////////
// CLogWndDoc シリアル化

void CLogWndDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: 格納するコードをここに追加してください．
    }
    else
    {
        // TODO: 読み込むコードをここに追加してください．
    }
}




///////////////////////////////////////////////////////////////////////////////
// CLogWndDoc 診断

#ifdef _DEBUG
void CLogWndDoc::AssertValid() const
{
    CDocument::AssertValid();
}


void CLogWndDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG




///////////////////////////////////////////////////////////////////////////////
// CLogWndDoc コマンド

void CLogWndDoc::DeleteContents(void)
{
    //CLogWndDoc::clear();
    //DEBUG_WARN("CLogWndDoc::DeleteContents: IN\n");
    CDocument::DeleteContents();
}



CLogWndView*  CLogWndDoc::GetView(void)
{
    //DEBUG_INFO("CLogWndDoc::GetView()：\n");
    POSITION pos = GetFirstViewPosition();
    while (pos!=NULL) {
        CLogWndView* pview = (CLogWndView*)GetNextView(pos);
        if (this==pview->GetDocument()) return pview;
    }
    return NULL;
}



void  CLogWndDoc::clear(void)
{
    save_fname = _T("");

    CSingleLock lock(&criticalKey);
    lock.Lock();
    while (!lock.IsLocked()) {
        Sleep(100);
        lock.Lock();
    }
    
    ringBuffer->clear();
    lock.Unlock();

    return;
}




///////////////////////////////////////////////////////////////////////////////

int   CLogWndDoc::writeLogFile(void)
{
    if (save_fname==_T("")) return -1;

    CSingleLock lock(&criticalKey);
    lock.Lock();
    while (!lock.IsLocked()) {
        Sleep(100);
        lock.Lock();
    }
    
    int size = 0;
    FILE* fp = tfopen(save_fname, _T("wb"));
    if (fp==NULL) {
        lock.Unlock();
        return -2;
    }

    for (int pos=0; pos<ringBuffer->getMaxLineY(); pos++) {
        Buffer buf = dup_Buffer(ringBuffer->pBuf[pos]);
        if (ringBuffer->getKindData(pos)!=LOG_RB_BINARY_DATA) {
            if (buf.buf[buf.vldsz-1]!='\n') cat_s2Buffer("\n", &buf);
        }
        fwrite((const char*)buf.buf, strlen((const char*)buf.buf), 1, fp);  // buf may be binary
        size += (int)strlen((const char*)buf.buf);
        free_Buffer(&buf);
    }
    fclose(fp);
    lock.Unlock();

    return size;
}



CString  CLogWndDoc::easyGetSaveFileName(LPCTSTR title, HWND hWnd) 
{   
    OPENFILENAME  ofn;
    TCHAR fn[LNAME];
    CString str = _T(""); 

    memset(fn, 0, LNAME);
    memset(&ofn, 0, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.Flags = 0;
    ofn.lpstrFile = fn;
    ofn.nMaxFile  = LNAME;
    ofn.lpstrTitle = title;

    BOOL ret = GetSaveFileName(&ofn);
    if (ret) str = fn;

    return str;
}




//////////////////////////////////////////////////////////////////////////////////////
// Print with Lock
//

void  CLogWndDoc::lprintBuffer(Buffer buf, int input)
{
    if (ringBuffer==NULL || pView==NULL || buf.buf==NULL) return;

    int no = 0, trymax = 100;

    CSingleLock lock(&criticalKey);
    lock.Lock();
    while (!lock.IsLocked() && no<trymax) {
        Sleep(100);
        lock.Lock();
        no++;
    }

    ringBuffer->putRingBuffer(buf, input);
    
    int lastPos = ringBuffer->getLastPosition();
    if (binHexMode && ringBuffer->getKindData(lastPos-1)==LOG_RB_BINARY_DATA) {
        ringBuffer->rewriteBinHexRingBuffer(lastPos-1, input);
    }
    lock.Unlock();

    InvalidateRect(pView->m_hWnd, NULL, FALSE);

    return;
}



void  CLogWndDoc::lprintString(char* msg, int input)
{
    if (ringBuffer==NULL || pView==NULL || msg==NULL) return;

    Buffer buf = make_Buffer_bystr(msg);
    printBuffer(buf, input);
    free_Buffer(&buf);

    return;
}



void  CLogWndDoc::lfprintFormat(int input, char* fmt, ...)
{
    if (ringBuffer==NULL || pView==NULL || fmt==NULL) return;

    int no = 0, trymax = 100;

    CSingleLock lock(&criticalKey);
    lock.Lock();
    while (!lock.IsLocked() && no<trymax) {
        Sleep(100);
        lock.Lock();
        no++;
    }

    va_list  args;
    va_start(args, fmt);
    ringBuffer->putRingFormat(input, fmt, args);
    va_end(args);
    
    lock.Unlock();

    InvalidateRect(pView->m_hWnd, NULL, FALSE);

    return;
}



void  CLogWndDoc::lprintFormat(char* fmt, ...)
{
    if (ringBuffer==NULL || pView==NULL || fmt==NULL) return;

    int no = 0, trymax = 100;

    CSingleLock lock(&criticalKey);
    lock.Lock();
    while (!lock.IsLocked() && no<trymax) {
        Sleep(100);
        lock.Lock();
        no++;
    }

    va_list  args;
    va_start(args, fmt);
    ringBuffer->putRingFormat(LOG_RB_MESG, fmt, args);
    va_end(args);
    
    lock.Unlock();

    InvalidateRect(pView->m_hWnd, NULL, FALSE);

    return;
}




///////////////////////////////////////////////////
// Print without Lock

void  CLogWndDoc::printBuffer(Buffer buf, int input)
{
    if (ringBuffer==NULL || pView==NULL || buf.buf==NULL) return;

    ringBuffer->putRingBuffer(buf, input);
    
    int lastPos = ringBuffer->getLastPosition();
    if (binHexMode && ringBuffer->getKindData(lastPos-1)==LOG_RB_BINARY_DATA) {
        ringBuffer->rewriteBinHexRingBuffer(lastPos-1, input);
    }

    InvalidateRect(pView->m_hWnd, NULL, FALSE);

    return;
}



void  CLogWndDoc::printString(char* msg, int input)
{
    if (ringBuffer==NULL || pView==NULL || msg==NULL) return;

    Buffer buf = make_Buffer_bystr(msg);
    printBuffer(buf, input);
    free_Buffer(&buf);

    return;
}



void  CLogWndDoc::fprintFormat(int input, char* fmt, ...)
{
    if (ringBuffer==NULL || pView==NULL || fmt==NULL) return;

    va_list  args;
    va_start(args, fmt);
    ringBuffer->putRingFormat(input, fmt, args);
    va_end(args);
    
    InvalidateRect(pView->m_hWnd, NULL, FALSE);

    return;
}



void  CLogWndDoc::printFormat(char* fmt, ...)
{
    if (ringBuffer==NULL || pView==NULL || fmt==NULL) return;

    va_list  args;
    va_start(args, fmt);
    ringBuffer->putRingFormat(LOG_RB_MESG, fmt, args);
    va_end(args);
    
    InvalidateRect(pView->m_hWnd, NULL, FALSE);

    return;
}


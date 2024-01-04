
#include "WinSMTool.h"
#include "WinTools.h"


using namespace jbxwl;


CWinSharedMem::CWinSharedMem(void)
{
    CWinSharedMem(JBXWL_DEFAULT_SMNAME, JBXWL_DEFAULT_SMSZIE);
}


CWinSharedMem::CWinSharedMem(const char* name, int size)
{
    buf = new_Buffer(JBXWL_DEFAULT_SMSZIE + 1);

    TCHAR sznm[] = _T("_SZ");

    m_hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, mbs2ts((char*)name));
    if (m_hMapping == NULL) {
        // メモリマップドファイルの生成
        m_hMapping = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, mbs2ts((char*)name));
    }
    if (m_hMapping != NULL) {
        // プロセス内のアドレス空間にファイルのビューをマップ
        m_pMappingView = ::MapViewOfFile(m_hMapping, FILE_MAP_ALL_ACCESS, 0, 0, size);
        // ミューテックスオブジェクトの生成
        m_pMutex = new CMutex(FALSE, mbs2ts((char*)name) + mbs2ts("_mutex"));
    }

    // サイズ用 4byte
    m_hMapping_sz = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, mbs2ts((char*)name) + sznm);
    if (m_hMapping_sz == NULL) {
        m_hMapping_sz = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, mbs2ts((char*)name) + sznm);
    }
    if (m_hMapping_sz != NULL) {
        m_pMappingView_sz = ::MapViewOfFile(m_hMapping_sz, FILE_MAP_ALL_ACCESS, 0, 0, 4);
    }
}


CWinSharedMem::~CWinSharedMem(void)
{
    if (m_pMappingView != NULL) ::UnmapViewOfFile(m_pMappingView);
    if (m_hMapping != NULL) ::CloseHandle(m_hMapping);
    if (m_pMappingView_sz != NULL) ::UnmapViewOfFile(m_pMappingView_sz);
    if (m_hMapping_sz != NULL) ::CloseHandle(m_hMapping_sz);
    if (m_pMutex!=NULL) delete m_pMutex;

    m_pMappingView = NULL;
    m_hMapping = NULL;
    m_pMappingView_sz = NULL;
    m_hMapping_sz = NULL;
    m_pMutex = NULL;

    del_Buffer(&buf);
}


// 共有メモリの内容を取得
int  CWinSharedMem::get()
{
    int sz, cc;

    m_pMutex->Lock(INFINITE);
    {
        memcpy(&sz, m_pMappingView_sz, 4);
        if (sz > 0) {
            copy_b2Buffer(m_pMappingView, buf, sz);
            cc = 0;
            memcpy(m_pMappingView_sz, &cc, 4);
        }
    }
    m_pMutex->Unlock();

    return sz;
}


// 共有メモリへ書き込む
void  CWinSharedMem::put(int sz)
{
    int seeksz = 0;
    if (sz<=0 || sz>JBXWL_DEFAULT_SMSZIE) sz = (int)strlen((const char*)buf) + 1;

    m_pMutex->Lock(INFINITE);
    {
        memcpy(&seeksz, m_pMappingView_sz, 4);
        memcpy(((unsigned char*)m_pMappingView + seeksz), buf->buf, buf->vldsz);
        sz += seeksz;
        memcpy(m_pMappingView_sz, &sz, 4);
    }
    m_pMutex->Unlock();

    return;
}


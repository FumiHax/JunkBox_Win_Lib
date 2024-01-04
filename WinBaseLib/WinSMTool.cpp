
#include "WinSMTool.h"
#include "WinTools.h"


using namespace jbxwl;


CWinSharedMemory::CWinSharedMemory(void)
{
    CWinSharedMemory(JBXWL_DEFAULT_SMNAME, JBXWL_DEFAULT_SMSZIE);
}


CWinSharedMemory::CWinSharedMemory(const char* name, int size)
{
    memset(buf, 0, JBXWL_DEFAULT_SMSZIE);

    m_hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, mbs2ts((char*)name));

    if (m_hMapping == NULL) {
        // メモリマップドファイルの生成
        m_hMapping = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, mbs2ts((char*)name));
    }

    if (m_hMapping != NULL) {
        // プロセス内のアドレス空間にファイルのビューをマップ
        m_pMappingView = ::MapViewOfFile(m_hMapping, FILE_MAP_ALL_ACCESS, 0, 0, size);
        // ミューテックスオブジェクトの生成
        m_pMutex = new CMutex(FALSE, mbs2ts((char*)name)+mbs2ts("_mutex"));
    }
}


CWinSharedMemory::~CWinSharedMemory(void)
{
    if (m_pMappingView!=NULL) ::UnmapViewOfFile(m_pMappingView);
    if (m_hMapping!=NULL) ::CloseHandle(m_hMapping);
    if (m_pMutex!=NULL) delete m_pMutex;

    m_pMappingView = NULL;
    m_hMapping = NULL;
    m_pMutex = NULL;
}


void  CWinSharedMemory::get(int sz)
{
    if (sz<=0 || sz>JBXWL_DEFAULT_SMSZIE) sz = JBXWL_DEFAULT_SMSZIE;
    memset(buf, 0, sz + 1);

    // 共有メモリの内容を取得
    m_pMutex->Lock(INFINITE);
    memcpy(buf, m_pMappingView, sz);
    m_pMutex->Unlock();

    //UpdateData(FALSE);
}


void  CWinSharedMemory::put(int sz)
{
    if (sz<=0 || sz>JBXWL_DEFAULT_SMSZIE) sz = (int)strlen((const char*)buf);
    buf[sz] = 0;

    //UpdateData(TRUE);

    // 共有メモリへ書き込む
    m_pMutex->Lock(INFINITE);
    memcpy(m_pMappingView, buf, sz + 1);
    m_pMutex->Unlock();
}


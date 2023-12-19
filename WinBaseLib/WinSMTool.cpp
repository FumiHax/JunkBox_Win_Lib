
#include "WinSMTool.h"
#include "WinTools.h"


using namespace jbxwl;


CWinSharedMemory::CWinSharedMemory(void)
{
    CWinSharedMemory(JBXWL_DEFAULT_SMNAME, 1024, FALSE);
}



CWinSharedMemory::CWinSharedMemory(const char* name, int size, BOOL create)
{
    m_hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, mbs2ts((char*)name));

    if (m_hMapping == NULL && create) {
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
    BOOL b = ::UnmapViewOfFile(m_pMappingView);
    ::CloseHandle(m_hMapping);

    delete m_pMutex;
}











void  CWinSharedMemory::get()
{
    // 共有メモリの内容を取得
    m_pMutex->Lock(INFINITE);
    //m_strEdit1 = (LPTSTR)m_pMappingView;
    m_pMutex->Unlock();

    //UpdateData(FALSE);
}



void  CWinSharedMemory::put()
{
    //UpdateData(TRUE);

    // 共有メモリへ書き込む
    m_pMutex->Lock(INFINITE);
    //memcpy(m_pMappingView, (LPCTSTR)m_strEdit1, m_strEdit1.GetLength() + 1);
    m_pMutex->Unlock();
}


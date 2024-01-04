#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_MAXVER
#endif

#include   <afxmt.h>

#define  JBXWL_DEFAULT_SMNAME  "JBXWL_WinSharedMemory"
#define  JBXWL_DEFAULT_SMSZIE  2048


//
namespace jbxwl {


class CWinSharedMemory
{
public:
    CWinSharedMemory(void);
    CWinSharedMemory(const char* name, int size = JBXWL_DEFAULT_SMSZIE);

    virtual ~CWinSharedMemory(void);

protected:
    HANDLE  m_hMapping      = NULL;     // メモリマップドファイルのハンドル
    void*   m_pMappingView  = NULL;     // ファイルのビューへのポインタ
    CMutex* m_pMutex        = NULL;     // ミューテックスオブジェクト

    void    get();
    void    put();

public:
    unsigned char buf[JBXWL_DEFAULT_SMSZIE + 1];
};


}       // namespace

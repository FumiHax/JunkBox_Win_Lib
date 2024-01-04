#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_MAXVER
#endif


#include  <afxmt.h>
#include  "buffer.h"

#define  JBXWL_DEFAULT_SMNAME  "JBXWL_WinSharedMemory"
#define  JBXWL_DEFAULT_SMSZIE  4096


//
namespace jbxwl {


class CWinSharedMem
{
public:
    CWinSharedMem(void);
    CWinSharedMem(const char* name, int size = JBXWL_DEFAULT_SMSZIE);
    virtual ~CWinSharedMem(void);

protected:
    HANDLE  m_hMapping        = NULL;     // メモリマップドファイルのハンドル
    HANDLE  m_hMapping_sz     = NULL;     // サイズ用メモリマップドファイルのハンドル
    void*   m_pMappingView    = NULL;     // ファイルのビューへのポインタ
    void*   m_pMappingView_sz = NULL;     // サイズ用ファイルのビューへのポインタ
    CMutex* m_pMutex          = NULL;     // ミューテックスオブジェクト

public:
    int     get();
    void    put(int sz);
    Buffer* buf;
};


}       // namespace

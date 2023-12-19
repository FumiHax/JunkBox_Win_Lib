#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_MAXVER
#endif

#include   <afxmt.h>


#define  JBXWL_DEFAULT_SMNAME  "JBXWL_WinSharedMemory"



//
namespace jbxwl {


class CWinSharedMemory
{
public:
    CWinSharedMemory(void);
    CWinSharedMemory(const char* name);
    CWinSharedMemory(const char* name, int size=1024, BOOL create=FALSE);

    virtual ~CWinSharedMemory(void);

protected:
    HANDLE      m_hMapping      = NULL;     // メモリマップドファイルのハンドル
    void*       m_pMappingView  = NULL;     // ファイルのビューへのポインタ
    CMutex*     m_pMutex        = NULL;     // ミューテックスオブジェクト

    void        createMap();

    void        get();
    void        put();

};


}       // namespace

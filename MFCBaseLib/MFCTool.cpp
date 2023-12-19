//
// MFCTool.cpp : 実装ファイル
//

#include "MFCTool.h"


using namespace jbxwl;


////////////////////////////////////////////////////////////////////////////////////////
// MENU Tool
//

/**
    メニューでリソースID id を持つものを探す．
*/
CMenu*  jbxwl::GetMenu_byID(CMenu* pMenu, UINT id)
{
    if (pMenu==NULL) return NULL;

    UINT n = pMenu->GetMenuItemCount();
    for (UINT i=0; i<n; i++){
        if (pMenu->GetMenuItemID(i)==id) return pMenu;

        CMenu* pSub = GetMenu_byID(pMenu->GetSubMenu(i), id);
        if (pSub!=NULL) return pSub;
    }
    return NULL;
}



CMenu*  jbxwl::GetMenu_byStringID(CMenu* pMenu, UINT id)
{
    if (pMenu==NULL) return NULL;

    CString str = LoadString_byID(id);
    if (str=="" || str.IsEmpty()) return NULL;

    return GetMenu_byName(pMenu, str);
}



CMenu*  jbxwl::GetMenu_byName(CMenu* pMenu, LPCTSTR name)
{
    if (pMenu==NULL) return NULL;
    CString str;

    UINT n = pMenu->GetMenuItemCount();
    for (UINT i=0; i<n; i++){
        pMenu->GetMenuString(i, str, MF_BYPOSITION);
        if (str==(CString)name) return pMenu->GetSubMenu(i);

        CMenu* pSub = GetMenu_byName(pMenu->GetSubMenu(i), name);
        if (pSub!=NULL) return pSub;
    }
    return NULL;
}




////////////////////////////////////////////////////////////////////////////////////////
// プログラムの再起動
//

void  jbxwl::RebootProgram(void)
{
    TCHAR path[LDATA];
    GetModuleFileName(NULL, (LPTSTR)path, (DWORD)LDATA);

    CString arg;
    arg.Format(_T("/waitpid %u"), GetCurrentProcessId());
    ShellExecute(NULL, NULL, (LPCTSTR)path, (LPCTSTR)arg, NULL, SW_SHOWNORMAL);

    SendWinMessage(WM_CLOSE);
}



BOOL  jbxwl::WaitPidTerm(int tm)
{
    DWORD pid = 0;

    for (int i=1; i<__argc; i++) {
        if (!tcscmp(__targv[i], _T("/waitpid"))) {
            if (i+1<__argc) {
                pid = ttoi(__targv[i+1]);
                break;
            }
        }
    }
    if (pid==0) return TRUE;

    HANDLE hdl = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (hdl!=NULL) {
        if (WaitForSingleObject(hdl, tm*1000)==WAIT_TIMEOUT) return FALSE;
    }

    return TRUE;
}


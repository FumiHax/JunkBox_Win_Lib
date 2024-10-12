/**
  Windows用 Tool

*/

//
#include  "..\stdafx.h"

#include  "WinTools.h"


using namespace jbxl;
using namespace jbxwl;


////////////////////////////////////////////////////////////////////////////////////////
// Uicode

/////////////////////////////////////////////////////////////////
// 変換
//      locale が正しくないと，日本語の変換は失敗する．
//

Buffer  jbxwl::String2Buffer(CString str)
{
    Buffer buf = make_Buffer(CStringA(str).GetLength());

    copy_b2Buffer(CStringA(str).GetBuffer(), &buf, CStringA(str).GetLength());
    return buf;
}



Buffer  jbxwl::tc2Buffer(TCHAR* tchar, int size)
{
    if (size<0) size = LDATA;
    Buffer buf = make_Buffer(size);

#ifdef _UNICODE
    //setlocale(LC_ALL, ".UTF8");

    size_t len;
    int err = wcstombs_s(&len, (char*)buf.buf, buf.bufsz, tchar, _TRUNCATE);

    if (err!=0) clear_Buffer(&buf);
    else buf.vldsz = (int)Min(len-1, strlen((char*)buf.buf));
    buf.state = err;
#else
    copy_s2Buffer(tchar, &buf);
#endif

    return buf;
}


Buffer  jbxwl::ts2Buffer(LPCTSTR str, int size)
{
    if (size<0) size = (int)tcslen(str);

#ifdef _UNICODE
    //setlocale(LC_ALL, ".UTF8");

    Buffer buf = make_Buffer((size+1)*4);
    size_t len;

    int err = wcstombs_s(&len, (char*)buf.buf, buf.bufsz, str, _TRUNCATE);
    if (err!=0) clear_Buffer(&buf);
    else buf.vldsz = (int)Min(len-1, strlen((char*)buf.buf));
    buf.state = err;
#else
    Buffer buf = make_Buffer(size);
    copy_b2Buffer((char*)str, &buf, size);
#endif

    return buf;
}


CString  jbxwl::mbs2ts(char* str)
{
    CString buf = _T("");
    if (str==NULL) return buf;

#ifdef _UNICODE
    //setlocale(LC_ALL, ".UTF8");
    TCHAR  tchar[SBUFSZ];
    size_t len = 0;
    size_t size = strlen(str);
    int err = -1;

    if (size + 1 > SBUFSZ) {
        TCHAR tchar_recv[RECVBUFSZ];
        if (size + 1 > RECVBUFSZ) {
            size_t ptchar_sz = (size + 1) * sizeof(TCHAR);
            TCHAR* ptchar = (TCHAR*)malloc(ptchar_sz);
            err = mbstowcs_s(&len, ptchar, ptchar_sz, str, size);
            if (err == 0) buf = ptchar;
            ::free(ptchar);
        }
        else {
            err = mbstowcs_s(&len, tchar_recv, RECVBUFSZ, str, size);
            if (err == 0) buf = tchar_recv;
        }
    }
    else {
        err = mbstowcs_s(&len, tchar, SBUFSZ, str, size);
        if (err == 0) buf = tchar;
    }
#else
    buf = (LPTSTR)str;
#endif

    return buf;
}


//
// 要 free.  NULL は絶対に返ってこない
// 
/*
inline char*  ts2mbs(LPCTSTR str)
{
    Buffer buf = ts2Buffer(str);
    return (char*)buf.buf;
}
*/


/**
int  jbxwl::copy_ts2Buffer(LPCTSTR str, Buffer* buf)

@retval 0以上  dstのバッファにコピーされた文字数．
@retval  -1    srcまたは dstが NULL
@retval  -2    メモリの確保に失敗した．何も実行されなかった．
*/
int  jbxwl::copy_ts2Buffer(LPCTSTR str, Buffer* buf)
{
    if (buf==NULL) return -1;
    int cc = 0;

#ifdef _UNICODE
    char* mbstr = ts2mbs(str);
    copy_s2Buffer(mbstr, buf);
    ::free(mbstr);
    cc = buf->vldsz;
#else
    cc = copy_s2Buffer((char*)str, buf);
#endif

    return cc;
}


/**
int  jbxwl::cat_ts2Buffer(LPCTSTR str, Buffer* buf)

@retval 0以上  dstのバッファにコピーされた文字数．
@retval  -1    srcまたは dstが NULL 
@retval  -2    メモリの確保に失敗した．何も実行されなかった．
*/
int  jbxwl::cat_ts2Buffer(LPCTSTR str, Buffer* buf)
{
    if (buf==NULL) return -1;
    int cc = 0;

#ifdef _UNICODE
    char* mbstr = ts2mbs(str);
    cat_s2Buffer(mbstr, buf);
    ::free(mbstr);
    cc = buf->vldsz;
#else
    cc = cat_s2Buffer((char*)str, buf);
#endif

    return cc;
}




/////////////////////////////////////////////////////////////////
// UTF-8, s-Jis


Buffer  jbxwl::utf8_to_sjis_byStr(CString str)
{
    Buffer buf = init_Buffer();
    if (str.IsEmpty()) return buf;

    buf = utf8_to_sjis(CStringA(str).GetBuffer(), CStringA(str).GetLength());

    return buf;
}


Buffer  jbxwl::utf8_to_sjis(void* ptr, size_t len)
{
    // utf-8 -> unicode
    int len_unic = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)ptr, (int)len, NULL, 0);
    wchar_t* buf_unic = new wchar_t[len_unic];
    MultiByteToWideChar(CP_UTF8, 0, (LPCCH)ptr, (int)len + 1, buf_unic, len_unic);

    // unicode -> sjis
    int len_sjis = WideCharToMultiByte(CP_THREAD_ACP, 0, buf_unic, -1, NULL, 0, NULL, NULL);
    Buffer buf_sjis = make_Buffer(len_sjis);
    WideCharToMultiByte(CP_THREAD_ACP, 0, buf_unic, len_unic, (LPSTR)buf_sjis.buf, len_sjis, NULL, NULL);

    delete buf_unic;

    buf_sjis.vldsz = len_sjis;
    return buf_sjis;
}



Buffer  jbxwl::sjis_to_utf8_byStr(CString str)
{
    Buffer buf = init_Buffer();
    if (str.IsEmpty()) return buf;

    buf = sjis_to_utf8(CStringA(str).GetBuffer(), CStringA(str).GetLength());

    return buf;
}


Buffer  jbxwl::sjis_to_utf8(void* ptr, size_t len)
{
    // sjis -> unicode
    int len_unic = MultiByteToWideChar(CP_THREAD_ACP, 0, (LPCCH)ptr, (int)len, NULL, 0);
    wchar_t* buf_unic = new wchar_t[len_unic];
    MultiByteToWideChar(CP_THREAD_ACP, 0, (LPCCH)ptr, (int)len + 1, buf_unic, len_unic);

    // unicode -> utf-8
    int len_utf8 = WideCharToMultiByte(CP_UTF8, 0, buf_unic, -1, NULL, 0, NULL, NULL);
    Buffer buf_utf8 = make_Buffer(len_utf8);
    WideCharToMultiByte(CP_UTF8, 0, buf_unic, len_unic, (LPSTR)buf_utf8.buf, len_utf8, NULL, NULL);

    delete buf_unic;

    buf_utf8.vldsz = len_utf8;
    return buf_utf8;
}



/////////////////////////////////////////////////////////////////
// File

unsigned long int  jbxwl::file_size_t(LPCTSTR fn)                                             
{                                     
    struct _stat stbuf;                                                                                                    
    _tstat(fn, &stbuf);
    return stbuf.st_size;
}


BOOL  jbxwl::file_exist_t(LPCTSTR fn)
{
    BOOL ret = FALSE;
    if (fn==NULL) return FALSE;

#ifdef _UNICODE
    FILE* fp = tfopen(fn, _T("rb"));
    if (fp!=NULL) {
        ret = TRUE;
        fclose(fp);
    }
#else
    ret = file_exist((char*)fn);
#endif

    return ret;
}


CString  jbxwl::get_file_name_t(LPCTSTR str)
{
    CString buf = _T("");

#ifdef _UNICODE
    char* mbstr = ts2mbs(str);
    char* fn = get_file_name(mbstr);
    buf = mbs2ts(fn);
    ::free(mbstr);
#else
    char* fn = get_file_name((char*)str);
    buf = (LPTSTR)fn;
#endif

    return buf;
}


CString  jbxwl::make_file_path_t(LPCTSTR str)
{
    CString buf = _T("");

#ifdef _UNICODE
    char* mbstr = ts2mbs(str);
    //char* pn = make_file_path(mbstr);
    char* pn = get_file_path(mbstr);
    buf = mbs2ts(pn);
    ::free(mbstr);
    ::free(pn);
#else
    //char* pn = make_file_path((char*)str);
    char* pn = get_file_path((char*)str);
    buf = (LPTSTR)pn;
    ::free(pn);
#endif

    return buf;
}


CString jbxwl::cut_file_extension_t(LPCTSTR str)
{
    CString buf = _T("");

#ifdef _UNICODE
    char* mbstr = ts2mbs(str);
    char* pn = cut_file_extension(mbstr);   // 要free
    buf = mbs2ts(pn);
    ::free(mbstr);
    ::free(pn);
#else
    char* pn = cut_file_extension((char*)str);
    buf = (LPTSTR)pn;
    ::free(pn);
#endif

    return buf;
}



tList*  jbxwl::get_dir_files_t(LPCTSTR dirn)
{
    tList* lp = NULL;
    tList* ln = NULL;

    WIN32_FIND_DATA  FindFileData;
    HANDLE hFind;
    CString dirstr = dirn;

    if (dirstr.Right(1)!="\\") dirstr += _T("\\*"); 
    hFind = FindFirstFile((LPCTSTR)dirstr, &FindFileData);
    if (hFind!=INVALID_HANDLE_VALUE) {
        Buffer tmp; 
        do {
            char* fn = ts2mbs(FindFileData.cFileName);
            if (strcmp(".", fn) && strcmp("..", fn)) { 
                tmp = ts2Buffer(dirn);
                if (tmp.buf[tmp.vldsz-1]!='\\') cat_s2Buffer("\\", &tmp);
                cat_ts2Buffer(FindFileData.cFileName, &tmp);
                ln = add_tList_node_str(ln, NULL, tmp.buf);
                if (lp==NULL) lp = ln;
                free_Buffer(&tmp);
            }
            ::free(fn);
        } while (FindNextFile(hFind, &FindFileData));

        FindClose(hFind);
    }
    return lp;
}


tList*  jbxwl::get_dir_files_rcsv_t(LPCTSTR dirn)
{
    tList* lp = get_dir_files_t(dirn);
    if (lp==NULL) return NULL;

    tList* lt = dup_tList(lp);
    while (lt!=NULL) {
        tList* lc = get_dir_files_rcsv_t(mbs2ts((char*)lt->ldat.val.buf));
        if (lc!=NULL) add_tList_end(lp, lc);
        lt = lt->next;
    }
    del_tList(&lt);

    return lp;
}


tList*  jbxwl::read_index_tList_file_t(LPCTSTR fn, char cc)
{
    char* pathstr = NULL;
    if (fn!=_T("")) pathstr = ts2mbs(fn);
    
    tList* lt = read_index_tList_file(pathstr, cc);
    freeNull(pathstr);

    return lt;
}



/////////////////////////////////////////////////////////////////
// Tools

CString  jbxwl::numbering_name_t(LPCTSTR fmt, int n)
{
    TCHAR  fname[LNAME];

    sntprintf(fname, LNAME-1, fmt, n);  
    CString ret = fname;
    return ret;
}


CString  jbxwl::get_tstr_param_tList(tList* lt, char* key, LPCTSTR dflt)
{
    Buffer buf = buffer_key_tList(lt, key, 1);
    if (buf.buf!=NULL) {
        CString ret = mbs2ts((char*)buf.buf);
        free_Buffer(&buf);
        return ret;
    }
    return (CString)dflt;
}



/////////////////////////////////////////////////////////////////
// locale

void  jbxwl::setResourceLocale(void)
{
    CString locale = LoadString_byID(IDS_STR_LOCALE);
    _tsetlocale(LC_ALL, (LPCTSTR)locale);
    return;
}


void  jbxwl::setSystemLocale(LPCTSTR locale)
{
    _tsetlocale(LC_ALL, locale);
    return;
}


CString  jbxwl::getResourceLocale(void)
{
    CString locale = LoadString_byID(IDS_STR_LOCALE);
    return locale;
}



////////////////////////////////////////////////////////////////////////////////////////
// User

// Lib
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "imagehlp.lib")


CString  jbxwl::GetCurrentUserHomeFolder(void)
{
    HANDLE  token;
    DWORD   size = (DWORD)LMESG;
    TCHAR   dirn[LMESG];
    CString ret = _T("");

    OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token);
    BOOL rslt = GetUserProfileDirectory(token, dirn, &size);

    if (rslt && size<=LMESG) ret = dirn;
    return ret;
}


CString  jbxwl::GetDefaultUserHomeFolder(void)
{
    DWORD  size = (DWORD)LMESG;
    TCHAR  dirn[LMESG];
    CString ret = _T("");

    BOOL rslt = GetDefaultUserProfileDirectory(dirn, &size);    
    if (rslt && size<=LMESG) ret = dirn;
    return ret;
}


CString  jbxwl::MakeWorkingFolderPath(LPCTSTR fnm, BOOL local, LPCTSTR dir, BOOL make)
{
    CString path = GetCurrentUserHomeFolder();

    if (local) path += _T("\\AppData\\Local");
    else       path += _T("\\AppData\\Roaming");

    if (dir!=NULL) path += dir;
    else           path += _T("\\");

    if (make) {
        char* mbstr = ts2mbs(path);
        MakeSureDirectoryPathExists(mbstr);
        ::free(mbstr);
    }
    if (fnm!=NULL) path += fnm;

    return path;
}


CString  jbxwl::GetProgramFolder(void)
{
    char* path = GetProgramFolderA();

    CString ret = mbs2ts(path);
    ::free(path);

    return ret;
}


char*  jbxwl::GetProgramFolderA(void)
{
    TCHAR  dirn[LMESG];

    GetModuleFileName(NULL, dirn, (DWORD)LMESG);    // エラーは敢えて無視    
    //
    char* path = ts2mbs(dirn);
    int   len  = (int)strlen(path);

    for (int i=1; i<len; i++) {
        if (path[len-i-1]=='\\') {
            path[len-i] = '\0';
            break;
        }
    }

    return path;
}


CString  jbxwl::GetCurrentFolder(void)
{
#ifdef _UNICODE
    TCHAR   dirn[LMESG];
    GetCurrentDirectory(LMESG, dirn);

#else
    char    dirn[LMESG];
    GetCurrentDirectory(LMESG, dirn);

#endif

    CString folder = dirn;
    return folder;
}



////////////////////////////////////////////////////////////////////////////////////////
// メッセージ 

// 自ウィンドウにメッセージを送る
void  jbxwl::SendWinMessage(UINT mesg, WPARAM wparam, LPARAM lparam)
{
    CWinApp* app = AfxGetApp();
    if (app!=NULL) {
        CWnd* wnd = app->m_pMainWnd;
        if (wnd!=NULL) wnd->SendMessage(mesg, wparam, lparam);
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Dialog

// メッセージ Dialog
int  jbxwl::MessageDLG(LPCTSTR ttl, LPCTSTR msg, UINT type, HWND hWnd)
{
    int ret = ::MessageBox(hWnd, msg, ttl, type);
    return ret;
}



///////////////////////////////////////////////////////////////////////////////
// Clipboard

void  jbxwl::SaveToClipboard_byStr(CString data)
{
    if (data.IsEmpty()) return;

    SaveToClipboard(CStringA(data).GetBuffer(), CStringA(data).GetLength());
}


void  jbxwl::SaveToClipboard(void* ptr, size_t len)
{
    if (ptr == NULL) return;

    HGLOBAL hMem = ::GlobalAlloc(GHND, len);
    char* pszptr = (char*)::GlobalLock(hMem);

    memcpy(pszptr, ptr, len);
    //lstrcpy(pszptr, data);

    ::GlobalUnlock(hMem);

    if (!OpenClipboard(NULL)) {
        ::GlobalFree(hMem);
        return;
    }
    if (!EmptyClipboard()) {
        ::GlobalFree(hMem);
        return;
    }
    if (SetClipboardData(CF_TEXT, hMem) == NULL) {
        ::GlobalFree(hMem);
    }
    CloseClipboard();

    return;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Key Event

void  jbxwl::SendKeyAction(WORD key, SENDKEY_Action action)
{
    INPUT  input;

    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.wScan = MapVirtualKeyW(key, 0);
    input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

    if (action==SENDKEY_TapAction || action==SENDKEY_DownAction) {
        SendInput(1, &input, sizeof(INPUT));
    }
    if (action==SENDKEY_TapAction || action==SENDKEY_UpAction) {
        input.ki.dwFlags |= KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }
}


//
void  jbxwl::SendKeyActionTap(WORD key1, WORD key2)
{
    SendKeyAction(key1, SENDKEY_DownAction);
    SendKeyAction(key2, SENDKEY_TapAction);
    SendKeyAction(key1, SENDKEY_UpAction);
}


//
void  jbxwl::SendKeyActionTap(WORD key1, WORD key2, WORD key3)
{
    SendKeyAction(key1, SENDKEY_DownAction);
    SendKeyAction(key2, SENDKEY_DownAction);
    SendKeyAction(key3, SENDKEY_TapAction);
    SendKeyAction(key2, SENDKEY_UpAction);
    SendKeyAction(key1, SENDKEY_UpAction);
}


/**
return TRUE:  com の処理は完了した．
       FALSE: com の処理はまだ完了していない．
*/
BOOL  jbxwl::DoSystemKeyAction(LPCTSTR com, BOOL child)
{
    if (com==NULL) return TRUE; 

    //
    if      (!tcscmp(com, _T("UP"))) {
        SendKeyActionTap(VK_UP);
    }
    else if (!tcscmp(com, _T("DOWN"))) {
        SendKeyActionTap(VK_DOWN);
    }
    else if (!tcscmp(com, _T("RIGHT"))) {
        SendKeyActionTap(VK_RIGHT);
    }
    else if (!tcscmp(com, _T("LEFT"))) {
        SendKeyActionTap(VK_LEFT);
    }
    else if (!tcscmp(com, _T("NEXT")) || !tcscmp(com, _T("TAB"))) {
        SendKeyActionTap(VK_TAB);
    }
    else if (!tcscmp(com, _T("BACK"))) {
        SendKeyActionTap(VK_LSHIFT, VK_TAB);
    }
    else if (!tcscmp(com, _T("ENTER"))) {
        SendKeyActionTap(VK_RETURN);
    }
    else if (!tcscmp(com, _T("MENU")) || !tcscmp(com, _T("ALT"))) {
        SendKeyActionTap(VK_LMENU);
    }   
    else if (!tcscmp(com, _T("ESC"))) {
        SendKeyActionTap(VK_ESCAPE);
    }
    //
    else if (!tcscmp(com, _T("CHANGE"))) {
        SendKeyActionTapCtrl(VK_F6);
    }
    else if (!tcscmp(com, _T("CLOSE"))) {
        if (child) SendKeyActionTapAlt(VK_F4);
        else       SendKeyActionTapCtrl(VK_F4);
    }
    //
    else {
        return FALSE;
    }

    return TRUE;    
}



////////////////////////////////////////////////////////////////////////////////////////
// Mouse Cursor

static TCHAR* MouseCursolTable[] = {
    IDC_APPSTARTING,    // 標準の矢印カーソルと小さい砂時計カーソル
    IDC_ARROW,          // 標準の矢印カーソル
    IDC_CROSS,          // 十字カーソル
    IDC_HAND,           // Windows 2000：ハンドカーソル
    IDC_HELP,           // ヘルプカーソル（矢印と疑問符）
    IDC_IBEAM,          // アイビーム（縦線）カーソル
    IDC_NO,             // 禁止カーソル（円に左上から右下への斜線）
    IDC_SIZEALL,        // 4 方向の矢印カーソル
    IDC_SIZENESW,       // 右上と左下を指す両方向矢印カーソル
    IDC_SIZENS,         // 上下を指す両方向矢印カーソル
    IDC_SIZENWSE,       // 左上と右下を指す両方向矢印カーソル
    IDC_SIZEWE,         // 左右を指す両方向矢印カーソル
    IDC_UPARROW,        // 上を指す垂直の矢印カーソル
    IDC_WAIT,           // 砂時計カーソル
    IDI_APPLICATION,    // アプリケーションアイコン
    IDI_ASTERISK,       // 情報アイコン（吹き出しの中に i）
    IDI_EXCLAMATION,    // 感嘆符のアイコン
    IDI_HAND,           // ストップマークのアイコン（米国の歩行者用信号の赤と同じで、手を開いて制止しているデザインです）
    IDI_QUESTION,       // 疑問符のアイコン
    IDI_WINLOGO,        // Windows ロゴのアイコン
    NULL
};


TCHAR*  jbxwl::GetMouseCursorType()
{
    CURSORINFO ci;

    ci.cbSize = sizeof(CURSORINFO);
    GetCursorInfo(&ci);

    if (ci.flags!=0) {
        TCHAR** table = MouseCursolTable;
        while (*table!=NULL) {
            if (ci.hCursor==LoadCursor(NULL, (LPTSTR)*table)) return *table;
            table++;
        }
        return NULL;
    }

    return NULL;
}



////////////////////////////////////////////////////////////////////////////////////////
//  Timer

/**
unsigned short jbxwl::GetMsecondsTimer()

1分計．0-59999ms を返す．
*/
unsigned short jbxwl::GetMsecondsTimer()
{
    SYSTEMTIME tm;
    GetLocalTime(&tm);

    unsigned short nm = tm.wSecond*(unsigned short)1000 + tm.wMilliseconds;
    return nm;
}


/**
unsigned short jbxwl::GetMsecondsLapTimer(unsigned short pm, unsigned short* nt)

ラップタイムを返す．

@code
    unsigned short ctime;
    frame_timer += GetMsecondsLapTimer(lap_timer, &ctime);
    lap_timer = ctime;
@endcode
*/
unsigned short jbxwl::GetMsecondsLapTimer(unsigned short pm, unsigned short* nt)
{
    SYSTEMTIME tm;
    GetLocalTime(&tm);

    unsigned short nm  = tm.wSecond*(unsigned short)1000 + tm.wMilliseconds;
    unsigned short ret;
    if (pm>nm) ret = nm + ((unsigned short)60000 - pm);
    else       ret = nm - pm;

    if (nt!=NULL) *nt = nm;
    return ret;
}



////////////////////////////////////////////////////////////////////////////////////////
//  Process/Thread

void  jbxwl::WinSystem(char* command, DWORD flag, BOOL wait)
{
    STARTUPINFOA sinfo;
    PROCESS_INFORMATION pinfo;
    
    memset(&sinfo, 0, sizeof(STARTUPINFO));
    sinfo.cb = sizeof(STARTUPINFO); 
    //sinfo.wShowWindow = SW_HIDE;
                    
    CreateProcessA(NULL, (LPSTR)command, NULL, NULL, FALSE, flag, NULL, NULL, (LPSTARTUPINFOA)&sinfo, &pinfo);

    CloseHandle(pinfo.hThread);
    if (wait) WaitForSingleObject(pinfo.hProcess, INFINITE);
    CloseHandle(pinfo.hProcess);

    return;
}



////////////////////////////////////////////////////////////////////////////////////////
// EventHandler

EventHandler::EventHandler(void)
{
    m_handle   = NULL;
    m_wait_err = 0;
}


EventHandler::EventHandler(BOOL manual, BOOL initial, LPCTSTR hname, LPSECURITY_ATTRIBUTES sec)
{
    m_handle   = NULL;  
    m_wait_err = 0;
    create(manual, initial, hname, sec);
}


void  EventHandler::clear(void)
{
    if (m_handle!=NULL) {
        CloseHandle(m_handle);
        m_handle = NULL;
    }
    m_wait_err = 0;

    return;
}


HANDLE  EventHandler::create(BOOL manual, BOOL initial, LPCTSTR hname, LPSECURITY_ATTRIBUTES secu)
{
    if (m_handle!=NULL) return NULL;

    m_handle = CreateEvent(secu, manual, initial, hname);
    return m_handle;
}


/**
BOOL  EventHandler::wait(DWORD msec=INFINITE)

    m_wait_err：
        WAIT_OBJECT_0
        WAIT_TIMEOUT
        WAIT_FAILED
        WAIT_ABANDONED
*/
BOOL  EventHandler::wait(DWORD msec)
{
    m_wait_err = WAIT_FAILED;
    if (m_handle==NULL) return FALSE;

    m_wait_err = WaitForSingleObject(m_handle, msec);
    
    if (m_wait_err!=WAIT_OBJECT_0) return FALSE;
    return TRUE;
}


BOOL  EventHandler::wait(HANDLE handle, DWORD msec)
{
    m_wait_err = WAIT_FAILED;
    if (handle==NULL) return FALSE;

    m_wait_err = WaitForSingleObject(handle, msec);
    
    if (m_wait_err!=WAIT_OBJECT_0) return FALSE;
    return TRUE;
}


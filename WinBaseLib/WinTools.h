#pragma once

/**
Tools for Windows ライブラリ

*/

//#include "tools.h"

#ifdef WIN32
#pragma warning(disable:4005)
#pragma warning(disable:4996)
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_MAXVER
#endif

//
#include  <afxmt.h>

#include  "WinBaseResource.h"
#include  "BasicLib.h"

#include  <userenv.h>
#include  <imagehlp.h>
#include  <locale.h>

// Locale
#define  JBXWL_LOCALE_US  "English"
#define  JBXWL_LOCALE_JP  "Japanese"


// Windows メッセージ
#define  JBXWL_WM_USER            WM_USER + 9000

#define  JBXWL_WM_NETWORK_EVENT   JBXWL_WM_USER + 100
#define  JBXWL_WM_NETWORK_OPEN    JBXWL_WM_USER + 101
#define  JBXWL_WM_NETWORK_CLOSE   JBXWL_WM_USER + 102
#define  JBXWL_WM_SPEECH_EVENT    JBXWL_WM_USER + 110



//
namespace jbxwl {


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Unicode

// 変換   
///     locale が正しくないと，日本語の変換は失敗する． ex.) setlocale(LC_CTYPE,"jpn") or setSystemLocale()
Buffer  tc2Buffer(TCHAR*  tchar, int size=-1);
Buffer  ts2Buffer(LPCTSTR str,   int size=-1);

CString      mbs2ts(char*   str);
inline char* ts2mbs(LPCTSTR str)  // 要 free. NULL は絶対に返ってこない
{
    Buffer buf = ts2Buffer(str);
    return (char*)buf.buf;
}

//
int     copy_ts2Buffer(LPCTSTR str, Buffer* buf);
int     cat_ts2Buffer (LPCTSTR str, Buffer* buf);

// File
unsigned long int file_size_t(LPCTSTR fn); 
BOOL    file_exist_t    (LPCTSTR fn);
CString get_file_name_t (LPCTSTR str);
CString make_file_path_t(LPCTSTR str);
CString cut_file_extension_t(LPCTSTR str);

#define get_file_path_t(p)  make_file_path_t((p))
#define del_file_extension_t(p)  cut_file_extension_t((p))

tList*  read_index_tList_file_t(LPCTSTR fn, char cc);
tList*  get_dir_files_t(LPCTSTR dirn);
tList*  get_dir_files_rcsv_t(LPCTSTR dirn);

// Tools
CString numbering_name_t(LPCTSTR fmt, int n);
CString get_tstr_param_tList(tList* lt, char* key, LPCTSTR dflt);

// locale
void    setResourceLocale(void);    // リソース IDS_STR_LOCALE が示すロケールに設定する．
CString getResourceLocale(void);    // リソース IDS_STR_LOCALE が示すロケールを返す．
void    setSystemLocale(LPCTSTR locale=_T(""));     // システムのロケールを設定する．


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  User

CString GetCurrentUserHomeFolder(void);
CString GetDefaultUserHomeFolder(void);
CString MakeWorkingFolderPath(LPCTSTR fnm, BOOL local, LPCTSTR dir=NULL, BOOL make=FALSE);

CString GetProgramFolder(void);
char*   GetProgramFolderA(void);

CString GetCurrentFolder(void);


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Message

// 自ウィンドウにメッセージを送る．
void  SendWinMessage(UINT mesg, WPARAM wparam=NULL, LPARAM lparam=NULL);


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Key Event

enum  SENDKEY_Action
{
    SENDKEY_TapAction,
    SENDKEY_DownAction,
    SENDKEY_UpAction
};

//
void  SendKeyAction(WORD key, SENDKEY_Action action);

inline void  SendKeyActionTap(WORD key) { SendKeyAction(key, SENDKEY_TapAction);}
void  SendKeyActionTap(WORD key1, WORD key2);
void  SendKeyActionTap(WORD key1, WORD key2, WORD key3);

//
inline void  SendKeyActionTapAlt(WORD key)  { SendKeyActionTap(VK_LMENU,    key);}
inline void  SendKeyActionTapShift(WORD key){ SendKeyActionTap(VK_LSHIFT,   key);}
inline void  SendKeyActionTapCtrl(WORD key) { SendKeyActionTap(VK_LCONTROL, key);}

//
BOOL  DoSystemKeyAction(LPCTSTR com, BOOL child=FALSE);


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Mouse Cursor

TCHAR*  GetMouseCursorType(void);


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Time

// 1分計 0-59999ms を返す．
unsigned short  GetMsecondsTimer(void);
unsigned short  GetMsecondsLapTimer(unsigned short pm, unsigned short* nt=NULL);


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Resource

inline  CString  LoadString_byID(UINT id)
{
    CString mesg;
    mesg.LoadString(id);
    return mesg;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event Handler クラス

class  EventHandler
{
public:
    EventHandler(void);
    EventHandler(BOOL manual, BOOL initial=FALSE, LPCTSTR hname=NULL, LPSECURITY_ATTRIBUTES sec=NULL);

    virtual ~EventHandler(void) { clear();}

    HANDLE  m_handle;
    DWORD   m_wait_err;

public:
    HANDLE  create(BOOL manual=TRUE, BOOL initial=FALSE, LPCTSTR hname=NULL, LPSECURITY_ATTRIBUTES sec=NULL);
    void    clear(void);

    BOOL    set(void) { return SetEvent(m_handle);}

    BOOL    wait(DWORD msec=INFINITE);
    BOOL    wait(HANDLE handle, DWORD msec=INFINITE);
};



///////////////////////////////////////////////////////////////////////////////
// メッセージ Dialog
  
int  MessageDLG(LPCTSTR ttl, LPCTSTR msg, UINT type,  HWND hWnd);



////////////////////////////////////////////////////////////////////////////////////////
//  Process/Thread

void  WinSystem(char* command, DWORD flag, BOOL wait);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

//
// SafeRelease(), DXRELEASE() [Dx9.h] と同じ
//
template <class T> inline void releaseNull(T*& ptr)
{
    if (!jbxl::isNull(ptr)) {
        ptr->Release();
        ptr = NULL;
    }
}



template <class T> inline void deleteNull(T*& ptr)
{
    if (!jbxl::isNull(ptr)) {
        delete ptr;
        ptr = NULL;
    }
}


}       // namespace




/*
HRESULT Code: http://support.microsoft.com/kb/186063/ja

0x8000FFFF    Catastrophic failure.
0x8007000D    The Data is invalid.
0x80004001    Not implemented.
0x8007000E    Ran out of memory.
0x80070057    One or more arguments are invalid.
0x80004002    No such interface supported.
0x80004003    Invalid pointer.
0x80070006    Invalid handle.
0x80004004    Operation aborted.
0x80004005    Unspecified error.
0x80070005    General access denied error.
0x80000001    Not implemented.
0x80000002    Ran out of memory.
0x80000003    One or more arguments are invalid.
0x80000004    No such interface supported.
0x80000005    Invalid pointer.
0x80000006    Invalid handle.
0x80000007    Operation aborted.
0x80000008    Unspecified error.
0x80000009    General access denied error.
0x8000000A    The data necessary to complete this operation not yet available.
0x80004006    Thread local storage failure.
0x80004007    Get shared memory allocator failure.
0x80004008    Get memory allocator failure.
0x84009       Unable to initialize class cache.
0x8400A       Unable to initialize RPC services.
0x8000400B    Cannot set thread local storage channel control.
0x8000400C    Could not allocate thread local storage channel control.
0x8000400D    The user supplied memory allocator is unacceptable.
0x8000400E    The OLE service mutex already exists.
0x8000400F    The OLE service file mapping already exists.
0x80004010    Unable to map view of file for OLE service.
0x80004011    Failure attempting to launch OLE service.
0x80004012    There was an attempt to call CoInitialize a second time while single threaded.
0x80004013    A Remote activation was necessary but was not allowed.
0x80004014    A Remote activation was necessary but the server name provided was invalid.
0x80004015    The class is configured to run as a security id different from the caller.
0x80004016    Use of Ole1 services requiring DDE windows is disabled.
0x80004017    A RunAs specification must be A RunAs specification must be <domain name>\<user name> or simply <user name>.             
0x80004018    The server process could not be started. The pathname may be incorrect.  
0x80004019    The server process could not be started as the configured identity. The pathname may be incorrect or unavailable.  
0x8000401A    The server process could not be started because the configured identity is incorrect. Check the username and password. 
0x8000401B    The client is not allowed to launch this server. 
0x8000401C    The service providing this server could not be started. 
0x8000401D    This computer was unable to communicate with the computer providing the server. 
0x8000401E    The server did not respond after being launched. 
0x8000401F    The registration information for this server is inconsistent or incomplete. 
0x80004020    The registration information for this interface is inconsistent or incomplete. 
0x80004021    The operation attempted is not supported. 
0x80040000    Invalid OLEVERB structure.
0x80040001    Invalid advise flags.
0x80040002    Can't enumerate any more, because the associated data is missing.
0x80040003    This implementation doesn't take advises.
0x80040004    There is no connection for this connection ID.
0x80040005    Need to run the object to perform this operation.
0x80040006    There is no cache to operate on.
0x80040007    Uninitialized object.
0x80040008    Linked object's source class has changed.
0x80040009    Not able to get the moniker of the object.
0x8004000A    Not able to bind to the source.
0x8004000B    Object is static; operation not allowed.
0x8004000C    User cancelled out of save dialog.
0x8004000D    Invalid rectangle.
0x8004000E    compobj.dll is too old for the ole2.dll initialized.
0x8004000F    Invalid window handle.
0x80040010    Object is not in any of the inplace active states.
0x80040011    Not able to convert object.
0x80040012    Not able to perform the operation because object is not given storage yet.
0x80040064    Invalid FORMATETC structure.
0x80040065    Invalid DVTARGETDEVICE structure.
0x80040066    Invalid STDGMEDIUM structure.
0x80040067    Invalid STATDATA structure.
0x80040068    Invalid lindex.
0x80040069    Invalid tymed.
0x8004006A    Invalid clipboard format.
0x8004006B    Invalid aspect(s).
0x8004006C    tdSize parameter of the DVTARGETDEVICE structure is invalid.
0x8004006D    Object doesn't support IViewObject interface.
0x80040100    Trying to revoke a drop target that has not been registered.
0x80040101    This window has already been registered as a drop target.
0x80040102    Invalid window handle.
0x80040110    Class does not support aggregation (or class object is remote).
0x80040111    ClassFactory cannot supply requested class.
0x80040140    Error drawing view.
0x80040150    Could not read key from registry.
0x80040151    Could not write key to registry.
0x80040152    Could not find the key in the registry.
0x80040153    Invalid value for registry.
0x80040154    Class not registered.
0x80040155    Interface not registered.
0x80040170    Cache not updated.
0x80040180    No verbs for OLE object.
0x80040181    Invalid verb for OLE object.
0x800401A0    Undo is not available.
0x800401A1    Space for tools is not available.
0x800401C0    OLESTREAM Get method failed.
0x800401C1    OLESTREAM Put method failed.
0x800401C2    Contents of the OLESTREAM not in correct format.
0x800401C3    There was an error in a Windows GDI call while converting the bitmap to a DIB.
0x800401C4    Contents of the IStorage not in correct format.
0x800401C5    Contents of IStorage is missing one of the standard streams.
0x800401C6    There was an error in a Windows GDI call while converting the DIB to a bitmap. 
0x800401D0    OpenClipboard Failed.
0x800401D1    EmptyClipboard Failed.
0x800401D2    SetClipboard Failed.
0x800401D3    Data on clipboard is invalid.
0x800401D4    CloseClipboard Failed.
0x800401E0    Moniker needs to be connected manually.
0x800401E1    Operation exceeded deadline.
0x800401E2    Moniker needs to be generic.
0x800401E3    Operation unavailable.
0x800401E4    Invalid syntax.
0x800401E5    No object for moniker.
0x800401E6    Bad extension for file.
0x800401E7    Intermediate operation failed.
0x800401E8    Moniker is not bindable.
0x800401E9    Moniker is not bound.
0x800401EA    Moniker cannot open file.
0x800401EB    User input required for operation to succeed.
0x800401EC    Moniker class has no inverse.
0x800401ED    Moniker does not refer to storage.
0x800401EE    No common prefix.
0x800401EF    Moniker could not be enumerated.
0x800401F0    CoInitialize has not been called. 
0x800401F1    CoInitialize has already been called. 
0x800401F2    Class of object cannot be determined.
0x800401F3    Invalid class string.
0x800401F4    Invalid interface string.
0x800401F5    Application not found.
0x800401F6    Application cannot be run more than once.
0x800401F7    Some error in application program.
0x800401F8    DLL for class not found.
0x800401F9    Error in the DLL.
0x800401FA    Wrong OS or OS version for application.
0x800401FB    Object is not registered.
0x800401FC    Object is already registered.
0x800401FD    Object is not connected to server.
0x800401FE    Application was launched but it didn't register a class factory.
0x800401FF    Object has been released.
0x80080001    Attempt to create a class object failed.
0x80080002    OLE service could not bind object.
0x80080003    RPC communication failed with OLE service.
0x80080004    Bad path to object.
0x80080005    Server execution failed.
0x80080006    OLE service could not communicate with the object server.
0x80080007    Moniker path could not be normalized.
0x80080008    Object server is stopping when OLE service contacts it.
0x80080009    An invalid root block pointer was specified.
0x80080010    An allocation chain contained an invalid link pointer.
0x80080011    The requested allocation size was too large.
0x80020001    Unknown interface. 
0x80020003    Member not found. 
0x80020004    Parameter not found. 
0x80020005    Type mismatch. 
0x80020006    Unknown name.  
0x80020007    No named arguments. 
0x80020008    Bad variable type. 
0x80020009    Exception occurred. 
0x8002000A    Out of present range. 
0x8002000B    Invalid index. 
0x8002000C    Unknown language. 
0x8002000D    Memory is locked. 
0x8002000E    Invalid number of parameters. 
0x8002000F    Parameter not optional. 
0x80020010    Invalid callee. 
0x80020011    Does not support a collection. 
0x80028016    Buffer too small. 
0x80028018    Old format or invalid type library. 
0x80028019    Old format or invalid type library. 
0x8002801C    Error accessing the OLE registry. 
0x8002801D    Library not registered. 
0x80028027    Bound to unknown type. 
0x80028028    Qualified name disallowed. 
0x80028029    Invalid forward reference, or reference to uncompiled type. 
0x8002802A    Type mismatch. 
0x8002802B    Element not found. 
0x8002802C    Ambiguous name. 
0x8002802D    Name already exists in the library. 
0x8002802E    Unknown LCID. 
0x8002802F    Function not defined in specified DLL. 
0x800288BD    Wrong module kind for the operation. 
0x800288C5    Size may not exceed 64K. 
0x800288C6    Duplicate ID in inheritance hierarchy. 
0x800288CF    Incorrect inheritance depth in standard OLE hmember. 
0x80028CA0    Type mismatch. 
0x80028CA1    Invalid number of arguments. 
0x80028CA2    I/O Error. 
0x80028CA3    Error creating unique tmp file. 
0x80029C4A    Error loading type library/DLL. 
0x80029C83    Inconsistent property functions. 
0x80029C84    Circular dependency between types/modules. 
0x80030001    Unable to perform requested operation. 
0x80030002    %1 could not be found. 
0x80030003    The path %1 could not be found. 
0x80030004    There are insufficient resources to open another file.
0x80030005    Access Denied. 
0x80030006    Attempted an operation on an invalid object. 
0x80030008    There is insufficient memory available to complete operation. 
0x80030009    Invalid pointer error. 
0x80030012    There are no more entries to return. 
0x80030013    Disk is write-protected. 
0x80030019    An error occurred during a seek operation. 
0x8003001D    A disk error occurred during a write operation. 
0x8003001E    A disk error occurred during a read operation. 
0x80030020    A share violation has occurred. 
0x80030021    A lock violation has occurred. 
0x80030050    %1 already exists. 
0x80030057    Invalid parameter error. 
0x80030070    There is insufficient disk space to complete operation. 
0x800300F0    Illegal write of non-simple property to simple property set. 
0x800300FA    An API call exited abnormally. 
0x800300FB    The file %1 is not a valid compound file. 
0x800300FC    The name %1 is not valid. 
0x800300FD    An unexpected error occurred. 
0x800300FE    That function is not implemented. 
0x800300FF    Invalid flag error. 
0x80030100    Attempted to use an object that is busy. 
0x80030101    The storage has been changed since the last commit. 
0x80030102    Attempted to use an object that has ceased to exist. 
0x80030103    Can't save. 
0x80030104    The compound file %1 was produced with an incompatible version of storage. 
0x80030105    The compound file %1 was produced with a newer version of storage. 
0x80030106    Share.exe or equivalent is required for operation. 
0x80030107    Illegal operation called on non-file based storage. 
0x80030108    Illegal operation called on object with extant marshallings. 
0x80030109    The docfile has been corrupted. 
0x80030110    OLE32.DLL has been loaded at the wrong address. 
0x80030201    The file download was aborted abnormally. The file is incomplete. 
0x80030202    The file download has been terminated. 
0x80010001    Call was rejected by callee. 
0x80010002    Call was canceled by the message filter. 
0x80010003    The caller is dispatching an intertask SendMessage call and cannot call out via PostMessage. 
0x80010004    The caller is dispatching an asynchronous call and cannot make an outgoing call on behalf of this call. 
0x80010005    It is illegal to call out while inside message filter. 
0x80010006    The connection terminated or is in a bogus state and cannot be used any more. Other connections are still valid. 
0x80010007    The callee (server [not server application]) is not available and disappeared; all connections are invalid. The call may have executed. 
0x80010008    The caller (client) disappeared while the callee (server) was processing a call. 
0x80010009    The data packet with the marshalled parameter data is incorrect. 
0x8001000A    The call was not transmitted properly; the message queue was full and was not emptied after yielding. 
0x8001000B    The client (caller) cannot marshal the parameter data - low memory, etc. 
0x8001000C    The client (caller) cannot unmarshal the return data - low memory, etc. 
0x8001000D    The server (callee) cannot marshal the return data - low memory, etc. 
0x8001000E    The server (callee) cannot unmarshal the parameter data - low memory, etc. 
0x8001000F    Received data is invalid; could be server or client data. 
0x80010010    A particular parameter is invalid and cannot be (un)marshalled. 
0x80010011    There is no second outgoing call on same channel in DDE conversation. 
0x80010012    The callee (server [not server application]) is not available and disappeared; all connections are invalid. The call did not execute. 
0x80010100    System call failed. 
0x80010101    Could not allocate some required resource (memory, events, ...) 
0x80010102    Attempted to make calls on more than one thread in single threaded mode. 
0x80010103    The requested interface is not registered on the server object. 
0x80010104    RPC could not call the server or could not return the results of calling the server. 
0x80010105    The server threw an exception. 
0x80010106    Cannot change thread mode after it is set. 
0x80010107    The method called does not exist on the server. 
0x80010108    The object invoked has disconnected from its clients. 
0x80010109    The object invoked chose not to process the call now. Try again later. 
0x8001010A    The message filter indicated that the application is busy. 
0x8001010B    The message filter rejected the call. 
0x8001010C    A call control interfaces was called with invalid data. 
0x8001010D    An outgoing call cannot be made since the application is dispatching an input-synchronous call. 
0x8001010E    The application called an interface that was marshalled for a different thread. 
0x8001010F    CoInitialize has not been called on the current thread. 
0x80010110    The version of OLE on the client and server machines does not match. 
0x80010111    OLE received a packet with an invalid header. 
0x80010112    OLE received a packet with an invalid extension. 
0x80010113    The requested object or interface does not exist. 
0x80010114    The requested object does not exist. 
0x80010115    OLE has sent a request and is waiting for a reply. 
0x80010116    OLE is waiting before retrying a request. 
0x80010117    Call context cannot be accessed after call completed. 
0x80010118    Impersonate on unsecured calls is not supported. 
0x80010119    Security must be initialized before any interfaces are marshalled or unmarshalled. It cannot be changed once initialized. 
0x8001011A    No security packages are installed on this machine or the user is not logged on or there are no compatible security packages between the client and server. 
0x8001011B    Access is denied. 
0x8001011C    Remote calls are not allowed for this process. 
0x8001011D    The marshalled interface data packet (OBJREF) has an invalid or unknown format. 
0x8001FFFF    An internal error occurred. 
0x80090001    Bad UID. 
0x80090002    Bad Hash. 
0x80090003    Bad Key. 
0x80090004    Bad Length. 
0x80090005    Bad Data. 
0x80090006    Invalid Signature. 
0x80090007    Bad Version of provider. 
0x80090008    Invalid algorithm specified. 
0x80090009    Invalid flags specified. 
0x8009000A    Invalid type specified. 
0x8009000B    Key not valid for use in specified state. 
0x8009000C    Hash not valid for use in specified state. 
0x8009000D    Key does not exist.  
0x8009000E    Insufficient memory available for the operation. 
0x8009000F    Object already exists. 
0x80090010    Access denied. 
0x80090011    Object was not found. 
0x80090012    Data already encrypted. 
0x80090013    Invalid provider specified. 
0x80090014    Invalid provider type specified. 
0x80090015    Provider's public key is invalid. 
0x80090016    Keyset does not exist. 
0x80090017    Provider type not defined. 
0x80090018    Provider type as registered is invalid. 
0x80090019    The keyset is not defined. 
0x8009001A    Keyset as registered is invalid. 
0x8009001B    Provider type does not match registered value. 
0x8009001C    The digital signature file is corrupt. 
0x8009001D    Provider DLL failed to initialize correctly. 
0x8009001E    Provider DLL could not be found. 
0x8009001F    The Keyset parameter is invalid. 
0x80090020    An internal error occurred. 
0x80090021    A base error occurred. 
0x800B0001    The specified trust provider is not known on this system. 
0x800B0002    The trust verification action specified is not supported by the specified trust provider. 
0x800B0003    The form specified for the subject is not one supported or known by the specified trust provider. 
0x800B0004    The subject is not trusted for the specified action. 
0x800B0005    Error due to problem in ASN.1 encoding process. 
0x800B0006    Error due to problem in ASN.1 decoding process. 
0x800B0007    Reading / writing Extensions where Attributes are appropriate, and visa versa. 
0x800B0008    Unspecified cryptographic failure. 
0x800B0009    The size of the data could not be determined. 
0x800B000A    The size of the indefinite-sized data could not be determined. 
0x800B000B    This object does not read and write self-sizing data.  
0x800B0100    No signature was present in the subject. 
0x800B0101    A required certificate is not within its validity period. 
0x800B0102    The validity periods of the certification chain do not nest correctly.
0x800B0103    A certificate that can only be used as an end-entity is being used as a CA or visa versa.
0x800B0104    A path length constraint in the certification chain has been violated.
0x800B0105    An extension of unknown type that is labeled 'critical' is present in a certificate.
0x800B0106    A certificate is being used for a purpose other than that for which it is permitted.
0x800B0107    A parent of a given certificate in fact did not issue that child certificate.
0x800B0108    A certificate is missing or has an empty value for an important field, such as a subject or issuer name.
0x800B0109    A certification chain processed correctly, but terminated in a root certificate which isn't trusted by the trust provider.
0x800B010A    A chain of certs didn't chain as they should in a certain application of chaining.
*/

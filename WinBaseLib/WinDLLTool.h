#pragma once

/**
Tools for ExWindows ライブラリ

*/

#include  "WinTools.h"


//
namespace jbxwl {


/**
@file    WinDLLTool.h

DLLModule
DLLModuleTBL

    通常，DLL中の関数の名前，型，引数はアプリケーションによって変化する．
    このライブラリは汎用化を目指しているので，これらの情報は直接には扱わない．
    そのため，このライブラリを使用してDLLを管理する場合は以下のように行う．
    
    アプリケーション作成者
        関数を呼び出し易くするために
            DLLModule を継承し，関数を呼び出すためのラッパー関数をメンバとしたクラスを作る．
            DLLModuleTBL を継承し，DLLModuleを継承したクラスのラッパー関数に実際の関数ポインタを割り当てる．
            具体的には，
                new_module() をオーバーロードして DLLModule を継承したクラスを作成できるようにする．
                set_module_function() をオーバーロードし，DLLModuleを継承したクラスのラッパー関数に実際の関数ポインタを割り当てる．
        DLL作成者に情報取得関数の名前，使用できる関数の数・型を伝える．

    DLL作成者．
        DLL中に情報所得関数  char* (*FUNC_STRING)(int) を用意する．
        char* (*FUNC_STRING)(0) は必ずタイトルを返す．
        char* (*FUNC_STRING)(1) 〜 char* (*FUNC_STRING)(fnum-1) は使用できる関数名を返す．
        情報所得関数の名前，fnum（関数の数）, 関数の型はアプリケーション作成者と予め合意しておく．
*/

/**
サンプル：
        
*/


typedef  char* (*FUNC_STRING)(int);


//
// DLLデータを格納するためのクラス
//
class  DLLModule
{
public:
    int     status;         // モジュールの状態．<=0 では不正な状態．
    int     fnum;           // 使用できる関数の数

    char*   title;
    CString dllname;        // DLL名
    CString filename;       // DLLファイル名
    CString funcname;       // 情報取得関数の名前．予め決めておく．

    HMODULE hmod;           // モジュールハンドラ

protected:
    FUNC_STRING pgetinfo;   // 情報取得関数へのポインタ
    void**  pfunc;          // 使用できる関数へのポインタ配列

public:
    DLLModule(LPCTSTR fname, int n) { init(fname, n);}
    DLLModule()  { init(NULL, 0);}
    virtual ~DLLModule() { free();}

    void   init(LPCTSTR fname, int n);
    void   free();
    char*  get_info(int n=0) { if (pgetinfo==NULL) return NULL; return (*pgetinfo)(n);}

    BOOL   set_funcname(LPCTSTR fname, int n);
    BOOL   get_module_info(tList* lp);
    void*  get_funcptr(int n);

    friend class DLLModuleTBL;
};



//
// DLLModule の配列のクラス
//
class  DLLModuleTBL 
{
public:
    int     vnum;           // 有効なDLLの数
    DLLModule** dllmod;     // DLLModuleポインタ配列へのポインタ

    bool    loaded;

protected:
    int     tnum;           // 読み込んだDLLの数（不正DLLを含む）
    tList*  lp;             // ディレクトリから読み取った DLL情報
    CString dirname;        // DLLを探すディレクトリ名

public:
    DLLModuleTBL(LPCTSTR dirn) { init(dirn);}
    DLLModuleTBL()  { init(NULL);}
    virtual ~DLLModuleTBL() { free();}

    void  init(LPCTSTR dirn=NULL);
    void  free();

    BOOL  load_module(LPCTSTR dirn=NULL);
    void  make_module_tbl(LPCTSTR fname, int n);

protected:
    tList*   get_dir_module(LPCTSTR dirn);

    virtual DLLModule* new_module(LPCTSTR fname, int n) { return new DLLModule(fname, n);} 
    virtual void set_module_function() {}
};


}       // namespace

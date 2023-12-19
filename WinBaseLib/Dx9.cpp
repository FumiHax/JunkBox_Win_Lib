/**
    DirectX9 ライブラリ

*/

#include "Dx9.h"


using namespace jbxl;
using namespace jbxwl;


LPDIRECT3D9     jbxwl::GpD3D    = NULL;
LPDIRECTINPUT8  jbxwl::GpDInput = NULL;

LPDIRECTINPUTDEVICE8  jbxwl::GpDMouse    = NULL;        // マウスのインターフェイス
LPDIRECTINPUTDEVICE8  jbxwl::GpDKeyBoard = NULL;        // キーボードのインターフェイス


BOOL  jbxwl::Dx9DividePrimitiveMode = TRUE;             // 分割描画モード
BOOL  jbxwl::Dx9ReverseZMode        = FALSE;            // Z軸反転モード（for 3D CT）



/**
    機能：  DirectX9のデバイスへのインターフェイスを得る．
    引数： hinst  アプリケーションのインスタンスハンドラ（通常は this->m_hInstanceを指定） 
    説明：  
*/
BOOL  jbxwl::Dx9CreateInterface(CWinApp*  wapp) 
{
    GpD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (GpD3D==NULL) {
        DEBUG_INFO("jbxwl::Dx9CreateInterface: ERROR: グラフィックデバイスの取得に失敗しました\n");
        return FALSE;
    }

    HRESULT hr = DirectInput8Create(wapp->m_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&GpDInput, NULL);
    if (GpDInput==NULL) {
        DEBUG_INFO("jbxwl::Dx9CreateInterface: ERROR: インプットデバイスの取得に失敗しました\n");
        DXRELEASE(GpD3D);
        return FALSE;
    }
    else {
        GpDMouse = Dx9CreateMouse(GpDInput, wapp->m_pMainWnd->m_hWnd);
        if (GpDMouse==NULL) {
            Sleep(1000);
            GpDMouse = Dx9CreateMouse(GpDInput, wapp->m_pMainWnd->m_hWnd);
            if (GpDMouse==NULL) {
                DEBUG_INFO("jbxwl::Dx9CreateInterface(): ERROR: マウスデバイスの取得に失敗しました\n");
                DXRELEASE(GpD3D);
                DXRELEASE(GpDInput);
                return FALSE;
            }
        }
        GpDKeyBoard = Dx9CreateKeyBoard(GpDInput, wapp->m_pMainWnd->m_hWnd);
        if (GpDKeyBoard==NULL) {
            Sleep(1000);
            GpDKeyBoard = Dx9CreateKeyBoard(GpDInput, wapp->m_pMainWnd->m_hWnd);
            if (GpDKeyBoard==NULL) {
                DEBUG_INFO("jbxwl::Dx9CreateInterface(): ERROR: キーボードデバイスの取得に失敗しました\n");
                DXRELEASE(GpDMouse);
                DXRELEASE(GpD3D);
                DXRELEASE(GpDInput);
                return FALSE;
            }
        }
    }

    return TRUE;
}



void  jbxwl::Dx9ReleaseInterface()
{
    DXRELEASE(GpDMouse);
    DXRELEASE(GpDKeyBoard);
    DXRELEASE(GpDInput);
    DXRELEASE(GpD3D);
}



/**
    機能：  DirectX9のグラフィックデバイスを得る．
    引数： gpD3D       DirectX9のインターフェイス 
            *d3dParam   デバイス作成のためのパラメータ
            hWnd        デバイスを作成するウィンドウのウィンドウハンドル  
    戻値：  DirectX9デバイス
    説明：  
*/
LPDIRECT3DDEVICE9  jbxwl::Dx9CreateGraphic(LPDIRECT3D9 gpD3D, D3DPRESENT_PARAMETERS* d3dParam, HWND hWnd)
{
    LPDIRECT3DDEVICE9  d3dDevice=NULL;
    HRESULT hr;

    hr = gpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, d3dParam, &d3dDevice);
    if (FAILED(hr)) {
        hr = gpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, d3dParam, &d3dDevice);
        if (FAILED(hr)) {
            hr = gpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, d3dParam, &d3dDevice);
            if (FAILED(hr)) return (LPDIRECT3DDEVICE9)NULL;
        }
    }
    return d3dDevice;
}



/**

*/
LPDIRECTINPUTDEVICE8  jbxwl::Dx9CreateMouse(LPDIRECTINPUT8 gpDInput, HWND hWnd)
{
    LPDIRECTINPUTDEVICE8  gpdmouse=NULL;

    HRESULT hr = gpDInput->CreateDevice(GUID_SysMouse, &gpdmouse, NULL);
    if (FAILED(hr) || gpdmouse==NULL) return NULL;

    hr = gpdmouse->SetDataFormat(&c_dfDIMouse);
    if (FAILED(hr)) return NULL;

    hr = gpdmouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    if (FAILED(hr)) return NULL;

//  軸モード 
/*  DIPROPDWORD  diprop;
    diprop.diph.dwSize = sizeof(diprop);
    diprop.diph.dwHeaderSize = sizeof(diprop.diph);
    diprop.diph.dwObj = 0;
    diprop.diph.dwHow = DIPH_DEVICE;
    diprop.dwData = DIPROPAXISMODE_ABS;
    hr = gpdmouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);
    if (FAILED(hr)) {
        DEBUG_INFO("Dx9CreateMouse: SetProprty\n");
        return NULL;
    }
/**/
    hr = gpdmouse->Acquire();
    if (FAILED(hr)) return NULL;

    return gpdmouse;  
}



//
//  不完全  注意!!!
//
LPDIRECTINPUTDEVICE8  jbxwl::Dx9CreateKeyBoard(LPDIRECTINPUT8 gpDInput, HWND hWnd)
{
    LPDIRECTINPUTDEVICE8  gpdkeyboard=NULL;

    HRESULT hr = gpDInput->CreateDevice(GUID_SysKeyboard, &gpdkeyboard, NULL);
    if (FAILED(hr)) return NULL;

    hr = gpdkeyboard->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(hr)) return NULL;

    hr = gpdkeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    if (FAILED(hr)) return NULL;
    else            return gpdkeyboard;  
}



/*
    機能：  バックバッファを得る．
    引数： gpD3DDevice     DirectX9デバイス 
            *pgpBackBuffer  獲得したバックバッファ．失敗した場合は NULL
    戻値：  バックバッファのサイズ．エラーの場合は  <0
    説明：  
*/
POINT  jbxwl::Dx9GetBackBuffer(LPDIRECT3DDEVICE9 gpD3DDevice, LPDIRECT3DSURFACE9* pgpBackBuffer)
{
    POINT  pt;
    pt.x = pt.y = -1;

    HRESULT hr = gpD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, pgpBackBuffer);
    if (FAILED(hr)) return pt;

    pt = Dx9GetSurfaceSize(pgpBackBuffer);
    return pt;
}



/*
    機能：  メモリ上の cmnHeadからサーフェイスを作成する．（ラッパー関数）
    引数： gpD3DDevice     DirectX9デバイス 
            *pgpSurface     作成したサーフェイス
            cmnhead         メモリ上のデータを記述する CmnHead 型データ．
    戻値：  実行結果の BOOL
    説明：  サポートするデータ形式は  DirectX9 (jpg, png, bmp, ......), SUN Raster, CT_DATA
            作成したサーフェイスのメモリ管理は D3DPOOL_DEFAULTになる．
*/
BOOL  jbxwl::Dx9SurfaceFromMemory(LPDIRECT3DDEVICE9 gpD3DDevice, LPDIRECT3DSURFACE9* pgpSurface, CmnHead cmnHead)
{
    BOOL ret;

    if (cmnHead.kind==HEADER_NONE) return FALSE;
    int chk = cmnHead.kind & 0x00ff;

    if (chk==UN_KNOWN_DATA) ret = Dx9SurfaceFromMemoryFile(gpD3DDevice, pgpSurface, cmnHead);
    else                    ret = Dx9SurfaceFromByteMemory(gpD3DDevice, pgpSurface, cmnHead);
    return  ret;
}



//
//  メモリ上の cmnHeadからサーフェイスを作成する．
//  DirectX9 がサポートするデータ(UN_KNOWN)用．cmnHead.grptrがメモリ上のファイル形式データを指す．
//
BOOL  jbxwl::Dx9SurfaceFromMemoryFile(LPDIRECT3DDEVICE9 gpD3DDevice, LPDIRECT3DSURFACE9* pgpSurface, CmnHead cmnHead)
{
    //HRESULT hr = gpD3DDevice->CreateOffscreenPlainSurface(
    //      cmnHead.xsize, cmnHead.ysize, (D3DFORMAT)cmnHead.depth, D3DPOOL_DEFAULT, pgpSurface, NULL);

    HRESULT hr = gpD3DDevice->CreateOffscreenPlainSurface(cmnHead.xsize, cmnHead.ysize,  D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, pgpSurface, NULL);
    if (FAILED(hr)) return FALSE;

    hr = D3DXLoadSurfaceFromFileInMemory(*pgpSurface, NULL, NULL, cmnHead.grptr, cmnHead.lsize, NULL, D3DX_FILTER_NONE, 0, (D3DXIMAGE_INFO*)cmnHead.buf);
    if (FAILED(hr)) return FALSE;

    return TRUE;
}



//
//  メモリ上の cmnHeadからサーフェイスを作成する．
//  RAS_DATA(SUN Raster), CT_DATAなどの独自ファイル用．表示データは 8bit
//
BOOL  jbxwl::Dx9SurfaceFromByteMemory(LPDIRECT3DDEVICE9 gpD3DDevice, LPDIRECT3DSURFACE9* pgpSurface, CmnHead cmnHead)
{
    HRESULT hr = gpD3DDevice->CreateOffscreenPlainSurface(cmnHead.xsize, cmnHead.ysize, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, pgpSurface, NULL);
    if (FAILED(hr)) return FALSE;

    RECT    rect;
    rect.left   = 0;
    rect.top    = 0;
    rect.right  = cmnHead.xsize - 1;
    rect.bottom = cmnHead.ysize - 1;
    hr = D3DXLoadSurfaceFromMemory(*pgpSurface, NULL, NULL, cmnHead.grptr, D3DFMT_L8,  cmnHead.xsize, NULL, &rect, D3DX_FILTER_NONE, 0);
//  hr = D3DXLoadSurfaceFromMemory(*pgpSurface, NULL, NULL, cmnHead.grptr, D3DFMT_L16, cmnHead.xsize*2, NULL, &rect, D3DX_FILTER_NONE, 0);
    if (FAILED(hr)) return FALSE;
    return TRUE;
}



/*
    機能：  メモリ上の cmnHeadからテクスチャを作成する．（ラッパー関数）
    引数： gpD3DDevice     DirectX9デバイス 
            *pgpSurface     作成したテクスチャ
            cmnhead         メモリ上のデータを記述する CmnHead 型データ．
    戻値：  実行結果の BOOL
    説明：  サポートするデータ形式は  DirectX9 (jpg, png, bmp, ......), SUN Raster, CT_DATA
            作成したテクスチャのメモリ管理は D3DPOOL_DEFAULTになる．
*/
BOOL  jbxwl::Dx9TextureFromMemory(LPDIRECT3DDEVICE9 gpD3DDevice, LPDIRECT3DTEXTURE9* pgpTexture, CmnHead cmnHead)
{
    BOOL ret;

    if (cmnHead.kind==HEADER_NONE) return FALSE;
    int chk = cmnHead.kind & 0x00ff;

    if (chk==UN_KNOWN_DATA) ret = Dx9TextureFromMemoryFile(gpD3DDevice, pgpTexture, cmnHead);
    else                    ret = Dx9TextureFromByteMemory(gpD3DDevice, pgpTexture, cmnHead);
    return  ret;
}



//
//  メモリ上の cmnHeadからテクスチャを作成する．
//  DirectX9 がサポートするデータ(UN_KNOWN)用．cmnHead.grptrがメモリ上のファイル形式データを指す．
//
BOOL  jbxwl::Dx9TextureFromMemoryFile(LPDIRECT3DDEVICE9 gpD3DDevice, LPDIRECT3DTEXTURE9* pgpTexture, CmnHead cmnHead)
{
    HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(
                gpD3DDevice, cmnHead.grptr, cmnHead.lsize, 0, 0, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, //D3DPOOL_MANAGED, 
                D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, NULL, NULL, pgpTexture);
    if (FAILED(hr)) return FALSE;
    return  TRUE;
}



//
//  メモリ上の cmnHeadからテクスチャを作成する．
//  RAS_DATA(SUN Raster), CT_DATAなどの独自ファイル用．テクスチャデータは 8bit
//
BOOL  jbxwl::Dx9TextureFromByteMemory(LPDIRECT3DDEVICE9 gpD3DDevice, LPDIRECT3DTEXTURE9* pgpTexture, CmnHead cmnHead)
{
    HRESULT hr;
    RECT    rect;
    LPDIRECT3DSURFACE9 surface;

    hr = D3DXCreateTexture(gpD3DDevice, cmnHead.xsize, cmnHead.ysize, 0, D3DUSAGE_RENDERTARGET, D3DFMT_L8, D3DPOOL_DEFAULT, pgpTexture);
    if (FAILED(hr)) return FALSE; 

    hr = (*pgpTexture)->GetSurfaceLevel(0, &surface);
    if (FAILED(hr)) return FALSE; 

    rect.left   = 0;
    rect.top    = 0;
    rect.right  = cmnHead.xsize - 1;
    rect.bottom = cmnHead.ysize - 1;
    hr = D3DXLoadSurfaceFromMemory(surface, NULL, NULL, cmnHead.grptr, D3DFMT_L8, cmnHead.xsize, NULL, &rect, D3DX_FILTER_LINEAR, 0);
    DXRELEASE(surface);

    if (FAILED(hr)) return FALSE;
    return TRUE;
}



/*
    機能：  DrawPrimitiveのラッパー．Dx9DividePrimitiveModeが TRUEの時，
            数回に分けて DrawPrimitiveを呼び出す．

    引数： gpD3DDevice     DirectX9デバイス 
            ptyepe          プリミティブタイプ
            unit            １個のプリミティブを書くために必要な頂点数．
            bsize           プリミティブの数

    戻値：  実行結果の HRESULT

    説明：  ある種のカード は Primitiveの数が多いと誤作動する場合があるので，
            DXMAXPRIMITIVE個ずつ分けて表示する．
*/
HRESULT  jbxwl::Dx9DrawPrimitive(LPDIRECT3DDEVICE9 gpD3DDevice, D3DPRIMITIVETYPE ptype, UINT unit, UINT bsize)
{
    HRESULT hr;
    
    if (Dx9DividePrimitiveMode) {   // 分割描画モード
        UINT  iter = bsize / DXMAXPRIMITIVE;
        UINT  imod = bsize % DXMAXPRIMITIVE;

        unsigned int i;
        for (i=0; i<iter; i++) {
            hr = gpD3DDevice->DrawPrimitive(ptype, i*unit*DXMAXPRIMITIVE, DXMAXPRIMITIVE);
            if (FAILED(hr)) return hr;
        }
        hr = gpD3DDevice->DrawPrimitive(ptype, i*unit*DXMAXPRIMITIVE, imod);
        //if (FAILED(hr)) Dx9DividePrimitiveMode = FALSE;                 
    }
    else {                          // ノーマルモード
        hr = gpD3DDevice->DrawPrimitive(ptype, 0, bsize);
        //if (FAILED(hr)) Dx9DividePrimitiveMode = TRUE;                 
    }

    return hr;
}



POINT  jbxwl::Dx9GetSurfaceSize(LPDIRECT3DSURFACE9* lpsf)
{
    D3DSURFACE_DESC  desc;
    POINT  pt;

    (*lpsf)->GetDesc(&desc);
    pt.x = desc.Width;
    pt.y = desc.Height;

    return pt;
}


#pragma once

/**
DirectX9 ライブラリ

*/

#include  "..\stdafx.h"

#include  "common++.h"

#include  <d3d9.h>
#include  <d3dx9.h>
#include  <dxerr.h>

//
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION  DIRECTINPUT_HEADER_VERSION  // 0x0800
#endif
#include  <dinput.h>

#include  "Gdata.h"


//
namespace jbxwl {


extern  LPDIRECT3D9     GpD3D;
extern  LPDIRECTINPUT8  GpDInput;

extern  LPDIRECTINPUTDEVICE8  GpDMouse;         // マウスのインターフェイス
extern  LPDIRECTINPUTDEVICE8  GpDKeyBoard;      // キーボードのインターフェイス

extern  BOOL  Dx9DividePrimitiveMode;
extern  BOOL  Dx9ReverseZMode;

#define  CDXRELEASE(d)  if((d)!=NULL){while((d)->Release());(d)=NULL;}    // 完全リーリス（つまり削除）
#define   DXRELEASE(d)  if((d)!=NULL){(d)->Release();(d)=NULL;}

#define DXMAXPRIMITIVE  32768


BOOL    Dx9CreateInterface(CWinApp* wapp);
void    Dx9ReleaseInterface();

LPDIRECT3DDEVICE9    Dx9CreateGraphic(LPDIRECT3D9, D3DPRESENT_PARAMETERS*, HWND);
LPDIRECTINPUTDEVICE8 Dx9CreateMouse(LPDIRECTINPUT8, HWND);
LPDIRECTINPUTDEVICE8 Dx9CreateKeyBoard(LPDIRECTINPUT8, HWND);

POINT   Dx9GetBackBuffer(LPDIRECT3DDEVICE9, LPDIRECT3DSURFACE9*);
BOOL    Dx9SurfaceFromMemory(LPDIRECT3DDEVICE9, LPDIRECT3DSURFACE9*, CmnHead);
BOOL    Dx9TextureFromMemory(LPDIRECT3DDEVICE9, LPDIRECT3DTEXTURE9*, CmnHead);

BOOL    Dx9SurfaceFromByteMemory(LPDIRECT3DDEVICE9, LPDIRECT3DSURFACE9*, CmnHead);
BOOL    Dx9SurfaceFromMemoryFile(LPDIRECT3DDEVICE9, LPDIRECT3DSURFACE9*, CmnHead);
BOOL    Dx9TextureFromByteMemory(LPDIRECT3DDEVICE9, LPDIRECT3DTEXTURE9*, CmnHead);
BOOL    Dx9TextureFromMemoryFile(LPDIRECT3DDEVICE9, LPDIRECT3DTEXTURE9*, CmnHead);

HRESULT Dx9DrawPrimitive(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, UINT, UINT);
POINT   Dx9GetSurfaceSize(LPDIRECT3DSURFACE9* lpsf);



//
inline D3DCOLOR  Dx9Word2RGBA(uWord pp) 
{
    int r, g, b, a;

    if (isBigEndian) {
        r = (int)((pp&0xf000)>>12);
        g = (int)((pp&0x0f00)>>8);
        b = (int)((pp&0x00f0)>>4);
        a = (int) (pp&0x000f);
    }
    else {
        b = (int)((pp&0xf000)>>12);
        a = (int)((pp&0x0f00)>>8);
        r = (int)((pp&0x00f0)>>4);
        g = (int) (pp&0x000f);
    }

    return D3DCOLOR_RGBA(r*17, g*17, b*17, a*17);
}


//
inline D3DCOLOR  Dx9Word2ARGB(uWord pp) 
{
    int r, g, b, a;

    if (isBigEndian) {
        a = (int)((pp&0xf000)>>12);
        r = (int)((pp&0x0f00)>>8);
        g = (int)((pp&0x00f0)>>4);
        b = (int) (pp&0x000f);
    }
    else {
        g = (int)((pp&0xf000)>>12);
        b = (int)((pp&0x0f00)>>8);
        a = (int)((pp&0x00f0)>>4);
        r = (int) (pp&0x000f);
    }

    return D3DCOLOR_ARGB(a*17, r*17, g*17, b*17);
}


//
inline D3DCOLOR  Dx9Word2RGB(uWord pp) 
{
    int r, g, b;

    if (isLittleEndian) swap_byte(&pp, 2, 2);

    r = (int)((pp&0xf800)>>11);
    g = (int)((pp&0x07e0)>>5);
    b = (int) (pp&0x001f);

    return D3DCOLOR_XRGB((int)(r*8.23), (int)(g*4.05), (int)(b*8.23));
}



/**
See Also in Gdata.h

inline  uWord  ARGB2Word(uWord a, uWord r, uWord g, uWord b);
inline  uWord  RGBA2Word(uWord r, uWord g, uWord b, uWord a);
inline  uWord  RGB2Word (uWord r, uWord g, uWord b);
*/


}       // namespace

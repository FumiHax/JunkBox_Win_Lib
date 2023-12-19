#pragma once

/**
@brief    メディアバッファ ヘッダ  
@file     WinMediaBuffer.h
@version  0.9
@date     2013 1/7
@author   Fumi.Iseki

@attention
Original Sample is KinectAudioStream.h (Kinect Developer Toolkit)
*/

#include  "WinTools.h"

#include  <dmo.h>       // IMediaObject
#include  <stack>
#include  <queue>


// Lin
#pragma comment(lib, "dmoguids.lib")


namespace jbxwl {


class CWinMediaBuffer : public IMediaBuffer
{
public:
    CWinMediaBuffer(DWORD len=0);
    virtual ~CWinMediaBuffer(void) { free();}

    void    init(DWORD len);
    void    free(void);
    void    clear(void) { m_pBuffer->vldsz = 0;}
    //
    Buffer* getBuffer(void) { return m_pBuffer;}

public:
    ///////////////////////////////////////////////////
    // IUnknown methods
    //
    STDMETHODIMP_(ULONG) AddRef()  { return 2;}
    STDMETHODIMP_(ULONG) Release() { return 1;}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        if (riid==IID_IUnknown) {
            AddRef();
            *ppv = (IUnknown*)this;
            return NOERROR;
        }
        else if (riid==IID_IMediaBuffer) {
            AddRef();
            *ppv = (IMediaBuffer*)this;
            return NOERROR;
        }
        else return E_NOINTERFACE;
    }

public:
    ///////////////////////////////////////////////////
    // IMediaBuffer methods
    //
    STDMETHODIMP SetLength(DWORD length)         { m_pBuffer->vldsz = (int)length; return NOERROR;}
    STDMETHODIMP GetMaxLength(DWORD* pMaxLength) { *pMaxLength = (DWORD)m_pBuffer->bufsz; return NOERROR;}
    
    STDMETHODIMP GetBufferAndLength(BYTE** ppData, DWORD* pLength)
    {
        if (ppData!=NULL)  *ppData  = (BYTE*)m_pBuffer->buf;
        if (pLength!=NULL) *pLength = (DWORD)m_pBuffer->vldsz;
        return NOERROR;
    }

protected:
    //
    Buffer* m_pBuffer;

};


//
typedef std::stack<CWinMediaBuffer*> MediaBufferStack;
typedef std::queue<CWinMediaBuffer*> MediaBufferQueue;


}       // namespace

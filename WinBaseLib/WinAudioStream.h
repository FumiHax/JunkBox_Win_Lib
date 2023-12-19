#pragma once

/**
@brief    オーディオストリーム ヘッダ  
@file     WinAudioStream.h
@version  0.9
@date     2013 1/7
@author   Fumi.Iseki

@attention
Original Sample is KinectAudioStream.h (Kinect Developer Toolkit)
*/

#include  "WinMediaBuffer.h"

#include  <mmreg.h>     // WAVEFORMATEX
#include  <avrt.h>      // MMCSS(AvSetMmThreadCharacteristics)
#include  <MMSystem.h>  // outout of Wave


// Lib
#pragma comment(lib, "avrt.lib")
#pragma comment(lib, "winmm.lib")


//
namespace jbxwl {


class CWinAudioStream : public IStream
{
public:

    CWinAudioStream(IMediaObject *pObj);
    virtual ~CWinAudioStream(void);

    BOOL    startCapture(WAVEFORMATEX* format=NULL);
    void    stopCapture (void);
    Buffer  input(void);            // not free

    //
    BOOL    openOutput(ULONG bufcount=100, WAVEFORMATEX* format=NULL);
    void    closeOutput(void);
    void    output(Buffer buf);

private:
    //
    void    freeOutputHeaders(void);

public:
    /////////////////////////////////////////////
    // IUnknown methods
    //
    STDMETHODIMP_(ULONG) AddRef(void) { return InterlockedIncrement(&m_cRef);}

    STDMETHODIMP_(ULONG) Release(void)
    {
        UINT ref = InterlockedDecrement(&m_cRef);
        if (ref==0) delete this;
        return ref;
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        if (riid==IID_IUnknown) {
            AddRef();
            *ppv = (IUnknown*)this;
            return S_OK;
        }
        else if (riid==IID_IStream) {
            AddRef();
            *ppv = (IStream*)this;
            return S_OK;
        }
        else return E_NOINTERFACE;
    }

public:
    /////////////////////////////////////////////
    // IStream methods
    //
    STDMETHODIMP Read(void*, ULONG, ULONG*);
    STDMETHODIMP Seek(LARGE_INTEGER, DWORD, ULARGE_INTEGER*);
    STDMETHODIMP Write(const void*, ULONG, ULONG*);

    // Not Implement
    STDMETHODIMP SetSize(ULARGE_INTEGER) { return E_NOTIMPL;}
    STDMETHODIMP CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*) { return E_NOTIMPL;}
    STDMETHODIMP Commit(DWORD) { return E_NOTIMPL;}
    STDMETHODIMP Revert(void)  { return E_NOTIMPL;}
    STDMETHODIMP LockRegion  (ULARGE_INTEGER, ULARGE_INTEGER, DWORD) { return E_NOTIMPL;}
    STDMETHODIMP UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) { return E_NOTIMPL;}
    STDMETHODIMP Stat(STATSTG*, DWORD) { return E_NOTIMPL;}
    STDMETHODIMP Clone(IStream**)      { return E_NOTIMPL;}

private:
    CRITICAL_SECTION    m_lock;

    UINT                m_cRef;         // Number of references
    IMediaObject*       m_pMediaObj;

private:    // Input
    static const UINT   MaxReadBuffers = 20;

    HANDLE              m_hStopEvent;
    HANDLE              m_hDataReady;

    CWinThread*         m_captureThread;

    MediaBufferStack    m_bufferStack;
    MediaBufferQueue    m_bufferQueue;

    CWinMediaBuffer*    m_writeBuffer;
    CWinMediaBuffer*    m_readBuffer;

    ULONG               m_readBufferSize;
    ULONG               m_readBufferCount;
    ULONG               m_readBufferIndex;

private:    // Output
    static const UINT   MaxOutputBuffers = 2*1024*1024; 

    HWAVEOUT            m_hWave;
    WAVEHDR*            m_waveHeaders;

    ULONG               m_outputBufferNum;
    ULONG               m_outputBufferIndex;

    Buffer              m_streamData;

private:
    CWinMediaBuffer*    GetWriteBuffer(void);
    void                Back2BufferStack(CWinMediaBuffer* pMediaBuf);
    void                Back2BufferStackAll(void);

    void                QueueCapturedData(BYTE* pData, UINT cbData);
    void                QueueCapturedBuffer(CWinMediaBuffer* pMediaBuf);

    void                ReadOneBuffer(BYTE** ppbData, ULONG* pcbData);

    static UINT         CaptureThread(LPVOID pParam);
    UINT                CaptureThread(void);

    //
    BOOL IsCapturing(void)
    {
        return (m_hStopEvent!=NULL) && (WaitForSingleObject(m_hStopEvent,0)!=WAIT_OBJECT_0);
    }
};


}       // namespace



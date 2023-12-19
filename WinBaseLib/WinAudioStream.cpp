/**
@brief    オーディオストリーム 
@file     WinAudioStream.cpp
@version  0.9
@date     2013 1/7
@author   Fumi.Iseki

@attention
Original Sample is KinectAudioStream.cpp (Kinect Developer Toolkit)
*/

#include  "WinBaseLib.h"
#include  "WinAudioStream.h"

#include  <strsafe.h>       // StringCch
#include  <limits.h>        // INT_MAX


#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include  <math.h>


using namespace jbxl;
using namespace jbxwl;


CWinAudioStream::CWinAudioStream(IMediaObject* pObj)
{
    m_cRef              = 1;
    m_pMediaObj         = pObj;

    m_writeBuffer       = NULL;
    m_readBuffer        = NULL;
    m_readBufferSize    = 0;
    m_readBufferCount   = 0;
    m_readBufferIndex   = 0;
    //
    m_hStopEvent        = NULL;
    m_hDataReady        = NULL;
    //
    m_captureThread     = NULL;

    m_hWave             = NULL;
    m_waveHeaders       = NULL;
    m_outputBufferNum   = 0;
    m_outputBufferIndex = 0;

    m_streamData        = init_Buffer();

    if (m_pMediaObj!=NULL) m_pMediaObj->AddRef();

    InitializeCriticalSection(&m_lock);
}



CWinAudioStream::~CWinAudioStream()
{
    DEBUG_INFO("DESTRUCTOR: CWinAudioStream: START\n");
    //
    closeOutput();
    stopCapture();

    DeleteCriticalSection(&m_lock);

    DEBUG_INFO("DESTRUCTOR: CWinAudioStream: END\n");
}



BOOL  CWinAudioStream::startCapture(WAVEFORMATEX* fmt)
{
    if (m_pMediaObj==NULL && fmt==NULL) return FALSE;

    m_hStopEvent  = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hDataReady  = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_writeBuffer = NULL;
    m_readBufferCount = 0;

    //
    WAVEFORMATEX format;
    if (fmt!=NULL) {
        format = *fmt;
    }
    else {
        DMO_MEDIA_TYPE media;
        m_pMediaObj->GetOutputCurrentType(0, &media);
        format = *(WAVEFORMATEX*)media.pbFormat;
    }

    // make Stack
    m_readBufferSize = format.nSamplesPerSec*format.nBlockAlign;
    for (UINT i=0; i<MaxReadBuffers; i++) {
        CWinMediaBuffer* pMediaBuf = new CWinMediaBuffer(m_readBufferSize);
        m_bufferStack.push(pMediaBuf);
    }
    m_streamData = make_Buffer(m_readBufferSize);

    // スレッドの起動
    m_captureThread = AfxBeginThread(CaptureThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    m_captureThread->m_bAutoDelete = FALSE;
    m_captureThread->ResumeThread();
    ::Sleep(10);

    return TRUE;
}



void  CWinAudioStream::stopCapture(void)
{
    if (m_hStopEvent!=NULL) {
        SetEvent(m_hStopEvent);
        //
        if (m_captureThread!=NULL) {
            // スレッドが終了するまで待つ
            WaitForSingleObject(m_captureThread->m_hThread, INFINITE);
            CloseHandle(m_captureThread->m_hThread);
            delete m_captureThread;
            m_captureThread = NULL;
        }
        CloseHandle(m_hStopEvent);
        m_hStopEvent = NULL;
    }

    //
    if (m_hDataReady!=NULL) {
        SetEvent(m_hDataReady);
        CloseHandle(m_hDataReady);
        m_hDataReady = NULL;
    }

    //
    Back2BufferStackAll();

    // delete Stack
    while(!m_bufferStack.empty()) {
        CWinMediaBuffer* pMediaBuf = m_bufferStack.top();
        delete pMediaBuf;
        m_bufferStack.pop();
    }

    //
    free_Buffer(&m_streamData);

    return;
}



BOOL  CWinAudioStream::openOutput(ULONG bufcount, WAVEFORMATEX* fmt)
{
    if (m_pMediaObj==NULL && fmt==NULL) return FALSE;

    //
    WAVEFORMATEX format;
    if (fmt!=NULL) {
        format = *fmt;
    }
    else {
        DMO_MEDIA_TYPE media;
        m_pMediaObj->GetOutputCurrentType(0, &media);
        format = *(WAVEFORMATEX*)media.pbFormat;
    }

    //
    MMRESULT ret = ::waveOutOpen(&m_hWave, WAVE_MAPPER, &format, NULL, NULL, 0);
    if (ret!=MMSYSERR_NOERROR) return FALSE;

    m_outputBufferNum = bufcount;
    ULONG bufsz = sizeof(WAVEHDR)*m_outputBufferNum;
    m_waveHeaders = (WAVEHDR*)malloc(bufsz);
    memset(m_waveHeaders, 0, bufsz);

    for (ULONG i=0; i<m_outputBufferNum; i++) {
        m_waveHeaders[i].lpData  = (LPSTR)malloc(MaxOutputBuffers);
        m_waveHeaders[i].dwUser  = i;
        m_waveHeaders[i].dwFlags = WHDR_DONE;
    }

    return TRUE;
}



void  CWinAudioStream::closeOutput(void)
{
    freeOutputHeaders();

    if (m_hWave!=NULL) {
        ::waveOutPause(m_hWave);
        ::waveOutClose(m_hWave);
        m_hWave = NULL;
    }
}



Buffer  CWinAudioStream::input(void) 
{ 
    if (m_streamData.buf!=NULL) {
        ULONG len = 0;
        Read((void*)m_streamData.buf, (ULONG)m_streamData.bufsz, &len);
        m_streamData.vldsz = (int)len;
    }
    return m_streamData;
}
    


void  CWinAudioStream::output(Buffer buf)
{
    if (buf.buf==NULL || buf.vldsz<=0) return;
        
    Write((const void*)buf.buf, (ULONG)buf.vldsz, NULL);
    return;
}



void  CWinAudioStream::freeOutputHeaders(void)
{
    if (m_waveHeaders==NULL) return;

    for (ULONG i=0; i<m_outputBufferNum; i++) {
        if (m_waveHeaders[i].lpData!=NULL) {
            ::free(m_waveHeaders[i].lpData);
            m_waveHeaders[i].lpData = NULL;
        }
    }
    freeNull(m_waveHeaders);
}




////////////////////////////////////////////////////////////////////////////////////
// IStream methods

STDMETHODIMP  CWinAudioStream::Read(void* pData, ULONG cbBuffer, ULONG *pcbRead)
{
   if (pcbRead==NULL) return E_INVALIDARG;

    ULONG bytesPendingToRead = cbBuffer;                // バッファの残り

    while (bytesPendingToRead>0 && IsCapturing()) {
        ReadOneBuffer((BYTE**)&pData, &bytesPendingToRead);
        if (m_readBuffer==NULL) WaitForSingleObject(m_hDataReady, INFINITE);
    }

    ULONG bytesRead = cbBuffer - bytesPendingToRead;    // 読み込んだデータ長
    m_readBufferCount += bytesRead;

    *pcbRead = bytesRead;

    return S_OK;
}



STDMETHODIMP  CWinAudioStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition )   
{
    if (plibNewPosition!=NULL) plibNewPosition->QuadPart = m_readBufferCount + dlibMove.QuadPart;

    return S_OK;
}



STDMETHODIMP  CWinAudioStream::Write(const void* pbuf, ULONG bufsz, ULONG* dummy)
{
    if (pbuf==NULL || bufsz==0 || bufsz>MaxOutputBuffers) return E_INVALIDARG;

    WAVEHDR* header = &m_waveHeaders[m_outputBufferIndex];
    if (!(header->dwFlags & WHDR_DONE)) return E_FAIL ;

    MMRESULT ret = ::waveOutUnprepareHeader(m_hWave, header, sizeof(WAVEHDR));
    if (ret!=MMSYSERR_NOERROR) return E_FAIL;

    header->dwBufferLength = (DWORD)bufsz;
    header->dwFlags = 0;
    memcpy(header->lpData, pbuf, bufsz);

    ret = ::waveOutPrepareHeader(m_hWave, header, sizeof(WAVEHDR));
    if (ret!=MMSYSERR_NOERROR) {
        header->dwFlags = WHDR_DONE;
        return E_FAIL;
    }

    ret = ::waveOutWrite(m_hWave, header, sizeof(WAVEHDR));
    if (ret!=MMSYSERR_NOERROR) {
        header->dwFlags = WHDR_DONE;
        return E_FAIL;
    }

    m_outputBufferIndex = (m_outputBufferIndex + 1) % m_outputBufferNum;

    return S_OK;
}




////////////////////////////////////////////////////////////////////////////////////
// Private CWinAudioStream methods

CWinMediaBuffer*  CWinAudioStream::GetWriteBuffer()
{
    CWinMediaBuffer* pMediaBuf = NULL;

    EnterCriticalSection(&m_lock);

    if (m_bufferStack.size()>0) {   
        pMediaBuf = m_bufferStack.top();
        m_bufferStack.pop();
        pMediaBuf->SetLength(0);
    }                           
    else if (m_bufferQueue.size()>0) {
        pMediaBuf = m_bufferQueue.front();
        m_bufferQueue.pop();
        pMediaBuf->SetLength(0);
    }

    LeaveCriticalSection(&m_lock);

    return pMediaBuf;
}



void  CWinAudioStream::Back2BufferStack(CWinMediaBuffer* pMediaBuf)
{
    if (pMediaBuf!=NULL) {
        EnterCriticalSection(&m_lock);
        //
        pMediaBuf->SetLength(0);
        m_bufferStack.push(pMediaBuf);
        //
        LeaveCriticalSection(&m_lock);
    }
}



void  CWinAudioStream::Back2BufferStackAll()
{
    EnterCriticalSection(&m_lock);

    while (m_bufferQueue.size()>0) {
        CWinMediaBuffer* pMediaBuf = m_bufferQueue.front();
        m_bufferQueue.pop();
        Back2BufferStack(pMediaBuf);
    }

    if (m_readBuffer!=NULL) Back2BufferStack(m_readBuffer);

    m_readBufferIndex = 0;
    m_readBuffer = NULL;
    //
    LeaveCriticalSection(&m_lock);
}



void  CWinAudioStream::QueueCapturedData(BYTE* pData, UINT cbData)
{
    if (cbData<=0) return;

    if (m_writeBuffer==NULL) m_writeBuffer = GetWriteBuffer();
    if (m_writeBuffer==NULL) return;

    //
    BYTE* pWriteData  = NULL;
    DWORD cbWriteData = 0;
    DWORD cbMaxLength = 0;

    //
    m_writeBuffer->GetBufferAndLength(&pWriteData, &cbWriteData);
    m_writeBuffer->GetMaxLength(&cbMaxLength);

    if (cbWriteData+cbData<cbMaxLength) {
        memcpy(pWriteData+cbWriteData, pData, cbData);
        m_writeBuffer->SetLength(cbWriteData + cbData);
    }
    else {
        QueueCapturedBuffer(m_writeBuffer);
        //
        m_writeBuffer = GetWriteBuffer();
        m_writeBuffer->GetBufferAndLength(&pWriteData, &cbWriteData);
        //
        memcpy(pWriteData, pData, cbData);
        m_writeBuffer->SetLength(cbData);
    }
}



void  CWinAudioStream::QueueCapturedBuffer(CWinMediaBuffer* pMediaBuf)
{
    EnterCriticalSection(&m_lock);

    m_bufferQueue.push(pMediaBuf);
    SetEvent(m_hDataReady);

    LeaveCriticalSection(&m_lock);
}



void  CWinAudioStream::ReadOneBuffer(BYTE** ppbData, ULONG* pcbData)
{
    EnterCriticalSection(&m_lock);

    //
    if (m_readBuffer==NULL) {
        if(m_bufferQueue.size()!=0) {
            m_readBuffer = m_bufferQueue.front();
            m_bufferQueue.pop();
        }
    }

    //
    if (m_readBuffer!=NULL) {
        //Copy as much data as we can or need
        BYTE* pData = NULL;
        DWORD dwDataLength = 0;
        m_readBuffer->GetBufferAndLength(&pData, &dwDataLength);

        ULONG cbToCopy = min(dwDataLength-m_readBufferIndex, *pcbData);
        memcpy(*ppbData, pData+m_readBufferIndex, cbToCopy);
        *ppbData = (*ppbData) + cbToCopy;
        *pcbData = (*pcbData) - cbToCopy;
        m_readBufferIndex    += cbToCopy;

        //
        if (m_readBufferIndex>=dwDataLength) {
            Back2BufferStack(m_readBuffer);
            m_readBuffer = NULL;
            m_readBufferIndex = 0;

            if (m_bufferQueue.size()!=0) {
                m_readBuffer = m_bufferQueue.front();
                m_bufferQueue.pop();
            }
        }
    }

    LeaveCriticalSection(&m_lock);
}



UINT   CWinAudioStream::CaptureThread(LPVOID pParam)
{
    CWinAudioStream* pthis = (CWinAudioStream*)pParam;
    return pthis->CaptureThread();
}



UINT  CWinAudioStream::CaptureThread(void)
{
    HANDLE mmHandle = NULL;
    DWORD  mmTaskIndex = 0;

    HRESULT hr = S_OK;
    bool bContinue = true;
    
    BYTE* pbOutputBuffer = NULL;
    CWinMediaBuffer outputBuffer(m_readBufferSize);
    
    DMO_OUTPUT_DATA_BUFFER OutputBufferStruct = {0};
    OutputBufferStruct.pBuffer = (IMediaBuffer*)&outputBuffer;
    
    DWORD dwStatus   = 0;
    ULONG cbProduced = 0;

    //
    mmHandle = AvSetMmThreadCharacteristics(_T("Audio"), &mmTaskIndex);

    while(bContinue) {
        //
        if (WaitForSingleObject(m_hStopEvent, 0)==WAIT_OBJECT_0) {
            bContinue = false;
            continue;
        }

        do {
            outputBuffer.clear();
            OutputBufferStruct.dwStatus = 0;

            hr = m_pMediaObj->ProcessOutput(0, 1, &OutputBufferStruct, &dwStatus);
            if (FAILED(hr)) {
                bContinue = false;
                break;
            }
            else if (hr==S_FALSE) {
                cbProduced = 0;
            }
            else {
                outputBuffer.GetBufferAndLength(&pbOutputBuffer, &cbProduced);
            }

            // Queue にデータを保存
            if (cbProduced>0) {
                QueueCapturedData(pbOutputBuffer, cbProduced);
                //Write(pbOutputBuffer, cbProduced, NULL);  // for TEST
            }

        } while (OutputBufferStruct.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE);

        //
        Sleep(10);
    }

    //
    SetEvent(m_hDataReady);
    AvRevertMmThreadCharacteristics(mmHandle);

    //
    if (FAILED(hr)) return FALSE;
    return TRUE;
}



#pragma once

#if defined(ENABLE_OPENNI) || defined(ENABLE_OPENNI2)


#include  "WinAudioStream.h"
#include  "OpenNiTool.h"

#include  <mfapi.h>         // IPropertyStore
#include  <wmcodecdsp.h>    // MFPKEY_WMAAECMA_SYSTEM_MODE
#include  <uuids.h>         // FORMAT_WaveFormatEx

// Lib
#pragma comment(lib, "Msdmo.lib")
#pragma comment(lib, "amstrmid.lib")


//
namespace jbxwl {


using namespace jbxl;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COpenNiMedia

class COpenNiMedia : public IMediaObject
{
public:
    COpenNiMedia(void);
    ~COpenNiMedia(void) {}

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
            *ppv = (IMediaObject*)this;
            return NOERROR;
        }
        else return E_NOINTERFACE;
    }

public:
    ///////////////////////////////////////////////////
    // IMediaObject methods
    //
    HRESULT STDMETHODCALLTYPE SetOutputType(DWORD index, const DMO_MEDIA_TYPE *pmt, DWORD flags);
    HRESULT STDMETHODCALLTYPE GetOutputCurrentType(DWORD index, DMO_MEDIA_TYPE *pmt);
    HRESULT STDMETHODCALLTYPE ProcessOutput(DWORD dwFlags, DWORD oputBufferCount, DMO_OUTPUT_DATA_BUFFER *outputBuffers, DWORD *pdwStatus);

    // Not Implement
    HRESULT STDMETHODCALLTYPE GetStreamCount( 
            DWORD *pcInputStreams,
            DWORD *pcOutputStreams) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE GetInputStreamInfo( 
            DWORD dwInputStreamIndex,
            DWORD *pdwFlags) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE GetOutputStreamInfo( 
            DWORD dwOutputStreamIndex,
            DWORD *pdwFlags) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE GetInputType( 
            DWORD dwInputStreamIndex,
            DWORD dwTypeIndex,
            DMO_MEDIA_TYPE *pmt) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE GetOutputType( 
            DWORD dwOutputStreamIndex,
            DWORD dwTypeIndex,
            DMO_MEDIA_TYPE *pmt) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE SetInputType( 
            DWORD dwInputStreamIndex,
            const DMO_MEDIA_TYPE *pmt,
            DWORD dwFlags) { return E_NOTIMPL;}
 
    HRESULT STDMETHODCALLTYPE GetInputCurrentType( 
            DWORD dwInputStreamIndex,
            DMO_MEDIA_TYPE *pmt) { return E_NOTIMPL;}
 
    HRESULT STDMETHODCALLTYPE GetInputSizeInfo( 
            DWORD dwInputStreamIndex,
            DWORD *pcbSize,
            DWORD *pcbMaxLookahead,
            DWORD *pcbAlignment) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE GetOutputSizeInfo( 
            DWORD dwOutputStreamIndex,
            DWORD *pcbSize,
            DWORD *pcbAlignment) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE GetInputMaxLatency( 
            DWORD dwInputStreamIndex,
            REFERENCE_TIME *prtMaxLatency) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE SetInputMaxLatency( 
            DWORD dwInputStreamIndex,
            REFERENCE_TIME rtMaxLatency) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE Flush(void) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE Discontinuity( 
            DWORD dwInputStreamIndex) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE AllocateStreamingResources(void) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE FreeStreamingResources(void) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE GetInputStatus( 
            DWORD dwInputStreamIndex,
            DWORD *dwFlags) { return E_NOTIMPL;}
        
    HRESULT STDMETHODCALLTYPE ProcessInput(
            DWORD dwInputStreamIndex, 
            IMediaBuffer *pBuffer, 
            DWORD dwFlags, 
            REFERENCE_TIME rtTimestamp, 
            REFERENCE_TIME rtTimelength) { return E_NOTIMPL;}

    HRESULT STDMETHODCALLTYPE Lock(
            LONG bLock) { return E_NOTIMPL;}
};




/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COpenNiAudio

// Audio Parameters
static const WORD       OpenNiAudioFormat = WAVE_FORMAT_PCM;
static const WORD       OpenNiAudioChannels = 2;
static const DWORD      OpenNiAudioSamplesPerSec = 44100;
static const WORD       OpenNiAudioBitsPerSample = 16;

//static const WORD       OpenNiAudioBlockAlign = 2;
//static const DWORD      OpenNiAudioAverageBytesPerSec = 88200;



class COpenNiAudio
{
public:
    //
//  COpenNiAudio(xn::Context* context);
    COpenNiAudio(COpenNiTool* tool);
    virtual ~COpenNiAudio(void);

//  BOOL    init(xn::Context* context);
    BOOL    init(COpenNiTool* tool);
    void    free(void);
    IStream* getIStream(void) { return m_pStream;}

public:
    COpenNiMedia*   m_mediaObj;

public:
    //
    BOOL    setMediaType(void);

    BOOL    startCapture(void) { if (m_audioStream!=NULL) return m_audioStream->startCapture(); else return FALSE;}
    void    stopCapture(void)  { if (m_audioStream!=NULL) m_audioStream->stopCapture(); }

    Buffer  getAudioData(void) { if (m_audioStream!=NULL) return m_audioStream->input(); else return init_Buffer();} // not free
    void    getAudioData(Buffer* pbuf);

    //
    BOOL    openOutput(void)   { if (m_audioStream!=NULL) return m_audioStream->openOutput(); else return FALSE;}
    void    closeOutput(void)  { if (m_audioStream!=NULL) m_audioStream->closeOutput();}

public:
    //
    WAVEFORMATEX  getAudioFormat(void);

private:
    //
    CWinAudioStream*    m_audioStream;
//  xn::AudioGenerator* m_audioSource;

    IMediaObject*       m_audioMedia;
    IStream*            m_pStream;

private:
    // コピーしないで参照を返す
    COpenNiAudio& operator = (COpenNiAudio& rhs) { return rhs;}
};


}       // namespace


#endif

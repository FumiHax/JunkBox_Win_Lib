
#include  "OpenNiAudio.h"

#if defined(ENABLE_OPENNI) || defined(ENABLE_OPENNI2)


using namespace jbxl;
using namespace jbxwl;


// 作成中


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COpenNiMedia

COpenNiMedia::COpenNiMedia(void)
{
}


//
HRESULT STDMETHODCALLTYPE COpenNiMedia::GetOutputCurrentType(DWORD index, DMO_MEDIA_TYPE *pmt) 
{ 
    return S_OK;
}


//
HRESULT STDMETHODCALLTYPE COpenNiMedia::ProcessOutput(DWORD dwFlags, DWORD oputBufferCount, DMO_OUTPUT_DATA_BUFFER *outputBuffers, DWORD *pdwStatus)
{
    return S_OK;
}


//
HRESULT STDMETHODCALLTYPE COpenNiMedia::SetOutputType(DWORD index, const DMO_MEDIA_TYPE *pmt, DWORD flags)
{
    return S_OK;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COpenNiAudio

//COpenNiAudio::COpenNiAudio(xn::Context* context)
COpenNiAudio::COpenNiAudio(COpenNiTool* tool)
{
    m_audioStream   = NULL;
//  m_audioSource   = NULL;
    m_audioMedia    = NULL;
    m_pStream       = NULL;
    m_mediaObj      = new COpenNiMedia();

    //
    if (tool!=NULL) init(tool);
}



COpenNiAudio::~COpenNiAudio(void)
{
    DEBUG_ERROR("DESTRUCTOR: COpenNiAudio: START\n");

    free();

    DEBUG_ERROR("DESTRUCTOR: COpenNiAudio: END\n");
}



BOOL  COpenNiAudio::init(COpenNiTool* tool)
{
    if (tool==NULL) return FALSE;

    //
    free();
    /*
    XnStatus rc = XN_STATUS_ERROR;

    if (context!=NULL) {
        m_audioSource = new xn::AudioGenerator();
        if (m_audioSource!=NULL) {
            rc = m_audioSource->Create(*context);
        }

        if (rc==XN_STATUS_OK) {
            HRESULT hr = m_mediaObj->QueryInterface(IID_IMediaObject,   (void**)&m_audioMedia);
            if (SUCCEEDED(hr)) {
                BOOL ret = setMediaType();
                if (ret) {
                    m_audioStream = new CWinAudioStream(m_audioMedia);
                    m_audioStream->QueryInterface(IID_IStream, (void**)&m_pStream);
                    return TRUE;
                }
            }
        }
    }
    */
    return FALSE;
}



void  COpenNiAudio::free(void)
{
    releaseNull(m_pStream);
    deleteNull (m_audioStream);

    releaseNull(m_audioMedia);
    deleteNull (m_mediaObj);

//  releaseNull(m_audioSource);
}



BOOL  COpenNiAudio::setMediaType(void)
{
    WAVEFORMATEX format = getAudioFormat();
    
    /*
    XnWaveOutputMode mt;
    mt.nSampleRate    = format.nSamplesPerSec;
    mt.nChannels      = (XnUInt8)format.nChannels;
    mt.nBitsPerSample = format.wBitsPerSample;

    XnStatus ret = m_audioSource->SetWaveOutputMode(mt);

    if (ret==XN_STATUS_ERROR) return FALSE;
    */
    return TRUE;
}



void  COpenNiAudio::getAudioData(Buffer* pbuf)
{
    if (m_pStream==NULL || pbuf==NULL) return;

    ULONG len = 0;

    m_pStream->Read((void*)pbuf->buf, (ULONG)pbuf->bufsz, &len);
    pbuf->vldsz = (int)len;

    return;
}



WAVEFORMATEX  COpenNiAudio::getAudioFormat(void)
{
    WORD   blockAlign = ((OpenNiAudioBitsPerSample+7)/8)*OpenNiAudioChannels;
    DWORD  avgBytesPerSec = blockAlign*OpenNiAudioSamplesPerSec;

    WAVEFORMATEX format = {
        OpenNiAudioFormat, OpenNiAudioChannels, OpenNiAudioSamplesPerSec, 
        avgBytesPerSec, blockAlign, OpenNiAudioBitsPerSample, 0
    };
    return format;
}


#endif      // defined(ENABLE_OPENNI) || defined(ENABLE_OPENNI2)




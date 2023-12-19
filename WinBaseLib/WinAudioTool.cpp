
//#include  "WinBaseLib.h"
#include  "WinAudioTool.h"

/*
一般のマイク用オーディオツール

作成中
参考　 KinectAudio
*/


using namespace jbxl;
using namespace jbxwl;


CWinAudioTool::CWinAudioTool(void)
{
    m_audioStream   = NULL;
    m_audioMedia    = NULL;
    m_audioProperty = NULL;
    m_pStream       = NULL;

    //
    init();
}



CWinAudioTool::~CWinAudioTool(void)
{
    DEBUG_INFO("DESTRUCTOR: CWinAudioTool: START\n");

    free();

    DEBUG_INFO("DESTRUCTOR: CWinAudioTool: END\n");
}



BOOL  CWinAudioTool::init(void)
{
    free();

//  if (SUCCEEDED(hr)) hr = m_audioSource->QueryInterface(IID_IMediaObject,   (void**)&m_audioMedia);
//  if (SUCCEEDED(hr)) hr = m_audioSource->QueryInterface(IID_IPropertyStore, (void**)&m_audioProperty);
    //
    //BOOL ret = setProperty(mode);
    BOOL ret = setMediaType();
    if (ret) {
        m_audioStream = new CWinAudioStream(m_audioMedia);
        m_audioStream->QueryInterface(IID_IStream, (void**)&m_pStream);
        return TRUE;
    }

    return FALSE;
}



void  CWinAudioTool::free(void)
{
    releaseNull(m_pStream);
    deleteNull (m_audioStream);

    releaseNull(m_audioProperty);
    releaseNull(m_audioMedia);
}



BOOL  CWinAudioTool::setMediaType(void)
{
    if (m_audioMedia==NULL) return FALSE;

    DMO_MEDIA_TYPE mt = {0};
    MoInitMediaType(&mt, sizeof(WAVEFORMATEX));
    mt.majortype = MEDIATYPE_Audio;
    mt.subtype   = MEDIASUBTYPE_PCM;
    mt.lSampleSize = 0;
    mt.bFixedSizeSamples    = TRUE;
    mt.bTemporalCompression = FALSE;
    mt.formattype = FORMAT_WaveFormatEx;    
    memcpy(mt.pbFormat, &getAudioFormat(), sizeof(WAVEFORMATEX));
    
    HRESULT hr = m_audioMedia->SetOutputType(0, &mt, 0);
    MoFreeMediaType(&mt);

    if (FAILED(hr)) return FALSE;
    return TRUE;

    //
    if (SUCCEEDED(hr)) { 
        m_audioStream = new CWinAudioStream(m_audioMedia);
        m_audioStream->QueryInterface(IID_IStream, (void**)&m_pStream);
    }
}



void  CWinAudioTool::getAudioData(Buffer* pbuf)
{
    if (m_pStream==NULL || pbuf==NULL) return;

    ULONG len = 0;

    m_pStream->Read((void*)pbuf->buf, (ULONG)pbuf->bufsz, &len);
    pbuf->vldsz = (int)len;

    return;
}



WAVEFORMATEX  CWinAudioTool::getAudioFormat(void)
{
    WORD   blockAlign = ((WinAudioToolBitsPerSample+7)/8)*WinAudioToolChannels;
    DWORD  avgBytesPerSec = blockAlign*WinAudioToolSamplesPerSec;

    WAVEFORMATEX format = {
        WinAudioToolFormat, WinAudioToolChannels, WinAudioToolSamplesPerSec, 
        avgBytesPerSec, blockAlign, WinAudioToolBitsPerSample, 0
    };
    return format;
}



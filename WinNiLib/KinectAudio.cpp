
//#include  "WinBaseLib.h"
#include  "KinectAudio.h"


#ifdef  ENABLE_KINECT_SDK


using namespace jbxl;
using namespace jbxwl;


CKinectAudio::CKinectAudio(INuiSensor* sensor, int mode)
{
    m_audioStream   = NULL;
    m_audioSource   = NULL;
    m_audioMedia    = NULL;
    m_audioProperty = NULL;
    m_pStream       = NULL;

    //
    if (sensor!=NULL) init(sensor, mode);
}



CKinectAudio::~CKinectAudio(void)
{
    DEBUG_INFO("DESTRUCTOR: CKinectAudio: START\n");

    free();

    DEBUG_INFO("DESTRUCTOR: CKinectAudio: END\n");
}



BOOL  CKinectAudio::init(INuiSensor* sensor, int mode)
{
    free();

    if (sensor!=NULL) {
        HRESULT hr = sensor->NuiGetAudioSource(&m_audioSource);
        if (SUCCEEDED(hr)) hr = m_audioSource->QueryInterface(IID_IMediaObject,   (void**)&m_audioMedia);
        if (SUCCEEDED(hr)) hr = m_audioSource->QueryInterface(IID_IPropertyStore, (void**)&m_audioProperty);
        //
        if (SUCCEEDED(hr)) {
            BOOL ret = setProperty(mode);
            if (ret) ret = setMediaType();
            if (ret) {
                m_audioStream = new CWinAudioStream(m_audioMedia);
                m_audioStream->QueryInterface(IID_IStream, (void**)&m_pStream);
                return TRUE;
            }
        }
    }
    return FALSE;
}



void  CKinectAudio::free(void)
{
    releaseNull(m_pStream);
    deleteNull (m_audioStream);

    releaseNull(m_audioProperty);
    releaseNull(m_audioMedia);
    releaseNull(m_audioSource);
}



/**
Set AEC-MicArray DMO system mode. This must be set for the DMO to work properly.

mode
    KINECT_AUDIO_SINGLE_AEC (0): SINGLE_CHANNEL_AEC
    KINECT_AUDIO_ARRAY      (2): OPTIBEAM_ARRAY_ONLY
    KINECT_AUDIO_ARRAY_AEC  (4): OPTIBEAM_ARRAY_AND_AEC
    KINECT_AUDIO_SINGLE     (5): SINGLE_CHANNEL_NSAGC
*/
BOOL  CKinectAudio::setProperty(int mode)
{
    if (m_audioProperty==NULL) return FALSE;

    PROPVARIANT prop;
    
    PropVariantInit(&prop);
    prop.vt   = VT_I4;
    prop.lVal = (LONG)mode;
    m_audioProperty->SetValue(MFPKEY_WMAAECMA_SYSTEM_MODE, prop);
    PropVariantClear(&prop);

    return TRUE;
}



BOOL  CKinectAudio::setMediaType(void)
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
}



void  CKinectAudio::getAudioData(Buffer* pbuf)
{
    if (m_pStream==NULL || pbuf==NULL) return;

    ULONG len = 0;

    m_pStream->Read((void*)pbuf->buf, (ULONG)pbuf->bufsz, &len);
    pbuf->vldsz = (int)len;

    return;
}



WAVEFORMATEX  CKinectAudio::getAudioFormat(void)
{
    WORD   blockAlign = ((KinectAudioBitsPerSample+7)/8)*KinectAudioChannels;
    DWORD  avgBytesPerSec = blockAlign*KinectAudioSamplesPerSec;

    WAVEFORMATEX format = {
        KinectAudioFormat, KinectAudioChannels, KinectAudioSamplesPerSec, 
        avgBytesPerSec, blockAlign, KinectAudioBitsPerSample, 0
    };
    return format;
}


#endif      // ENABLE_KINECT_SDK



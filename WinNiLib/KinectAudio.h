#pragma once

#ifdef  ENABLE_KINECT_SDK


#include  "WinAudioStream.h"

#include  <NuiApi.h>
#include  <mfapi.h>         // IPropertyStore
#include  <wmcodecdsp.h>    // MFPKEY_WMAAECMA_SYSTEM_MODE
#include  <uuids.h>         // FORMAT_WaveFormatEx


// Lib
#pragma comment(lib, "Msdmo.lib")
#pragma comment(lib, "amstrmid.lib")
#pragma comment(lib, "Kinect10.lib")


#define     KINECT_AUDIO_SINGLE_AEC     0
#define     KINECT_AUDIO_ARRAY          2
#define     KINECT_AUDIO_ARRAY_AEC      4
#define     KINECT_AUDIO_SINGLE         5


//
namespace jbxwl {



// Audio Parameters
static const WORD       KinectAudioFormat = WAVE_FORMAT_PCM;
static const WORD       KinectAudioChannels = 1;
static const DWORD      KinectAudioSamplesPerSec = 16000;
static const WORD       KinectAudioBitsPerSample = 16;

//static const WORD       KinectAudioBlockAlign = 2;
//static const DWORD      KinectAudioAverageBytesPerSec = 32000;


class CKinectAudio
{
public:
    //
    CKinectAudio(INuiSensor* sensor=NULL, int mode=KINECT_AUDIO_SINGLE_AEC);
    virtual ~CKinectAudio(void);

    BOOL     init(INuiSensor* sensor, int mode=KINECT_AUDIO_SINGLE_AEC);
    void     free(void);
    IStream* getIStream(void) { return m_pStream;}
    
    //
    BOOL     setProperty(int mode);
    BOOL     setMediaType(void);

    BOOL     startCapture(void) { if (m_audioStream!=NULL) return m_audioStream->startCapture(); else return FALSE;}
    void     stopCapture(void)  { if (m_audioStream!=NULL) m_audioStream->stopCapture(); }

    Buffer   getAudioData(void) { if (m_audioStream!=NULL) return m_audioStream->input(); else return init_Buffer();} // not free
    void     getAudioData(Buffer* pbuf);

    //
    BOOL     openOutput(void)   { if (m_audioStream!=NULL) return m_audioStream->openOutput(); else return FALSE;}
    void     closeOutput(void)  { if (m_audioStream!=NULL) m_audioStream->closeOutput();}

public:
    //
    WAVEFORMATEX  getAudioFormat(void);

private:
    //
    CWinAudioStream*    m_audioStream;

    INuiAudioBeam*      m_audioSource;
    IMediaObject*       m_audioMedia;
    IPropertyStore*     m_audioProperty;
    IStream*            m_pStream;

private:
    // コピーしないで参照を返す
    CKinectAudio& operator = (CKinectAudio& rhs) { return rhs;}

};


}       // namespace

#endif

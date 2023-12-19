#pragma once

/*
一般のマイク用オーディオツール

WinAudioStream.h を使用する．

作成中
参考　 KinectAudio

*/

#include  "WinAudioStream.h"

#include  <mfapi.h>         // IPropertyStore
#include  <wmcodecdsp.h>    // MFPKEY_WMAAECMA_SYSTEM_MODE
#include  <uuids.h>         // FORMAT_WaveFormatEx


// Lib
#pragma comment(lib, "Msdmo.lib")
#pragma comment(lib, "amstrmid.lib")


//
namespace jbxwl {


// Audio Parameters
static const WORD       WinAudioToolFormat = WAVE_FORMAT_PCM;
static const WORD       WinAudioToolChannels = 1;
static const DWORD      WinAudioToolSamplesPerSec = 16000;
static const WORD       WinAudioToolBitsPerSample = 16;

//static const WORD       WinAudioToolBlockAlign = 2;
//static const DWORD      WinAudioToolAverageBytesPerSec = 32000;



class CWinAudioTool
{
public:
    //
    CWinAudioTool(void);
    virtual ~CWinAudioTool(void);

    BOOL     init(void);
    void     free(void);
    IStream* getIStream(void) { return m_pStream;}
    
//  BOOL     setProperty(int mode);
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

    IMediaObject*       m_audioMedia;
    IPropertyStore*     m_audioProperty;
    IStream*            m_pStream;

private:
    // コピーしないで参照を返す
    CWinAudioTool& operator = (CWinAudioTool& rhs) { return rhs;}

};


}       // namespace


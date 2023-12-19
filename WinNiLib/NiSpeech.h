#pragma once

/**
@brief    Speech Platform クラス ヘッダ  
@file     NiSpeech.h
@version  0.9
@date     2013 1/10
@author   Fumi.Iseki

@attention
Original Sample is SpeechBasics.h (Kinect Developer Toolkit)
*/


#ifdef  ENABLE_NI_SPEECH

#include  "NiToolWin.h"

#include  <sapi.h>
#include  <sphelper.h>


// Lib
#pragma  comment(lib, "sapi.lib")


#define  NI_SPEECH_LANG_US          "Language=409"
#define  NI_SPEECH_LANG_JP          "Language=411"

#define  NI_SPEECH_LANG_US_KINECT   "Language=409;Kinect=True"
#define  NI_SPEECH_LANG_JP_KINECT   "Language=411;Kinect=True"


//
namespace jbxwl {

//
// Speech クラス
//

class  CNiSpeech
{
public:
    CNiSpeech(IStream* stream=NULL, WAVEFORMATEX* format=NULL);
    virtual ~CNiSpeech(void) {}

    void    free(void);

    //
    BOOL    init(IStream* stream, WAVEFORMATEX* format);
    BOOL    create(LPCTSTR lang);
    BOOL    load(LPCTSTR file);
    
    //
    BOOL    start(double confidence);
    void    stop(void);

    void    setConfidence(double confd);

private:
    static UINT speechThread(LPVOID pParam);
    UINT        speechThread(void);
    void        process(void);

protected:
    virtual void  map2action(LPCTSTR tag, double confd);

protected:
    ISpStream*      m_pSpeechStream;
    ISpRecognizer*  m_pSpeechRecognizer;
    ISpRecoContext* m_pSpeechContext;
    ISpRecoGrammar* m_pSpeechGrammar;

    HANDLE          m_hSpeechEvent;
    HANDLE          m_hStopEvent;

    CWinThread*     m_speechThread;

    double          m_confidence;
};


}       // namespace


#endif      // ENABLE_NI_SPEECH

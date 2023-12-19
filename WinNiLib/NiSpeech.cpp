/**
@brief    Speech Platform クラス  
@file     NiSpeech.cpp
@version  0.9
@date     2013 1/10
@author   Fumi.Iseki

@attention
Original Sample is SpeechBasics.cpp (Kinect Developer Toolkit)
*/

#include  "WinBaseLib.h"
#include  "NiSpeech.h"



#ifdef  ENABLE_NI_SPEECH


using namespace jbxl;
using namespace jbxwl;


//////////////////////////////////////////////////////////////////////////////////////////
// Face Tracker クラス

CNiSpeech::CNiSpeech(IStream* stream, WAVEFORMATEX* format)
{
    m_pSpeechStream     = NULL;
    m_pSpeechRecognizer = NULL;
    m_pSpeechContext    = NULL;
    m_pSpeechGrammar    = NULL;

    m_hSpeechEvent      = NULL;
    m_hStopEvent        = NULL;
    m_speechThread      = NULL;

    m_confidence        = 0.1;

    if (stream!=NULL && format!=NULL) init(stream, format);
}



BOOL  CNiSpeech::init(IStream* stream, WAVEFORMATEX* format)
{
    DEBUG_INFO("CNiSpeech::init(): START\n");
    free();

    if (stream==NULL || format==NULL) return FALSE;

    HRESULT hr = CoCreateInstance(CLSID_SpStream, NULL, CLSCTX_INPROC_SERVER, __uuidof(ISpStream), (void**)&m_pSpeechStream);
    if (FAILED(hr)) {
        DEBUG_INFO("CNiSpeech::init(): ERR 1\n");
        return FALSE;
    }

    hr = m_pSpeechStream->SetBaseStream(stream, SPDFID_WaveFormatEx, format);
    if (FAILED(hr)) {
        DEBUG_INFO("CNiSpeech::init(): ERR 2\n");
        releaseNull(m_pSpeechStream);
        return FALSE;
    }

    DEBUG_INFO("CNiSpeech::init(): END\n");
    return TRUE;
}



void  CNiSpeech::free(void)
{
    DEBUG_INFO("CNiSpeech::free(): START\n");
    releaseNull(m_pSpeechGrammar);
    releaseNull(m_pSpeechContext);
    releaseNull(m_pSpeechRecognizer);
    //releaseNull(m_pSpeechStream); // sometimes crash 
    DEBUG_INFO("CNiSpeech::free(): END\n");
}



BOOL  CNiSpeech::create(LPCTSTR lang)
{
    DEBUG_INFO("CNiSpeech::create(): START\n");
    ISpObjectToken *pEngineToken = NULL;

    releaseNull(m_pSpeechContext);
    releaseNull(m_pSpeechRecognizer);
   
    HRESULT hr = CoCreateInstance(CLSID_SpInprocRecognizer, NULL, CLSCTX_INPROC_SERVER, __uuidof(ISpRecognizer), (void**)&m_pSpeechRecognizer);
    if (FAILED(hr)) {
        DEBUG_INFO("CNiSpeech::create(): ERR 1\n");
        return FALSE;
    }

    m_pSpeechRecognizer->SetInput(m_pSpeechStream, FALSE);
    hr = SpFindBestToken(SPCAT_RECOGNIZERS, lang, NULL, &pEngineToken);

    if (SUCCEEDED(hr)) {
        m_pSpeechRecognizer->SetRecognizer(pEngineToken);
        hr = m_pSpeechRecognizer->CreateRecoContext(&m_pSpeechContext);
        releaseNull(pEngineToken);
    }

    if (FAILED(hr)) {
        DEBUG_INFO("CNiSpeech::create(): ERR 2\n");
        releaseNull(m_pSpeechContext);
        releaseNull(m_pSpeechRecognizer);
        releaseNull(m_pSpeechStream);
        return FALSE;
    }

    DEBUG_INFO("CNiSpeech::create(): END\n");
    return TRUE;
}



BOOL  CNiSpeech::load(LPCTSTR file)
{
    DEBUG_INFO("CNiSpeech::load(): START\n");

    if (m_pSpeechContext==NULL) {
        DEBUG_INFO("CNiSpeech::load(): SpeechContext ERR\n");
        return FALSE;
    }

    releaseNull(m_pSpeechGrammar);

    HRESULT hr = m_pSpeechContext->CreateGrammar(1, &m_pSpeechGrammar);

    if (SUCCEEDED(hr)) hr = m_pSpeechGrammar->LoadCmdFromFile(file, SPLO_STATIC);
    if (FAILED(hr)) {
        DEBUG_INFO("CNiSpeech::load: ERR 2\n");
        releaseNull(m_pSpeechGrammar);
        return FALSE;
    }

    DEBUG_INFO("CNiSpeech::load(): END\n");
    return TRUE;
}



BOOL  CNiSpeech::start(double confidence=0.1)
{
    if (m_pSpeechGrammar==NULL || m_pSpeechRecognizer==NULL || m_pSpeechContext==NULL) return FALSE;
    DEBUG_INFO("CNiSpeech::start(): START\n");

//  stop();     // if working then stop

    m_confidence = confidence;
    //
    m_pSpeechGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);
    m_pSpeechRecognizer->SetRecoState(SPRST_ACTIVE_ALWAYS);
    DEBUG_INFO("CNiSpeech::start(): SetRecoState OK\n");
    ::DisPatcher();
    ::Sleep(100);

    m_pSpeechContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));
    HRESULT hr = m_pSpeechContext->Resume(0);
    if (FAILED(hr)) {
        DEBUG_INFO("CNiSpeech::start(): Resume ERR\n");
        free();
        return FALSE;
    }
    DEBUG_INFO("CNiSpeech::start(): Resume OK\n");

    m_hStopEvent   = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hSpeechEvent = m_pSpeechContext->GetNotifyEventHandle();
    //
    m_speechThread = AfxBeginThread(speechThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    m_speechThread->m_bAutoDelete = FALSE;
    m_speechThread->ResumeThread();
    ::Sleep(10);

    DEBUG_INFO("CNiSpeech::start(): END");
    return TRUE;
}



void  CNiSpeech::stop(void)
{
    DEBUG_INFO("CNiSpeech::stop(): START\n");

    if (m_hStopEvent!=NULL) {
        SetEvent(m_hStopEvent);
        //
        if (m_speechThread!=NULL) {
            WaitForSingleObject(m_speechThread->m_hThread, INFINITE);
            CloseHandle(m_speechThread->m_hThread);
            delete m_speechThread;  // as m_bAutoDelete==FALSE 
            m_speechThread = NULL;
            //
            //m_pSpeechContext->Pause(0);
            DEBUG_INFO("CNiSpeech::stop(): Pause OK\n");
       }
        CloseHandle(m_hStopEvent);
        m_hStopEvent = NULL;
    }

    //
    if (m_hSpeechEvent!=NULL) {
        CloseHandle(m_hSpeechEvent);
        m_hSpeechEvent = NULL;
    }

    DEBUG_INFO("CNiSpeech::stop(): END\n");
    return;
}



void  CNiSpeech::process(void)
{
    SPEVENT curEvent;
    ULONG   fetched = 0;

    m_pSpeechContext->GetEvents(1, &curEvent, &fetched);

    while (fetched>0) {
        if (curEvent.eEventId==SPEI_RECOGNITION && curEvent.elParamType==SPET_LPARAM_IS_OBJECT) {
            //
            ISpRecoResult* result = reinterpret_cast<ISpRecoResult*>(curEvent.lParam);
            SPPHRASE* pPhrase = NULL;
            result->GetPhrase(&pPhrase);
            //
            if (pPhrase!=NULL) {
                if (pPhrase->pProperties!=NULL && pPhrase->pProperties->pFirstChild!=NULL) {
                    const SPPHRASEPROPERTY* pSemanticTag = pPhrase->pProperties->pFirstChild;
                    //
                    map2action(pSemanticTag->pszValue, pSemanticTag->SREngineConfidence);
                }
                ::CoTaskMemFree(pPhrase);
            }
        }

        m_pSpeechContext->GetEvents(1, &curEvent, &fetched);
    }

    return;
}



void  CNiSpeech::setConfidence(double confd)
{
    if (confd>1.0)      confd = 1.0;
    else if (confd<0.0) confd = 0.0;

    m_confidence = confd;
}




/////////////////////////////////////////////////////////////
// スレッド

UINT  CNiSpeech::speechThread(LPVOID pParam)
{
    CNiSpeech* pthis = (CNiSpeech*)pParam;
    return pthis->speechThread();
}



UINT  CNiSpeech::speechThread(void)
{
    DEBUG_INFO("CNiSpeech::speechThread(): START\n");

    bool bContinue = true;
    
    while(bContinue) {
        //
        if (WaitForSingleObject(m_hStopEvent, 0)==WAIT_OBJECT_0) {
            bContinue = false;
            continue;
        }
        
        if (WaitForSingleObject(m_hSpeechEvent, 0)==WAIT_OBJECT_0) {
            process();
        }

        Sleep(10);
    }

    DEBUG_INFO("CNiSpeech::speechThread(): END\n");
    return TRUE;
}



void  CNiSpeech::map2action(LPCTSTR tag, double confd)
{
    WORD param;

    if (confd>=m_confidence) {
        param = TRUE;
        SendWinMessage(JBXWL_WM_SPEECH_EVENT, (WPARAM)&param, (LPARAM)tag);
    }
    else {
        param = FALSE;
        SendWinMessage(JBXWL_WM_SPEECH_EVENT, (WPARAM)&param, (LPARAM)_T("N/A"));
    }
}


#endif      // ENABLE_NI_SPEECH


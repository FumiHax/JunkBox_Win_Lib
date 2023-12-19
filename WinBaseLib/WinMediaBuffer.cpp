/**
@brief    メディアバッファ  
@file     WinMediaBuffer.cpp
@version  0.9
@date     2013 1/7
@author   Fumi.Iseki
*/

#include "WinMediaBuffer.h"


using namespace jbxwl;


CWinMediaBuffer::CWinMediaBuffer(DWORD len)
{
    m_pBuffer = NULL;
    init(len);
}



void  CWinMediaBuffer::init(DWORD len)
{
    free();

    m_pBuffer = new_Buffer(0);
    if (len>0) *m_pBuffer = make_Buffer(len);
}



void  CWinMediaBuffer::free(void) 
{
    if (m_pBuffer!=NULL) {
        del_Buffer(&m_pBuffer);
    }
    m_pBuffer = NULL;
}



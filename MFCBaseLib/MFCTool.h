#pragma once

#include  "MFCBase.h"


//
namespace jbxwl {


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CMenu

CMenu*  GetMenu_byID(CMenu* pMenu, UINT id);
CMenu*  GetMenu_byStringID(CMenu* pMenu, UINT id);
CMenu*  GetMenu_byName(CMenu* pMenu, LPCTSTR name);


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// プログラムの再起動

void  RebootProgram(void);
//
BOOL  WaitPidTerm(int tm);


}       // namespace

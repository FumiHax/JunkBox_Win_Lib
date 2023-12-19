#pragma once

/**
 MFCLib 用入出力関数

*/

#include  "MFCBase.h"
#include  "Gio.h"
#include  "ProgressBarDLG.h"


//
namespace jbxwl {

using namespace jbxl;


int  copyFileWithCounter(LPCTSTR src, LPCTSTR dst);
int  copyFPWithCounter  (FILE* src, FILE* dst, unsigned long int sz);

int  writeGraphicFileWithCounter(LPCTSTR fname, MSGraph<sWord> vp, int kind, BOOL mlt=FALSE, int fn=0, int tn=0);



}       // namespace


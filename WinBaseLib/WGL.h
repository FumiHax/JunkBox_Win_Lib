#pragma once

/**
OpenGL Extension Wrangler Library

*/

#include  "..\stdafx.h"
#include  "common++.h"


#ifdef  ENABLE_OPENGL

#include  <GL/glew.h>
#include  <GL/glext.h>


//
namespace jbxwl {


HGLRC  WGLCreateContext(CDC* dc, PIXELFORMATDESCRIPTOR* param);
void   WGLDeleteContext(HGLRC hrc);

BOOL   WGLGetCurrent(CDC* dc, HGLRC hrc);

}       // namespace



#endif          // ENABLE_OPENGL

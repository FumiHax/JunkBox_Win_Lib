/**
	OpenGL Extension Wrangler Library

*/

#include "WGL.h"


#ifdef  ENABLE_OPENGL

using namespace jbxl;
using namespace jbxwl;



/*
	機能：
	引数：
	戻値：
	説明：
*/
HGLRC  jbxwl::WGLCreateContext(CDC* pDC, PIXELFORMATDESCRIPTOR* param)
{
	HGLRC hrc = NULL;
	BOOL  ret = FALSE;

	//
    int pixelformat = ::ChoosePixelFormat(pDC->GetSafeHdc(), param);
	if (pixelformat) ret = ::SetPixelFormat(pDC->GetSafeHdc(), pixelformat, param);

	if (ret) {
		hrc = ::wglCreateContext(pDC->GetSafeHdc());
		if (hrc) {
			ret = ::wglMakeCurrent(pDC->GetSafeHdc(), hrc);
			if (!ret) {
				::wglDeleteContext(hrc);
				hrc = NULL;
			}
		}
	}
	return hrc;
}




BOOL  jbxwl::WGLGetCurrent(CDC* pDC, HGLRC hrc)
{
	return ::wglMakeCurrent(pDC->GetSafeHdc(), hrc);
}





void  jbxwl::WGLDeleteContext(HGLRC hrc)
{
    ::wglMakeCurrent(NULL, NULL);
    ::wglDeleteContext(hrc);
}



#endif			// ENABLE_OPENGL







#include  "Gdata.h"
#include  "NiToolWin.h"


using namespace jbxl;
using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////////
// Color

void  jbxwl::NiSetUserColor(int label, uByte* ptr, BOOL use_image)
{
    if (label==1) { 
        if (!use_image) ptr[0] = ptr[1] = 0;
        ptr[2] = NI_DETECT_COLOR_BRIGHT;            // R
    }
    else if (label==2) {
        if (!use_image) ptr[0] = ptr[2] = 0;
        ptr[1] = NI_DETECT_COLOR_BRIGHT;            // G
    }
    else if (label==3) {
        if (!use_image) ptr[1] = ptr[2] = 0;
        ptr[0] = NI_DETECT_COLOR_BRIGHT;            // B
    }
    else if (label==4) {
        if (!use_image) ptr[2] = 0;
        ptr[0] = ptr[1] = NI_DETECT_COLOR_BRIGHT;   // B, G
    }
    else if (label==5) {
        if (!use_image) ptr[1] = 0;
        ptr[0] = ptr[2] = NI_DETECT_COLOR_BRIGHT;   // B, R
    }
    else {
        if (!use_image) ptr[0] = 0;
        ptr[1] = ptr[2] = NI_DETECT_COLOR_BRIGHT;   // G, R
    }

    return;
}



unsigned int  jbxwl::NiGetSkeletonColor(int label)
{
    unsigned int col;

    if (label==1) { 
        col = BGRA2Int(255, 255, 255-NI_DETECT_COLOR_BRIGHT, 0);
    }
    else if (label==2) {
        col = BGRA2Int(255, 255-NI_DETECT_COLOR_BRIGHT, 255, 0);
    }
    else if (label==3) {
        col = BGRA2Int(255-NI_DETECT_COLOR_BRIGHT, 255, 255, 0);
    }
    else if (label==4) {
        col = BGRA2Int(255-NI_DETECT_COLOR_BRIGHT, 255-NI_DETECT_COLOR_BRIGHT, 255, 0);
    }
    else if (label==5) {
        col = BGRA2Int(255-NI_DETECT_COLOR_BRIGHT, 255, 255-NI_DETECT_COLOR_BRIGHT, 0);
    }
    else {
        col = BGRA2Int(255, 255-NI_DETECT_COLOR_BRIGHT, 255-NI_DETECT_COLOR_BRIGHT, 0);
    }

/*
    if (label==1) { 
        col = BGRA2Int(255, 255, 255, 0);
    }
    else if (label==2) {
        col = BGRA2Int(255, 255, 255, 0);
    }
    else if (label==3) {
        col = BGRA2Int(255, 255, 255, 0);
    }
    else if (label==4) {
        col = BGRA2Int(255, 255, 255, 0);
    }
    else if (label==5) {
        col = BGRA2Int(255, 255, 255, 0);
    }
    else {
        col = BGRA2Int(255, 255, 255, 0);
    }
    */

    return col;
}



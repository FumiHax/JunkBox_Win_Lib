#pragma once

//
#include  "MFCio.h"
#include  "MFCTool.h"


#define  MFCLIB_VERSION     "v5.0 '11 10/5" 
#define  MFCLIB_NANE        "MFCLib" 
#define  MFCLIB_COPYRIGHT   "by Fumi.Iseki (C) 2004" 


#include  "appCallBack.h"       // アプリケーションクラスへのコールバック用マクロ定義

// for DirectX
#include  "Dx2DView.h"          // 画像ファイル表示
#include  "DxGraphDoc.h"        // DirectXの機能を利用した画像ファイル読み込み
#include  "DxMGRView.h"         // GR画像のマルチスライス表示
//#include  "DxSMPLDisp.h"      // MSGraph<sWord> の簡易画像表示
#include  "DxVScrollView.h"     // 垂直方向のスクロールバー付き画像表示
#include  "DxSRenderView.h"     // ボリュームデータのサーフェイスレンダリング表示

#include  "DxDirectView.h"      // 直接データによる2D表示
//#include  "Dx3DDirectView.h"  // 直接データによる3D表示

//
#include  "RwGRDoc.h"
#include  "vThumbNailDoc.h"     // 複数の画像を読み込み，サムネイルを作成
#include  "vThumbNailFrame.h"   // サムネイル表示用フレーム

//
#include  "LogWndDoc.h"
#include  "LogWndFrame.h"
#include  "LogWndView.h"

//
//#include  "DxPolygonDoc.h"
//#include  "DxPolygonView.h"


#pragma once

#include  "DxBaseClass.h"


// DxPolygonView ビュー

class DxPolygonView : public CDx3DBaseView
{
    DECLARE_DYNCREATE(DxPolygonView)

protected:
    DxPolygonView();           // 動的生成で使用される protected コンストラクタ
    virtual ~DxPolygonView();

public:
    virtual void OnDraw(CDC* pDC);      // このビューを描画するためにオーバーライドされます。
#ifdef _DEBUG
    virtual void AssertValid() const;
#ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
    DECLARE_MESSAGE_MAP()
};

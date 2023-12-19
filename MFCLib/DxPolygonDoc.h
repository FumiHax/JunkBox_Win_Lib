#pragma once

#include  "ExClass.h"


// DxPolygonDoc ドキュメント

class DxPolygonDoc : public CExDocument
{
    DECLARE_DYNCREATE(DxPolygonDoc)

public:
    DxPolygonDoc();
    virtual ~DxPolygonDoc();
#ifndef _WIN32_WCE
    virtual void Serialize(CArchive& ar);   // ドキュメント I/O に対してオーバーライドされました。
#endif
#ifdef _DEBUG
    virtual void AssertValid() const;
#ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
    virtual BOOL OnNewDocument();

    DECLARE_MESSAGE_MAP()
};


#pragma once

#include  "DxBaseClass.h"


//
namespace jbxwl {


// CDx3DDirectView

class CDx3DDirectView : public CDxVTXBaseView
{
    DECLARE_DYNAMIC(CDx3DDirectView)

public:
    CDx3DDirectView();
    virtual ~CDx3DDirectView();

protected:
    DECLARE_MESSAGE_MAP()
    virtual void OnDraw(CDC* /*pDC*/);
public:
    virtual void OnInitialUpdate();
};


}       // namespace


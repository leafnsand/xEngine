#pragma once

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "ui/GObject.h"
//#include "n3spine/spinetypes.h"

//using namespace Spine;

NS_FGUI_BEGIN

class GSpine : public GObject
{
    FUI_RTTI_DERIVED(GSpine, GObject)
public:
    GSpine(){}
    virtual ~GSpine();

    CREATE_FUNC(GSpine);

    void Init(const AZStd::string& spineName);

    void RefreshTransform() override;
//    SpineId GetSpineId() { return _spineId; }

private:
//    SpineId _spineId;
};

NS_FGUI_END

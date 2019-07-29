#ifndef __GEARTEXT_H__
#define __GEARTEXT_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class GObject;

class GearText : public GearBase
{
    FUI_RTTI_DERIVED(GearText, GearBase)
public:
    GearText(GObject* owner);
    virtual ~GearText();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const AZStd::string&  pageId, const AZStd::string& value) override;
    void init() override;

private:
    unordered_map<AZStd::string, AZStd::string> _storage;
    AZStd::string _default;
};

NS_FGUI_END

#endif

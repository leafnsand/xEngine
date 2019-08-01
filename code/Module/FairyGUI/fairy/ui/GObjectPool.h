#ifndef __GOBJECTPOOL_H__
#define __GOBJECTPOOL_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"

NS_FGUI_BEGIN

class GObject;

class GObjectPool : public Ref
{
    FUI_RTTI_DERIVED(GObjectPool, Ref)
public:
    GObjectPool();
    ~GObjectPool();

    GObject* getObject(const AZStd::string& url);
    void returnObject(GObject* obj);

private:
    unordered_map<AZStd::string, Vector<GObject*>> _pool;
};

NS_FGUI_END

#endif

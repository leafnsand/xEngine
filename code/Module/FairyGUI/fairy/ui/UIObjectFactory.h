#ifndef __UIOBJECTFACTORY_H__
#define __UIOBJECTFACTORY_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"
#include "PackageItem.h"
#include "GLoader.h"

NS_FGUI_BEGIN

class UIObjectFactory
{
public:
    typedef function<GComponent*()> GComponentCreator;
    typedef function<GLoader*()> GLoaderCreator;

    static void setPackageItemExtension(const AZStd::string& url, GComponentCreator creator);
    static GObject* newObject(PackageItem* pi);
    static GObject* newObject(const AZStd::string& type);

    static void setLoaderExtension(GLoaderCreator creator);

private:
    static void resolvePackageItemExtension(PackageItem* pi);

    static unordered_map<AZStd::string, GComponentCreator> _packageItemExtensions;
    static GLoaderCreator _loaderCreator;

    friend class UIPackage;
};

NS_FGUI_END

#endif

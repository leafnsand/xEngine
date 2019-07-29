#ifndef __UIPACKAGE_H__
#define __UIPACKAGE_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "PackageItem.h"
#include "GObject.h"
#include <unordered_map>

NS_FGUI_BEGIN

struct AtlasSprite;
class PixelHitTestData;

class UIPackage
{
public:
    UIPackage();
    ~UIPackage();

    static UIPackage* getById(const AZStd::string& id);
    static UIPackage* getByName(const AZStd::string& name);
    static UIPackage* addPackage(const AZStd::string& descFilePath);
    static void removePackage(const AZStd::string& packageIdOrName);
    static void removeAllPackages();
    static GObject* createObject(const AZStd::string& pkgName, const AZStd::string& resName);
    static GObject* createObjectFromURL(const AZStd::string& url);
    static AZStd::string getItemURL(const AZStd::string& pkgName, const AZStd::string& resName);
    static PackageItem* getItemByURL(const AZStd::string& url);
    static AZStd::string normalizeURL(const AZStd::string& url);

    static Texture2D* getEmptyTexture() { return _emptyTexture; }

    const AZStd::string& getId() const { return _id; }
    const AZStd::string& getName() const { return _name; }

    PackageItem* getItem(const AZStd::string& itemId);
    PackageItem* getItemByName(const AZStd::string& itemName);
    void loadItem(const AZStd::string& resName);
    void loadItem(PackageItem* item);

    PixelHitTestData* getPixelHitTestData(const AZStd::string& itemId);

    static int _constructing;
    static const AZStd::string URL_PREFIX;

private:
    void create(const AZStd::string& assetPath);
    void decodeDesc(Data& data);
    void loadPackage();
    SpriteFrame* createSpriteTexture(AtlasSprite* sprite);
    void loadAtlas(PackageItem* item);
    void loadMovieClip(PackageItem* item);
    void loadFont(PackageItem* item);
    void loadComponent(PackageItem* item);
    void loadComponentChildren(PackageItem* item);
    void translateComponent(PackageItem* item);

    GObject* createObject(const AZStd::string& resName);
    GObject* createObject(PackageItem* item);

private:
    AZStd::string _id;
    AZStd::string _name;
    AZStd::string _assetPath;

    vector<PackageItem*> _items;
    unordered_map<AZStd::string, PackageItem*> _itemsById;
    unordered_map<AZStd::string, PackageItem*> _itemsByName;
    unordered_map<AZStd::string, AtlasSprite*> _sprites;
    unordered_map<AZStd::string, Data*> _descPack;
    unordered_map<AZStd::string, PixelHitTestData*> _hitTestDatas;
    AZStd::string _assetNamePrefix;
    AZStd::string _customId;
    bool _loadingPackage;

    static list<UIPackage*> _packageList;
    static unordered_map<AZStd::string, UIPackage*> _packageInstById;
    static unordered_map<AZStd::string, UIPackage*> _packageInstByName;
    static ValueMap _stringsSource;
    static Texture2D* _emptyTexture;
};

NS_FGUI_END

#endif

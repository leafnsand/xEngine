#ifndef __PACKAGEITEM_H__
#define __PACKAGEITEM_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "FieldTypes.h"

NS_FGUI_BEGIN

class UIPackage;
class DisplayListItem;
class UIObjectFactory;
class GComponent;
class BitmapFont;
class Animation;
class SpriteFrame;
class Texture2D;


class PackageItem : public Ref
{
public:
    PackageItem();
    virtual ~PackageItem();

    void load();

public:
    UIPackage* owner;

    PackageItemType type;
    AZStd::string id;
    AZStd::string name;
    int width;
    int height;
    AZStd::string file;
    bool decoded;
    bool exported;

    //atlas
    Texture2D* texture;

    //image
    Rect* scale9Grid;
    bool scaleByTile;
    int tileGridIndice;
    SpriteFrame* spriteFrame;
    SpriteFrame* refSpriteFrame;

    //movieclip
    Animation* animation;
    float delayPerUnit;
    float repeatDelay;

    //component
    TXMLDocument* componentData;
    vector<DisplayListItem*>* displayList;
    function<GComponent*()> extensionCreator;

    //font
    BitmapFont* bitmapFont;
};

class DisplayListItem
{
public:
    PackageItem* packageItem;
    AZStd::string type;
    TXMLElement* desc;
    int listItemCount;

    DisplayListItem(PackageItem* pi, const AZStd::string& type);
    virtual ~DisplayListItem();
};

NS_FGUI_END

#endif

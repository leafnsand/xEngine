#include "PackageItem.h"
#include "UIPackage.h"
#include "display/BitMapFont.h"
#include "display/SpriteFrame.h"

NS_FGUI_BEGIN

PackageItem::PackageItem() :
    owner(nullptr),
    width(0),
    height(0),
    decoded(false),
    exported(false),
    texture(nullptr),
    spriteFrame(nullptr),
    refSpriteFrame(nullptr),
    scale9Grid(nullptr),
    scaleByTile(false),
    tileGridIndice(0),
    animation(nullptr),
    repeatDelay(0),
    componentData(nullptr),
    displayList(nullptr),
    extensionCreator(nullptr),
    bitmapFont(nullptr)
{
}

PackageItem::~PackageItem()
{
    FUI_SAFE_DELETE(scale9Grid);
    if (displayList)
    {
        for (auto &it : *displayList)
            FUI_SAFE_DELETE(it);
        delete displayList;
    }
    FUI_SAFE_DELETE(componentData);
    if (bitmapFont) //bitmapfont will be released by fontatlas
        bitmapFont->ReleaseAtlas();

    if(spriteFrame)
    {
        delete spriteFrame;
        spriteFrame = nullptr;
    }
    if (texture)
    {
        delete texture;
        texture = nullptr;
    }

//    FUI_SAFE_RELEASE(animation);
//    FUI_SAFE_RELEASE(texture);
//    FUI_SAFE_RELEASE(spriteFrame);
}

void PackageItem::load()
{
    owner->loadItem(this);
}

DisplayListItem::DisplayListItem(PackageItem* pi, const AZStd::string& type)
    :packageItem(pi),
    type(type),
    desc(nullptr),
    listItemCount(0)
{
}

DisplayListItem::~DisplayListItem()
{
}

NS_FGUI_END
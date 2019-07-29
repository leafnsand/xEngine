#include "utils/FileUtils.h"
#include "display/SpriteFrame.h"
#include "display/Texture2D.h"
#include "display/FonsFont.h"
#include "gears/GearSize.h"
#include "render/FGuiCentral.h"
#include "AzCore/Asset/AssetManager.h"
#include "Asset/FairyAsset.h"

#include <AzCore/Math/Vector2.h>
#include "Font/EBus/FontSystemComponentBus.h"
#include "Renderer/Asset/TextureAsset.h"
#include "Renderer/EBus/RendererSystemComponentBus.h"

using namespace fairygui;

FUI__Implement_Singleton(FileUtils)

FileUtils::Status FileUtils::loadAsset(const AZStd::string& path, Data& data)
{
    auto asset = AZ::Data::AssetManager::Instance().FindAsset<Module::FairyAsset>(path.c_str());
    if(asset.IsReady())
    {
        data.copy((unsigned char*)asset.Get()->GetData(), asset.Get()->GetLength());
        return FileUtils::Status::OK;
    }
    return FileUtils::Status::OpenFailed;
}

FileUtils::Status FileUtils::loadAsset(const AZStd::string& path, AZStd::string& str)
{
    auto asset = AZ::Data::AssetManager::Instance().FindAsset<Module::FairyAsset>(path.c_str());
    if (asset.IsReady())
    {
        str.clear();
        str.assign((char*)asset.Get()->GetData(), 0, asset.Get()->GetLength());
        return FileUtils::Status::OK;
    }
    return FileUtils::Status::OpenFailed;
}

bool FileUtils::isFileExist(const AZStd::string& filename)
{
    auto id = AZ::Data::AssetManager::Instance().GetAssetIdByPath(filename.c_str(), AZ::AzTypeInfo<Module::FairyAsset>::Uuid());
    if(id.IsValid())
    {
        auto asset = AZ::Data::AssetManager::Instance().FindAsset(id);
        return asset.IsReady();
    }
    return false;
}

//-------------------------------------------------------------------------------------
Texture2D* Texture2D::create(AZStd::string& filename)
{
    Texture2D* pt = new Texture2D();

    AZ::Data::Asset<Module::TextureAsset> texAsset;
    EBUS_EVENT_RESULT(texAsset, Module::RendererSystemRequestBus, GetTexture, filename);
    pt->data = texAsset.GetData();

    return pt;
}

Texture2D::~Texture2D()
{
    if (this->data && !this->isFont)
    {
        AZ::Data::Asset<Module::TextureAsset> asset = (Module::TextureAsset*)this->data;
        EBUS_EVENT(Module::RendererSystemRequestBus, ReleaseTexture, asset);
        this->data = nullptr;
    }
}

int Texture2D::getWidth()
{
    if (this->data)
    {
        auto texInfo = ((Module::TextureAsset*)this->data)->GetInfo();
        return texInfo.width;
    }
    return 0;
}

int Texture2D::getHeight()
{
    if (this->data)
    {
        auto texInfo = ((Module::TextureAsset*)this->data)->GetInfo();
        return texInfo.height;
    }
    return 0;
}

//-------------------------------------------------------------------------------------
Texture2D* FonsFont::GetTextTexture(AZStd::string& fontName)
{
    Texture2D* pt = new Texture2D();

    AZ::Data::Asset<Module::TextureAsset> asset;
    EBUS_EVENT_RESULT(asset, Module::FontSystemRequestBus, GetTexture, fontName.c_str());
    pt->data = asset.Get();

    pt->isFont = true;

    return pt;
}

int FonsFont::GetFonsFontIndex(const char* face)
{
    int ret = 0;
    EBUS_EVENT_RESULT(ret, Module::FontSystemRequestBus, GetFonsFontIndex, face);
    return ret;
}

bool FonsFont::Decutf8(unsigned int& utf8state, unsigned int& codepoint, const char* text)
{
    bool flag = false;
    EBUS_EVENT_RESULT(flag, Module::FontSystemRequestBus, Decutf8, utf8state, codepoint, text);
    return flag;
}

Size FonsFont::GetTextBounds(AZStd::string fontName, float fontSize, const char* text, const char* end)
{
    float outWidth, outHeight;
    EBUS_EVENT(Module::FontSystemRequestBus, GetTextBounds, fontName.c_str(), text, fontSize, outWidth, outHeight, end);
    return Size(outWidth, outHeight);
}

void FonsFont::CaculateFont(AZStd::string fontName, Vec2 beginPos, int fontSize, const char* text, AZStd::vector<Vec2>& fontVertex)
{
    EBUS_EVENT(Module::FontSystemRequestBus, BeginText, fontName.c_str(), beginPos.x, beginPos.y, text, fontSize);
    bool ret = false;
    AZ::Vector2 posMin, posMax, uvMin, uvMax;
    EBUS_EVENT_RESULT(ret, Module::FontSystemRequestBus, GetNextCharacter, posMin, posMax, uvMin, uvMax);
    while (ret)
    {
        fontVertex.push_back(Vec2(posMin.GetX(), posMin.GetY()));
        fontVertex.push_back(Vec2(posMax.GetX(), posMax.GetY()));
        fontVertex.push_back(Vec2(uvMin.GetX(), uvMin.GetY()));
        fontVertex.push_back(Vec2(uvMax.GetX(), uvMax.GetY()));

        EBUS_EVENT_RESULT(ret, Module::FontSystemRequestBus, GetNextCharacter, posMin, posMax, uvMin, uvMax);
    }
    EBUS_EVENT(Module::FontSystemRequestBus, EndText);
}

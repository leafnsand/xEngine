#include "spine/extension.h"

#include "Renderer/EBus/RendererSystemComponentBus.h"

void _spAtlasPage_createTexture(spAtlasPage* self, const char* path)
{
    AZ::Data::Asset<Module::TextureAsset> asset;
    EBUS_EVENT_RESULT(asset, Module::RendererSystemRequestBus, GetTexture, path);
    self->rendererObject = asset.GetData();
}

void _spAtlasPage_disposeTexture(spAtlasPage* self)
{
    AZ::Data::Asset<Module::TextureAsset> asset = (Module::TextureAsset*)self->rendererObject;
    EBUS_EVENT(Module::RendererSystemRequestBus, ReleaseTexture, asset);
}

char* _spUtil_readFile(const char* path, int* length)
{
    return nullptr;
}

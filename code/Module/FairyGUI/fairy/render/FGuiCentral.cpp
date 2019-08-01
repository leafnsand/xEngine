#include "FGuiCentral.h"
#include "display/SpriteFrame.h"
#include "display/FonsFont.h"
#include "extension/GSpine.h"
#include "extension/GParticle2d.h"
#include "BatchNode.h"
#include "RenderDataProcess.h"
#include "Renderer/Asset/TextureAsset.h"
#include "bgfx/embedded_shader.h"
#include "base/AutoreleasePool.h"
#include "utils/FileUtils.h"

#include <Window/EBus/WindowSystemComponentBus.h>
#include <bgfx/bgfx.h>

#include "Renderer/Base/Material.h"
#include "Renderer/EBus/RendererComponentBus.h"
#include "EBus/FairySystemComponentBus.h"

NS_FGUI_BEGIN

#include "vs_fairy.bin.h"
#include "fs_fairy_normal.bin.h"
#include "fs_font_normal.bin.h"

static const bgfx::EmbeddedShader s_embeddedShaders[] =
{
    BGFX_EMBEDDED_SHADER(vs_fairy),
    BGFX_EMBEDDED_SHADER(fs_fairy_normal),
    BGFX_EMBEDDED_SHADER(fs_font_normal),
    BGFX_EMBEDDED_SHADER_END()
};
//-----------------------------------------------------------------------------------------------------------------
FUI__Implement_Singleton(FGuiCentral)

using Module::Material;

AZStd::shared_ptr<Module::Material> FGuiCentral::GetMaterial(MaterialType materialType, Module::TextureAsset* textureAsset)
{
    if (textureAsset==nullptr)
    {
        return nullptr;
    }
    int index = textureAsset->GetHandle().idx;
    AZStd::string ext = AZStd::string::format("_%d", index);
    AZStd::shared_ptr<Material> material = nullptr;
    switch (materialType)
    {
    case MaterialType::Fairy_Normal:
        EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "fairy_normal" + ext);
        if (material == nullptr)
        {
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "fairy_normal", "fairy_normal" + ext);
        }
     
        break;
    case MaterialType::Font_Normal:     
        EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "font_normal" + ext);
        if (material == nullptr)
        {
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "font_normal", "font_normal" + ext);
        }
        break;
    }
    if (material)
    {
        material->SetTexture("s_texColor", textureAsset);
    }
    return material;
}

void FGuiCentral::SetDefaultSize(int width, int height)
{
    RenderDataProcess::SetDefaultSize(width, height);
    if (UIRoot)
    {
        UIRoot->setWidth((float)width);
        UIRoot->setHeight((float)height);
    }
}

void FGuiCentral::ChangeViewSize(int width, int height)
{
    if(UIRoot)
    {
        UIRoot->setWidth((float)width);
        UIRoot->setHeight((float)height);
    }
    RenderDataProcess::ResetZoom(width, height);
}

FGuiCentral::FGuiCentral()
{
 

    auto vertexShaderHandle = bgfx::createEmbeddedShader(s_embeddedShaders, bgfx::getRendererType(), "vs_fairy");
    auto normalShaderHandle = bgfx::createEmbeddedShader(s_embeddedShaders, bgfx::getRendererType(), "fs_fairy_normal");
    auto fontShaderHandle = bgfx::createEmbeddedShader(s_embeddedShaders, bgfx::getRendererType(), "fs_font_normal");
    m_normalShader = bgfx::createProgram(vertexShaderHandle, normalShaderHandle);
    m_fontShader = bgfx::createProgram(vertexShaderHandle, fontShaderHandle);
    bgfx::destroy(fontShaderHandle);
    bgfx::destroy(normalShaderHandle);
    bgfx::destroy(vertexShaderHandle);

    bgfx::VertexDecl vertexDecl;
    vertexDecl
        .begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
        .end();

    m_vertexBufferHandle = bgfx::createDynamicVertexBuffer(std::numeric_limits<AZ::u16>::max(), vertexDecl);
    m_indexBufferHandle = bgfx::createDynamicIndexBuffer(std::numeric_limits<AZ::u16>::max());

    m_textureUniform = bgfx::createUniform("s_texColor", bgfx::UniformType::Int1);
}

FGuiCentral::~FGuiCentral()
{
    WeakPtr::ClearAll();
    UIPackage::removeAllPackages();

    m_meshContext.clear();
    m_vecRenderObj.clear();

    bgfx::destroy(m_vertexBufferHandle);
    bgfx::destroy(m_indexBufferHandle);
    bgfx::destroy(m_textureUniform);
    bgfx::destroy(m_normalShader);
    bgfx::destroy(m_fontShader);


 
}

void FGuiCentral::Update(float delta)
{
    deltaTime = delta;
    Scheduler::getInstance()->update(delta);
    if (UIRoot)
    {
        //update transform
        UIRoot->RefreshTransform();
    }
}

NS_FGUI_END

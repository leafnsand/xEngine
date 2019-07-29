#pragma once
#include "FairyGUIMacros.h"
#include "fairytypes.h"
#include "render/RenderTypes.h"
#include "bgfx/bgfx.h"
#include "Renderer/Base/Material.h"

NS_FGUI_BEGIN

class BatchNode;
class FGuiCentral
{
    FUI_Declare_Singleton(FGuiCentral)
public:
    FGuiCentral();
    virtual ~FGuiCentral();

    void Update(float delta);
    void PreRender();
    void Render();

    void SetDefaultSize(int width, int height);
    void ChangeViewSize(int width, int height);
    
    void BeginMask(GObject* pMask);
    void EndMask();

    void UpdateMeshBuffer();
    void ApplySubMesh(int index);

    float deltaTime;
private:
    bool CollectData(const Vector<GObject*>& vec);

    void DealWithChildren(GComponent* parent);

    bool IsFairyExtension(GObject* obj, FairyRenderType& type);

private:

    enum MaterialType
    {
        Fairy_Normal = 0,
        Font_Normal = 10
    };

    bool m_showMask = false;
    Rect m_maskRect = Rect::ZERO;

    AZStd::shared_ptr<Module::Material> GetMaterial(MaterialType, Module::TextureAsset*);

    MeshContext m_meshContext;

    vector<RenderQueueObject> m_vecRenderObj;
    FairyRenderType m_currentRenderType;

    int m_maskIndex = 0;
    BatchNode* m_renderNode = nullptr;
    MaskNode* m_currentMaskNode = nullptr;

    bgfx::DynamicVertexBufferHandle m_vertexBufferHandle = BGFX_INVALID_HANDLE;
    bgfx::DynamicIndexBufferHandle m_indexBufferHandle = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle m_textureUniform = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle m_normalShader = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle m_fontShader = BGFX_INVALID_HANDLE;
};

NS_FGUI_END
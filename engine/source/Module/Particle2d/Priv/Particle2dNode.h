#pragma once

#include "AzCore/Asset/AssetCommon.h"

#include "Particle2d/Priv/Particle2dTypes.h"
#include "Renderer/Asset/TextureAsset.h"
#include "Renderer/Base/Material.h"

#include "AzCore/Memory/Memory.h"
#include "AzCore/Memory/SystemAllocator.h"

// #include "AzCore/Component/EntityId.h"

namespace Module
{
    class Particle2dComponent;
}

namespace Particle2d
{
	class Particle2dNode : public AZ::Data::AssetBus::Handler
	{
	public:
		AZ_CLASS_ALLOCATOR(Particle2dNode, AZ::SystemAllocator, 0);

		Particle2dNode();
		~Particle2dNode();

        void Init(Module::Particle2dComponent* componentPtr, Particle2DStructData* structPtr, Particle2DLauncherData* dataPtr);

		void Discard();

		void BRUpdate();

		void PreRender();

		void Render();

		int GetVertexBufferIndex();

		void Clean();

        AZStd::shared_ptr<Module::Material> GetMaterial(AZStd::string& name, Module::TextureAsset* textureAsset);

        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset);

        AZStd::vector<int> m_childNodeVector;

        int m_childNodeNum = 0;

        AZ::Data::Asset<Module::TextureAsset> m_textureAsset;

		Particle2DLauncherData* m_dataPtr;

        Module::Particle2dComponent* m_component;
        
		int TotalNeedRenderUnitNum = 0;

		int VertexBufferStartIndex = 0;

		int VertexBufferDrawCount = 0;

		int CurVertexBufferIndex = 0;
       
       ;
    };
}

#pragma once

#include "AzCore/Asset/AssetManager.h"

#include "Particle2d/Priv/Particle2dTypes.h"

namespace Module
{
	class Particle2dDescAsset : public AZ::Data::AssetData
	{
	public:
		AZ_CLASS_ALLOCATOR(Particle2dDescAsset, AZ::SystemAllocator, 0);
		AZ_RTTI(Particle2dDescAsset, "{2AA8A5C0-10D8-4363-BD66-C2D6B2C8947C}", AZ::Data::AssetData);

		~Particle2dDescAsset() override;

		Particle2d::Particle2DSystemDesc& GetDesc()
		{
			return m_desc;
		}

	private:
		Particle2d::Particle2DSystemDesc m_desc;

		friend class Particle2dDescAssetHandler;
	};
}
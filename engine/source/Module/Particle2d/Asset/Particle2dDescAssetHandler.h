#pragma once

#include "Particle2d/Asset/Particle2dDescAsset.h"
#include "AzCore/Asset/GenericAssetHandler.h"

namespace Module
{
	class Particle2dDescAssetHandler : public AZ::GenericAssetHandler<Particle2dDescAsset>
	{
	public:
		AZ_CLASS_ALLOCATOR(Particle2dDescAssetHandler, AZ::SystemAllocator, 0);

		bool OnLoadAssetData(const AZ::Data::Asset<AZ::Data::AssetData>& asset, void* buffer, size_t length) override;

	private:
		void DecodeDesc();
	};
}
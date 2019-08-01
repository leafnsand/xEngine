#include "Particle2d/Component/Particle2dSystemComponent.h"
#include "Particle2d/Asset/Particle2dDescAssetHandler.h"
#include "AzCore/Serialization/SerializeContext.h"
namespace Module
{
    void Particle2dSystemComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            
            serializeContext->Class<Particle2dSystemComponent>()
                ->SerializerForEmptyClass();
        }
    }

	Particle2dSystemComponent::Particle2dSystemComponent()
	{
		
	}

	void Particle2dSystemComponent::Init()
	{

	}

	void Particle2dSystemComponent::Activate()
	{
        m_assetHandlers.emplace_back(aznew Particle2dDescAssetHandler);
    
	}

	void Particle2dSystemComponent::Deactivate()
	{
        m_assetHandlers.clear();
  
	}




}
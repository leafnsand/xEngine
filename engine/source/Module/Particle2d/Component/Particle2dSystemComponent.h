#pragma once

#include "AzCore/Component/Component.h"
#include "Particle2dComponent.h"

namespace Module
{
	class Particle2dSystemComponent
		: public AZ::Component
	{
	public:
		AZ_COMPONENT(Particle2dSystemComponent, "{F672C19B-3674-43C6-B304-00C0616473A3}");

		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
		{
			provided.push_back(AZ_CRC("Particle2dSystemService"));
		}

		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
		{
			incompatible.push_back(AZ_CRC("Particle2dSystemService"));
		}

		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
		{
			
		}

		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
		{
			
		}

        static void Reflect(AZ::ReflectContext* reflection);

		Particle2dSystemComponent();
		~Particle2dSystemComponent() override = default;
		Particle2dSystemComponent(const Particle2dSystemComponent&) = delete;

		// AZ::Component
		void Init() override;
		void Activate() override;
		void Deactivate() override;
	protected:
		// Particle2dSystemRequestBus::Handler
     
	private:

        AZStd::vector<AZStd::unique_ptr<AZ::Data::AssetHandler>> m_assetHandlers;
	
	};
}

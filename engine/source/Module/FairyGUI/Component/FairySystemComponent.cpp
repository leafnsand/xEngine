#include <FairyGUI/Component/FairySystemComponent.h>

#include <AzCore/Input/Devices/Mouse/InputDeviceMouse.h>
#include "Window/EBus/WindowSystemComponentBus.h"
#include "Font/EBus/FontSystemComponentBus.h"

#include <bx/math.h>

#include "render/RenderDataProcess.h"
#include "render/FGuiCentral.h"
#include "AzCore/Input/Devices/Touch/InputDeviceTouch.h"

#include <AzCore/Serialization/SerializeContext.h>
#include "AzCore/Component/TransformComponent.h"
#include "FairyPanelComponent.h"
#include "AzCore/Component/GameEntityContextBus.h"
#include "Renderer/Component/RendererComponent.h"
#include "FairyGUI/EBus/FairySystemComponentBus.h"


#include "AzCore/RTTI/BehaviorContext.h"
#include "AzCore/RTTI/TypeInfo.h"
#include "AzCore/RTTI/RTTI.h"

#include "FairyGUI/lua/n3lua_fairy.h"
#include "AzCore/Script/ScriptSystemBus.h"
#include "Renderer/Component/CameraComponent.h"
#include "display/FonsFont.h"
#include "utils/FileUtils.h"
#include "base/AutoreleasePool.h"

namespace Module
{
    class PackageLoader
        : protected AZ::Data::AssetBus::MultiHandler
    {
    public:
        AZ_CLASS_ALLOCATOR(PackageLoader, AZ::SystemAllocator, 0);

        PackageLoader(AZStd::string name)
        {
            m_name = name;
            m_bytesAsset.Create(AZStd::string::format("%s.bytes", name.c_str()).c_str());
            m_spriteAsset.Create(AZStd::string::format("%s@sprites.bytes", name.c_str()).c_str());
        }

        void Load(AddPackageCallBack callback)
        {
            if (m_bytesAsset.GetId().IsValid())
            {
                AZ::Data::AssetBus::MultiHandler::BusConnect(m_bytesAsset.GetId());
                m_bytesAsset.QueueLoad();
            }
            if (m_spriteAsset.GetId().IsValid())
            {
                AZ::Data::AssetBus::MultiHandler::BusConnect(m_spriteAsset.GetId());
                m_spriteAsset.QueueLoad();
            }
            m_callback = callback;
        }
        
    protected:
        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override
        {
            if (m_bytesAsset.IsReady() && m_spriteAsset.IsReady())
            {
                UIPackage::addPackage(m_name);
                if(m_callback)
                {
                    m_callback();
                }
                AZ::Data::AssetBus::MultiHandler::BusDisconnect(m_bytesAsset.GetId());
                AZ::Data::AssetBus::MultiHandler::BusDisconnect(m_spriteAsset.GetId());

                delete this;
            }
        }

    private:
        AZ::Data::Asset<FairyAsset> m_bytesAsset;
        AZ::Data::Asset<FairyAsset> m_spriteAsset;

        AZStd::string m_name = "";
        AddPackageCallBack m_callback = nullptr;
    };

    void FairySystemComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<FairySystemComponent>()
                ->SerializerForEmptyClass();
        }
    }

    //--------------------------------------------------------------------------------------------------------
    FairySystemComponent::FairySystemComponent()
        : AZ::InputChannelEventListener(AZ::InputChannelEventListener::GetPriorityUI())
    {
        m_assetHandlers.emplace_back(aznew FairyAssetHandler);
    }

    void FairySystemComponent::Init()
    {
        
    }

    void FairySystemComponent::Activate()
    {
        AZ::SystemTickBus::Handler::BusConnect();
        AZ::InputChannelEventListener::BusConnect();
        FairySystemRequestBus::Handler::BusConnect();
        FairyPackageSystemRequestBus::Handler::BusConnect();
      
        WindowsSystemNotificationBus::Handler::BusConnect();
        // MeshProviderNotificationBus::Handler::BusConnect(GetEntityId());

        UIConfig::CreateInstance();
        Scheduler::CreateInstance();
        AutoreleasePool::CreateInstance();
        FileUtils::CreateInstance();
        UBBParser::CreateInstance();
        FonsFont::CreateInstance();

        this->InitFairyRoot();
    }

    void FairySystemComponent::Deactivate()
    {
        this->DestroyFairyRoot();

        FonsFont::DestroyInstance();
        UBBParser::DestroyInstance();
        FileUtils::DestroyInstance();
        AutoreleasePool::DestroyInstance();
        Scheduler::DestroyInstance();
        UIConfig::DestroyInstance();

        // MeshProviderNotificationBus::Handler::BusDisconnect();
        WindowsSystemNotificationBus::Handler::BusDisconnect();
     
        FairyPackageSystemRequestBus::Handler::BusDisconnect();
        FairySystemRequestBus::Handler::BusDisconnect();
        AZ::InputChannelEventListener::BusDisconnect();
        AZ::SystemTickBus::Handler::BusDisconnect();
    }

    void FairySystemComponent::OnSystemTick()
    {
        float time = 0;
        EBUS_EVENT_RESULT(time, AZ::TickRequestBus, GetTickDeltaTime);
        FGuiCentral::getInstance()->Update(time);
    }
    
    /*
    void FairySystemComponent::OnPreRender()
    {
        EBUS_EVENT_ID(m_rootPanel->GetId(), Module::FairyPanelRequestBus, ClearMaterialAndMesh);
        FGuiCentral::getInstance()->PreRender();
        if (m_rootPanel)
        {
            EBUS_EVENT_ID(m_rootPanel->GetId(), MeshProviderRequestBus, UpdateMeshBuffer);
        }
    }
    */
    /*
    void FairySystemComponent::OnRender()
    {
        FGuiCentral::getInstance()->Render();
    }
    */
//    void FairySystemComponent::OnPostRender()
//    {
//        
//    }
    /*
    void FairySystemComponent::SetMaterialAndMesh(AZStd::shared_ptr<Module::Material> material, int subMeshIndex)
    {
        if(m_rootPanel)
        {
            EBUS_EVENT_ID(m_rootPanel->GetId(), Module::FairyPanelRequestBus, SetMaterialAndMesh, material, subMeshIndex);
        }
    }

    void FairySystemComponent::UpdateMeshBuffer()
    {
        FGuiCentral::getInstance()->UpdateMeshBuffer();
    }

    void FairySystemComponent::ApplySubMesh(int index)
    {

        FGuiCentral::getInstance()->ApplySubMesh(index);
    }
    */
 

    //--------------------------------------------------------------------------------------------------------
    void FairySystemComponent::AddPackage(const AZStd::string& url, AddPackageCallBack callback)
    {
        if (UIPackage::getById(url))
        {
            callback();
            return;
        }

        PackageLoader* pl = aznew PackageLoader(url);
        if(pl)
        {
            pl->Load(callback);
        }
    }

    GRoot* FairySystemComponent::GetGRoot()
    {
        return UIRoot;
    }

    GObject* FairySystemComponent::CreateObject(const AZStd::string& pkgName, const AZStd::string& resName)
    {
        return UIPackage::createObject(pkgName, resName);
    }

    AZStd::string FairySystemComponent::GetPackageItemURL(const AZStd::string& pkgName, const AZStd::string& resName)
    {
        return UIPackage::getItemURL(pkgName, resName);
    }

    GObject* FairySystemComponent::CreateSpine(const AZStd::string& spineName)
    {
        GSpine* pSpine = GSpine::create();
        pSpine->Init(spineName);
        return pSpine;
    }

    GObject* FairySystemComponent::CreateParticle2d(const AZStd::string& particleName)
    {
        GParticle2d* pParticle = GParticle2d::create();
        pParticle->Init(particleName);
        return pParticle;
    }

    void FairySystemComponent::ChangeStageCameraPos(float posX, float posY)
    {
        AZ::Entity* entity = nullptr;
        EBUS_EVENT_RESULT(entity, AZ::GameEntityContextRequestBus, FindGameEntityByName, "stageCamera");
        if (entity != nullptr && entity->GetId().IsValid())
        {
            EBUS_EVENT_ID(entity->GetId(), AZ::TransformBus, SetWorldTranslation, AZ::Vector3(posX, posY, 0));
        }
       
    }

    float FairySystemComponent::GetPanelScaleValue()
    {
        return m_panelScale;
    }

    Ref* FairySystemComponent::CreateEmptyObject(const AZStd::string& typeName)
    {
        Ref* obj = nullptr;
        if (typeName == "object")
            obj = GObject::create();
        else if (typeName == "component")
            obj = GComponent::create();
        else if (typeName == "image")
            obj = GImage::create();
        else if (typeName == "movieclip")
            obj = GMovieClip::create();
        else if (typeName == "text")
            obj = GBasicTextField::create();
        else if (typeName == "richtext")
            obj = GRichTextField::create();
        else if (typeName == "inputtext")
            obj = GTextInput::create();
        else if (typeName == "group")
            obj = GGroup::create();
        else if (typeName == "list")
            obj = GList::create();
        else if (typeName == "graph")
            obj = GGraph::create();
        else if (typeName == "combobox")
            obj = GComboBox::create();
        else if (typeName == "button")
            obj = GButton::create();
        else if (typeName == "label")
            obj = GLabel::create();
        else if (typeName == "progressbar")
            obj = GProgressBar::create();
        else if (typeName == "slider")
            obj = GSlider::create();
        else if (typeName == "scrollbar")
            obj = GScrollBar::create();
        else if (typeName == "loader")
            obj = GLoader::create();
        else if (typeName == "window")
            obj = Window::create();
        else
            obj = GObject::create();

        return obj;
    }
    /*
    void FairySystemComponent::CreateRootPanel()
    {
        if (m_rootPanel)
        {
            return;
        }
        EBUS_EVENT_RESULT(m_rootPanel, AZ::GameEntityContextRequestBus, CreateGameEntity, "canvas");
        if (m_rootPanel)
        {
            m_rootPanel->CreateComponent(AZ::TransformComponent::CreateDescriptor()->GetUuid());
            m_rootPanel->CreateComponent(Module::FairyPanelComponent::CreateDescriptor()->GetUuid());
            m_rootPanel->CreateComponent(Module::RendererComponent::CreateDescriptor()->GetUuid());

            EBUS_EVENT(AZ::GameEntityContextRequestBus, ActivateGameEntity, m_rootPanel->GetId());
        }
    }
    */

    AZ::EntityId FairySystemComponent::CreateUIPanel(const AZStd::string& pkgName, const AZStd::string& componentName)
    {
        AZ::Entity* entity = nullptr;
        EBUS_EVENT_RESULT(entity, AZ::GameEntityContextRequestBus, CreateGameEntity, "uipanel");
        if (entity)
        {
            entity->CreateComponent(AZ::TransformComponent::CreateDescriptor()->GetUuid());
            entity->CreateComponent(Module::FairyPanelComponent::CreateDescriptor()->GetUuid());
            // entity->CreateComponent(Module::RendererComponent::CreateDescriptor()->GetUuid());
            EBUS_EVENT(AZ::GameEntityContextRequestBus, ActivateGameEntity, entity->GetId());
            if (pkgName.length() && componentName.length())
            {
                EBUS_EVENT_ID(entity->GetId(), Module::FairyPanelRequestBus, SetConfigData, pkgName, componentName);
            }   
            EBUS_EVENT_ID(entity->GetId(), AZ::TransformBus, SetWorldTranslation, AZ::Vector3(10000.0f, 10000.0f, 0.0f));
        }
        CreateStageCamera();
        return entity->GetId();
    }

    //--------------------------------------------------------------------------------------------------------
    void FairySystemComponent::InitFairyRoot()
    {
        FGuiCentral::CreateInstance();

        GRoot::create();

        AZ::ScriptContext* scPtr = nullptr;
        EBUS_EVENT_RESULT(scPtr, AZ::ScriptSystemRequestBus, GetContext);
        N3Lua::UseFairy(scPtr->NativeContext());

        int width, height;
        EBUS_EVENT(WindowSystemRequestBus, GetWindowSize, width, height);
        m_panelScale = 10.0f / (height * 0.01f);
        FGuiCentral::getInstance()->SetDefaultSize(width, height);
        // CreateStageCamera(width, height);
        m_defaultWidth = width;
        m_defaultHeight = height;


        return;

     /*
        this->AddPackage("Box2dUse", [this]()
        {
            GObject* obj = nullptr;
            EBUS_EVENT_RESULT(obj, FairyPackageSystemRequestBus, CreateObject, "Box2dUse", "Ground");
            if (obj)
            {
                UIRoot->addChild(obj);
            }
        });
       */
    
        //Test        


    

     

    
        
        GObject* obj = nullptr;
        EBUS_EVENT_RESULT(obj, FairyPackageSystemRequestBus, CreateSpine, "choujiangzhuanpan");
        if (obj)
        {
            UIRoot->addChild(obj);
            if(GSpine* spine = obj->SafeCast<GSpine>())
            {
//                auto id = spine->GetSpineId();
//                EBUS_EVENT_ID(id, Module::SpineRequestBus, SetAnimation, 0, "xunhuan", true);
            }
        }

//        return;
   
    }

    void FairySystemComponent::DestroyFairyRoot()
    {
        DestroyStageCamera();

        GRoot::destroy();

        FGuiCentral::DestroyInstance();
    }

    void FairySystemComponent::CreateStageCamera()
    {
        AZ::Entity* entity = nullptr;
        EBUS_EVENT_RESULT(entity, AZ::GameEntityContextRequestBus, FindGameEntityByName, "stageCamera");
        if (entity && entity->GetId().IsValid())
        {
            return;
        }
        EBUS_EVENT_RESULT(entity, AZ::GameEntityContextRequestBus, CreateGameEntity, "stageCamera");
        if (entity)
        {
            entity->CreateComponent(AZ::TransformComponent::CreateDescriptor()->GetUuid());
            entity->CreateComponent(Module::CameraComponent::CreateDescriptor()->GetUuid());
           
            EBUS_EVENT(AZ::GameEntityContextRequestBus, ActivateGameEntity, entity->GetId());
            EBUS_EVENT_ID(entity->GetId(), CameraRequestBus, SetClearFlags, (CameraClearFlags)3);
            EBUS_EVENT_ID(entity->GetId(), CameraRequestBus, SetOrthographicSize, 5);
            EBUS_EVENT_ID(entity->GetId(), CameraRequestBus, SetOrthographic, true);
            EBUS_EVENT_ID(entity->GetId(), CameraRequestBus, SetDepth, 10);
          
            EBUS_EVENT_ID(entity->GetId(), AZ::TransformBus, SetWorldTranslation, AZ::Vector3(m_defaultWidth * 0.005 * m_panelScale + 10000.0f, -m_defaultHeight * 0.005 * m_panelScale + 10000.0f, 0.0f));
        }
    }

    void FairySystemComponent::DestroyStageCamera()
    {
        AZ::Entity* entity = nullptr;
        EBUS_EVENT_RESULT(entity, AZ::GameEntityContextRequestBus, FindGameEntityByName, "stageCamera");
        if (entity!= nullptr && entity->GetId().IsValid())
        {
            EBUS_EVENT(AZ::GameEntityContextRequestBus, DestroyGameEntity, entity->GetId());
        }
    }

    //--------------------------------------------------------------------------------------------------------
    bool FairySystemComponent::OnInputChannelEventFiltered(const AZ::InputChannel& inputChannel)
    {
        if (AZ::InputDeviceMouse::IsMouseDevice(inputChannel.GetInputDevice().GetInputDeviceId()))
        {
            AZ::Vector2 viewportPos(0.0f, 0.0f);
            const AZ::InputChannel::PositionData2D* positionData2D = inputChannel.GetCustomData<AZ::InputChannel::PositionData2D>();
            if (positionData2D)
            {
                viewportPos.SetX(positionData2D->m_normalizedPosition.GetX() * RenderDataProcess::Width / RenderDataProcess::Zoom);
                viewportPos.SetY(positionData2D->m_normalizedPosition.GetY() * RenderDataProcess::Height / RenderDataProcess::Zoom);

                Touch touch;
                touch.setTouchInfo(0, viewportPos.GetX(), viewportPos.GetY());
                const AZ::InputChannel::Snapshot inputSnapshot(inputChannel);
                if (inputSnapshot.m_state == AZ::InputChannel::State::Began)
                {
                    if (inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::SystemCursorPosition
                        && inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::Movement::X
                        && inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::Movement::Y
                        && inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::Movement::Z
                        && positionData2D->m_normalizedPosition.GetX() >= 0
                        && positionData2D->m_normalizedPosition.GetX() <= 1
                        && positionData2D->m_normalizedPosition.GetY() >= 0
                        && positionData2D->m_normalizedPosition.GetY() <= 1)
                    {
                        UIRoot->getInputProcessor()->onTouchBegan(&touch);
                    }
                }

                if (inputChannel.GetInputChannelId() == AZ::InputDeviceMouse::SystemCursorPosition
                    && positionData2D->m_normalizedPosition.GetX() >= 0
                    && positionData2D->m_normalizedPosition.GetX() <= 1
                    && positionData2D->m_normalizedPosition.GetY() >= 0
                    && positionData2D->m_normalizedPosition.GetY() <= 1)
                {
                    UIRoot->getInputProcessor()->onTouchMoved(&touch);
                }

                if (inputSnapshot.m_state == AZ::InputChannel::State::Ended)
                {
                    if (inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::SystemCursorPosition
                        && inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::Movement::X
                        && inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::Movement::Y
                        && inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::Movement::Z)
                    {
                        UIRoot->getInputProcessor()->onTouchEnded(&touch);
                    }
                }
            }
        }
        else if (AZ::InputDeviceTouch::IsTouchDevice(inputChannel.GetInputDevice().GetInputDeviceId()))
        {
            AZ::Vector2 viewportPos(0.0f, 0.0f);
            const AZ::InputChannel::PositionData2D* positionData2D = inputChannel.GetCustomData<AZ::InputChannel::PositionData2D>();
            if (positionData2D)
            {
                viewportPos.SetX(positionData2D->m_normalizedPosition.GetX() * RenderDataProcess::Width / RenderDataProcess::Zoom);
                viewportPos.SetY(positionData2D->m_normalizedPosition.GetY() * RenderDataProcess::Height / RenderDataProcess::Zoom);

                Touch touch;
                touch.setTouchInfo(0, viewportPos.GetX(), viewportPos.GetY());

                const AZ::InputChannel::Snapshot inputSnapshot(inputChannel);
                if (inputSnapshot.m_state == AZ::InputChannel::State::Began)
                {
                    UIRoot->getInputProcessor()->onTouchBegan(&touch);
                }

                if (inputSnapshot.m_state == AZ::InputChannel::State::Updated)
                {
                    UIRoot->getInputProcessor()->onTouchMoved(&touch);
                }

                if (inputSnapshot.m_state == AZ::InputChannel::State::Ended)
                {
                    UIRoot->getInputProcessor()->onTouchEnded(&touch);
                }
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------------------------
    void FairySystemComponent::OnWindowSizeChanged(int width, int height)
    {
        if(FGuiCentral::HasInstance())
            FGuiCentral::getInstance()->ChangeViewSize(width, height);
    }
}

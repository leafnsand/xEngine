/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#ifndef AZ_UNITY_BUILD

#include <AzCore/PlatformIncl.h>

#include <AzCore/Casting/numeric_cast.h>

#include <AzCore/Component/ComponentApplication.h>
#include <AzCore/Component/TickBus.h>

#include <AzCore/Memory/AllocationRecords.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/ObjectStream.h>

#include <AzCore/RTTI/AttributeReader.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/RTTI/AzStdReflectionComponent.h>
#include <AzCore/IO/FileIO.h>
#include <AzCore/IO/SystemFile.h>

#include <AzCore/Asset/AssetManagerComponent.h>
#include <AzCore/Memory/MemoryComponent.h>
#include <AzCore/Jobs/JobManagerComponent.h>
#include <AzCore/Serialization/ObjectStreamComponent.h>
#include <AzCore/Script/ScriptSystemComponent.h>
#include <AzCore/Script/ScriptComponent.h>
#include <AzCore/Component/TransformComponent.h>
#include <AzCore/Component/GameEntityContextComponent.h>
#include <AzCore/Input/System/InputSystemComponent.h>
#include <AzCore/InputEvent/InputEventSystemComponent.h>
#include <AzCore/Slice/SliceMetadataInfoComponent.h>
#include <AzCore/Slice/SliceSystemComponent.h>

#include <AzCore/XML/rapidxml.h>

#if defined(AZ_ENABLE_DEBUG_TOOLS)
#include <AzCore/Debug/StackTracer.h>
#endif // defined(AZ_ENABLE_DEBUG_TOOLS)

#if defined(AZ_PLATFORM_APPLE)
#   include <mach-o/dyld.h>
#endif

namespace AZ
{
    //=========================================================================
    // ComponentApplication::Descriptor
    // [5/30/2012]
    //=========================================================================
    ComponentApplication::Descriptor::Descriptor()
    {
        m_useExistingAllocator = false;
        m_grabAllMemory = false;
        m_allocationRecords = true;
        m_allocationRecordsSaveNames = false;
        m_allocationRecordsAttemptDecodeImmediately = false;
        m_autoIntegrityCheck = false;
        m_markUnallocatedMemory = true;
        m_doNotUsePools = false;
        m_enableScriptReflection = true;

        m_pageSize = SystemAllocator::Descriptor::Heap::m_defaultPageSize;
        m_poolPageSize = SystemAllocator::Descriptor::Heap::m_defaultPoolPageSize;
        m_memoryBlockAlignment = SystemAllocator::Descriptor::Heap::m_memoryBlockAlignment;
        m_memoryBlocksByteSize = 0;
        m_reservedOS = 0;
        m_reservedDebug = 0;
        m_recordingMode = Debug::AllocationRecords::RECORD_STACK_IF_NO_FILE_LINE;
        m_stackRecordLevels = 5;
    }

    //=========================================================================
    // Reflect
    //=========================================================================
    void  ComponentApplication::Descriptor::Reflect(ReflectContext* context, ComponentApplication* app)
    {
        DynamicModuleDescriptor::Reflect(context);

        if (auto serializeContext = azrtti_cast<SerializeContext*>(context))
        {
            serializeContext->Class<Descriptor>(&app->GetDescriptor())
                ->Field("useExistingAllocator", &Descriptor::m_useExistingAllocator)
                ->Field("grabAllMemory", &Descriptor::m_grabAllMemory)
                ->Field("allocationRecords", &Descriptor::m_allocationRecords)
                ->Field("allocationRecordsSaveNames", &Descriptor::m_allocationRecordsSaveNames)
                ->Field("allocationRecordsAttemptDecodeImmediately", &Descriptor::m_allocationRecordsAttemptDecodeImmediately)
                ->Field("recordingMode", &Descriptor::m_recordingMode)
                ->Field("stackRecordLevels", &Descriptor::m_stackRecordLevels)
                ->Field("autoIntegrityCheck", &Descriptor::m_autoIntegrityCheck)
                ->Field("markUnallocatedMemory", &Descriptor::m_markUnallocatedMemory)
                ->Field("doNotUsePools", &Descriptor::m_doNotUsePools)
                ->Field("enableScriptReflection", &Descriptor::m_enableScriptReflection)
                ->Field("pageSize", &Descriptor::m_pageSize)
                ->Field("poolPageSize", &Descriptor::m_poolPageSize)
                ->Field("blockAlignment", &Descriptor::m_memoryBlockAlignment)
                ->Field("blockSize", &Descriptor::m_memoryBlocksByteSize)
                ->Field("reservedOS", &Descriptor::m_reservedOS)
                ->Field("reservedDebug", &Descriptor::m_reservedDebug)
                ->Field("modules", &Descriptor::m_modules)
                ;
        }
        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->EBus<ComponentApplicationBus>("ComponentApplicationBus")
                ->Event("GetExecutableFolder", &ComponentApplicationBus::Events::GetExecutableFolder);

            behaviorContext->Class<ComponentApplication::Descriptor>("ComponentApplication_Descriptor")
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
                ->Constructor()
                ->Property("m_useExistingAllocator", BehaviorValueProperty(&Descriptor::m_useExistingAllocator))
                ->Property("m_grabAllMemory", BehaviorValueProperty(&Descriptor::m_grabAllMemory))
                ->Property("m_allocationRecords", BehaviorValueProperty(&Descriptor::m_allocationRecords))
                ->Property("m_allocationRecordsSaveNames", BehaviorValueProperty(&Descriptor::m_allocationRecordsSaveNames))
                ->Property("m_allocationRecordsAttemptDecodeImmediately", BehaviorValueProperty(&Descriptor::m_allocationRecordsAttemptDecodeImmediately))
                ->Property("m_recordingMode", 
                    [](Descriptor* component) { return (int&)(component->m_recordingMode); },
                    [](Descriptor* component, const int& i) { component->m_recordingMode = (Debug::AllocationRecords::Mode)i; })
                ->Property("m_stackRecordLevels", BehaviorValueProperty(&Descriptor::m_stackRecordLevels))
                ->Property("m_autoIntegrityCheck", BehaviorValueProperty(&Descriptor::m_autoIntegrityCheck))
                ->Property("m_markUnallocatedMemory", BehaviorValueProperty(&Descriptor::m_markUnallocatedMemory))
                ->Property("m_doNotUsePools", BehaviorValueProperty(&Descriptor::m_doNotUsePools))
                ->Property("m_enableScriptReflection", BehaviorValueProperty(&Descriptor::m_enableScriptReflection))
                ->Property("m_pageSize", BehaviorValueProperty(&Descriptor::m_pageSize))
                ->Property("m_poolPageSize", BehaviorValueProperty(&Descriptor::m_poolPageSize))
                ->Property("m_memoryBlockAlignment", BehaviorValueProperty(&Descriptor::m_memoryBlockAlignment))
                ->Property("m_memoryBlocksByteSize", BehaviorValueProperty(&Descriptor::m_memoryBlocksByteSize))
                ->Property("m_reservedOS", BehaviorValueProperty(&Descriptor::m_reservedOS))
                ->Property("m_reservedDebug", BehaviorValueProperty(&Descriptor::m_reservedDebug))
                //TODO
                //->Property("m_modules", BehaviorValueProperty(&Descriptor::m_modules))
                ;
        }
    }

    //=========================================================================
    // Create
    //=========================================================================
    void* ComponentApplication::Descriptor::Create(const char* name)
    {
        (void)name;
        return this; /// we the the factory and the object as we are part of the component application
    }

    //=========================================================================
    // Destroy
    //=========================================================================
    void ComponentApplication::Descriptor::Destroy(void* data)
    {
        // do nothing as descriptor is part of the component application
        (void)data;
    }

    //=========================================================================
    // ComponentApplication
    // [5/30/2012]
    //=========================================================================
    ComponentApplication::ComponentApplication()
    {
        m_isStarted = false;
        m_isSystemAllocatorOwner = false;
        m_isOSAllocatorOwner = false;
        m_memoryBlock = nullptr;
        m_osAllocator = nullptr;
        m_serializeContext = nullptr;
        m_behaviorContext = nullptr;
        m_currentTime = AZStd::chrono::system_clock::time_point::max();

        m_deltaTime = 0.f;
        m_exeDirectory[0] = '\0';

        TickBus::AllowFunctionQueuing(true);
    }

    //=========================================================================
    // ~ComponentApplication
    // [5/30/2012]
    //=========================================================================
    ComponentApplication::~ComponentApplication()
    {
        if (m_isStarted)
        {
            Destroy();
        }
    }

    //=========================================================================
    // Create
    // [5/30/2012]
    //=========================================================================
    Entity* ComponentApplication::Create(const char* applicationDescriptorFile,
        const StartupParameters& startupParameters)
    {
        AZ_Assert(!m_isStarted, "Component descriptor already started!");

        m_startupParameters = startupParameters;

        if (!startupParameters.m_allocator)
        {
            if (!AZ::AllocatorInstance<AZ::OSAllocator>::IsReady())
            {
                AZ::AllocatorInstance<AZ::OSAllocator>::Create();
                m_isOSAllocatorOwner = true;
            }
            m_osAllocator = &AZ::AllocatorInstance<AZ::OSAllocator>::Get();
        }
        else
        {
            m_osAllocator = startupParameters.m_allocator;
        }

        // Make the path root-relative unless it's already a full path.
        AZ::OSString fullApplicationDescriptorPath = GetFullPathForDescriptor(applicationDescriptorFile);

        // Open the file
        AZ_Assert(!fullApplicationDescriptorPath.empty(), "Application descriptor filename cannot be NULL!");
        IO::SystemFile cfg;
        cfg.Open(fullApplicationDescriptorPath.c_str(), IO::SystemFile::SF_OPEN_READ_ONLY);
        size_t len = static_cast<size_t>(cfg.Length());
        AZ_Assert(len > 0, "Application descriptor file %s is invalid!", applicationDescriptorFile);
        (void)len;
        IO::SystemFileStream stream(&cfg, false);

        AZ::SystemAllocator::Descriptor desc;

        // Create temporary system allocator
        bool isExistingSystemAllocator = AZ::AllocatorInstance<AZ::SystemAllocator>::IsReady();

        if (!isExistingSystemAllocator)
        {
            enum
            {
                TemporaryBlockSize = 10 *  1024 * 1024
            };

            desc.m_heap.m_numMemoryBlocks = 1;
            desc.m_heap.m_memoryBlocksByteSize[0] = TemporaryBlockSize;
            desc.m_heap.m_memoryBlocks[0] = m_osAllocator->Allocate(desc.m_heap.m_memoryBlocksByteSize[0], desc.m_heap.m_memoryBlockAlignment);
            AZ::AllocatorInstance<AZ::SystemAllocator>::Create(desc);
        }

        // Get the system allocator configuration
        {
            SerializeContext sc;
            Descriptor::Reflect(&sc, this);
            ObjectStream::LoadBlocking(&stream, sc, nullptr, ObjectStream::FilterDescriptor(0, ObjectStream::FILTERFLAG_IGNORE_UNKNOWN_CLASSES));
        }

        // Destroy the temp system allocator
        if (!isExistingSystemAllocator)
        {
            AZ::AllocatorInstance<AZ::SystemAllocator>::Destroy();
            m_osAllocator->DeAllocate(desc.m_heap.m_memoryBlocks[0], desc.m_heap.m_memoryBlocksByteSize[0], desc.m_heap.m_memoryBlockAlignment);
        }
        else
        {
            AZ_Assert(m_descriptor.m_useExistingAllocator,
                "%s has useExistingAllocator set to false, but an existing system allocator was found! If you will create the system allocator yourself then set useExistingAllocator to true!",
                applicationDescriptorFile);
        }

        CreateCommon();

        // Create system entity. Reset the descriptor in preparation for reserializing it.
        m_descriptor = Descriptor();
        stream.Seek(0, IO::GenericStream::ST_SEEK_BEGIN);

        AZ::Entity* systemEntity = nullptr;
        ObjectStream::LoadBlocking(&stream, *m_serializeContext, [this, &systemEntity](void* classPtr, const AZ::Uuid& classId, const AZ::SerializeContext* sc) {
            if (ModuleEntity* moduleEntity = sc->Cast<ModuleEntity*>(classPtr, classId))
            {
                m_moduleManager->AddModuleEntity(moduleEntity);
            }
            else if (Entity* entity = sc->Cast<Entity*>(classPtr, classId))
            {
                if (entity->GetId() == AZ::SystemEntityId)
                {
                    systemEntity = entity;
                }
            }
            else if (!sc->Cast<Descriptor*>(classPtr, classId))
            {
                char idStr[Uuid::MaxStringBuffer];
                classId.ToString(idStr, AZ_ARRAY_SIZE(idStr));
                AZ_Error("ComponentApplication", false, "Unknown class type %p %s", classPtr, idStr);
            }
        });

        AZ_Assert(systemEntity, "SystemEntity failed to load!");
        cfg.Close();

        AddRequiredSystemComponents(systemEntity);
        m_isStarted = true;
        return systemEntity;
    }

    //=========================================================================
    // Create
    // [5/31/2012]
    //=========================================================================
    Entity* ComponentApplication::Create(const Descriptor& descriptor,
        const StartupParameters& startupParameters)
    {
        AZ_Assert(!m_isStarted, "Component application already started!");


        if (startupParameters.m_allocator == nullptr)
        {
            if (!AZ::AllocatorInstance<AZ::OSAllocator>::IsReady())
            {
                AZ::AllocatorInstance<AZ::OSAllocator>::Create();
                m_isOSAllocatorOwner = true;
            }
            m_osAllocator = &AZ::AllocatorInstance<AZ::OSAllocator>::Get();
        }
        else
        {
            m_osAllocator = startupParameters.m_allocator;
        }

        m_descriptor = descriptor;
        m_startupParameters = startupParameters;
        CreateCommon();

        AZ::Entity* systemEntity = aznew Entity(SystemEntityId, "SystemEntity");
        AddRequiredSystemComponents(systemEntity);
        m_isStarted = true;
        return systemEntity;
    }

    //=========================================================================
    // CreateCommon
    //=========================================================================
    void ComponentApplication::CreateCommon()
    {
        CalculateExecutablePath();

        CreateSystemAllocator();

        RegisterCoreComponents();

        CreateSerializeContext();
        Reflect(m_serializeContext);

        CreateBehaviorContext();
        Reflect(m_behaviorContext);

        ComponentApplicationBus::Handler::BusConnect();

        m_currentTime = AZStd::chrono::system_clock::now();
        TickRequestBus::Handler::BusConnect();

#if defined(AZ_ENABLE_DEBUG_TOOLS)
        // Prior to loading more modules, we make sure SymbolStorage
        // is listening for the loads so it can keep track of which
        // modules we may eventually need symbols for
        Debug::SymbolStorage::RegisterModuleListeners();
#endif // defined(AZ_ENABLE_DEBUG_TOOLS)

        PreModuleLoad();

        // Setup the modules list
        m_moduleManager = AZStd::make_unique<ModuleManager>();
        m_moduleManager->SetSystemComponentTags(m_startupParameters.m_systemComponentTags);
        // Load dynamic modules if appropriate for the platform
        if (m_startupParameters.m_loadDynamicModules)
        {
            AZ::ModuleManagerRequests::LoadModulesResult loadModuleOutcomes;
            ModuleManagerRequestBus::BroadcastResult(loadModuleOutcomes, &ModuleManagerRequests::LoadDynamicModules, m_descriptor.m_modules, ModuleInitializationSteps::RegisterComponentDescriptors, true);

#if defined(AZ_ENABLE_TRACING)
            for (const auto& loadModuleOutcome : loadModuleOutcomes)
            {
                AZ_Error("ComponentApplication", loadModuleOutcome.IsSuccess(), loadModuleOutcome.GetError().c_str());
            }
#endif
        }
        // Load static modules if provided by the platform
        if (m_startupParameters.m_createStaticModulesCallback)
        {
            ModuleManagerRequestBus::Broadcast(&ModuleManagerRequests::LoadStaticModules, m_startupParameters.m_createStaticModulesCallback, ModuleInitializationSteps::RegisterComponentDescriptors);
        }
    }

    //=========================================================================
    // Destroy
    // [5/30/2012]
    //=========================================================================
    void ComponentApplication::Destroy()
    {
        // Finish all queued work
        TickBus::ExecuteQueuedEvents();
        TickBus::AllowFunctionQueuing(false);

        // deactivate all entities
        Entity* systemEntity = nullptr;
        while (!m_entities.empty())
        {
            Entity* entity = m_entities.begin()->second;
            m_entities.erase(m_entities.begin());

            if (entity->GetId() == SystemEntityId)
            {
                systemEntity = entity;
            }
            else
            {
                delete entity;
            }
        }

        // Force full garbage collect after all game entities destroyed, but before modules are unloaded.
        // This is to ensure that all references to reflected classes/ebuses are cleaned up before the data is deleted.
        // This problem could also be solved by using ref-counting for reflected data.
        ScriptSystemRequestBus::Broadcast(&ScriptSystemRequests::GarbageCollect);

        // Uninit and unload any dynamic modules.
        m_moduleManager.reset();

        // deactivate all system components
        if (systemEntity)
        {
            if (systemEntity->GetState() == Entity::ES_ACTIVE)
            {
                systemEntity->Deactivate();
            }

            delete systemEntity;
        }

        m_entities.clear();
        m_entities.rehash(0); // force free all memory

        DestroyBehaviorContext();
        DestroySerializeContext();

        // delete all descriptors left for application clean up
        EBUS_EVENT(ComponentDescriptorBus, ReleaseDescriptor);

        // Disconnect from application and tick request buses
        ComponentApplicationBus::Handler::BusDisconnect();
        TickRequestBus::Handler::BusDisconnect();

        // Clear the descriptor to deallocate all strings (owned by ModuleDescriptor)
        m_descriptor = Descriptor();

        // kill the system allocator if we created it
        if (m_isSystemAllocatorOwner)
        {
            AZ::AllocatorInstance<AZ::SystemAllocator>::Destroy();

            {
                if (m_memoryBlock != nullptr)
                {
                    m_osAllocator->DeAllocate(m_memoryBlock);
                }
            }
            m_memoryBlock = nullptr;
            m_isSystemAllocatorOwner = false;
        }

        

        if (m_isOSAllocatorOwner)
        {
            AZ::AllocatorInstance<AZ::OSAllocator>::Destroy();
            m_isOSAllocatorOwner = false;
        }

        m_osAllocator = nullptr;

        m_isStarted = false;

#if defined(AZ_ENABLE_DEBUG_TOOLS)
        // Unregister module listeners after allocators are destroyed
        // so that symbol/stack trace information is available at shutdown
        Debug::SymbolStorage::UnregisterModuleListeners();
#endif // defined(AZ_ENABLE_DEBUG_TOOLS)
    }

    //=========================================================================
    // CreateSystemAllocator
    // [5/30/2012]
    //=========================================================================
    void ComponentApplication::CreateSystemAllocator()
    {
        if (m_descriptor.m_useExistingAllocator)
        {
            m_isSystemAllocatorOwner = false;
            AZ_Assert(AZ::AllocatorInstance<AZ::SystemAllocator>::IsReady(), "You must setup AZ::SystemAllocator instance, before you can call Create application with m_useExistingAllocator flag set to true");
        }
        else
        {
            // Create the system allocator
            AZ::SystemAllocator::Descriptor desc;
            desc.m_heap.m_pageSize = m_descriptor.m_pageSize;
            desc.m_heap.m_poolPageSize = m_descriptor.m_poolPageSize;
            desc.m_heap.m_numMemoryBlocks = 1;
            if (m_descriptor.m_grabAllMemory)
            {
                // grab all available memory
                AZ::u64 availableOS = AZ_CORE_MAX_ALLOCATOR_SIZE;
                AZ::u64 reservedOS = m_descriptor.m_reservedOS;
                AZ::u64 reservedDbg = m_descriptor.m_reservedDebug;
                AZ_Warning("Memory", false, "This platform is not supported for grabAllMemory flag! Provide a valid allocation size and set the m_grabAllMemory flag to false! Using default max memory size %llu!", availableOS);
                AZ_Assert(availableOS > 0, "OS doesn't have any memory available!");
                // compute total memory to grab
                desc.m_heap.m_memoryBlocksByteSize[0] = static_cast<size_t>(availableOS - reservedOS - reservedDbg);
                // memory block MUST be a multiple of pages
                desc.m_heap.m_memoryBlocksByteSize[0] = AZ::SizeAlignDown(desc.m_heap.m_memoryBlocksByteSize[0], m_descriptor.m_pageSize);
            }
            else
            {
                desc.m_heap.m_memoryBlocksByteSize[0] = static_cast<size_t>(m_descriptor.m_memoryBlocksByteSize);
            }

            {
                if (desc.m_heap.m_memoryBlocksByteSize[0] > 0) // 0 means one demand memory which we support
                {
                    m_memoryBlock = m_osAllocator->Allocate(desc.m_heap.m_memoryBlocksByteSize[0], m_descriptor.m_memoryBlockAlignment);
                }
            }

            desc.m_heap.m_memoryBlocks[0] = m_memoryBlock;
            desc.m_allocationRecords = m_descriptor.m_allocationRecords;
            desc.m_stackRecordLevels = aznumeric_caster(m_descriptor.m_stackRecordLevels);
            AZ::AllocatorInstance<AZ::SystemAllocator>::Create(desc);
            AZ::Debug::AllocationRecords* records = AllocatorInstance<SystemAllocator>::Get().GetRecords();
            if (records)
            {
                records->SetMode(m_descriptor.m_recordingMode);
                records->SetSaveNames(m_descriptor.m_allocationRecordsSaveNames);
                records->SetDecodeImmediately(m_descriptor.m_allocationRecordsAttemptDecodeImmediately);
                records->AutoIntegrityCheck(m_descriptor.m_autoIntegrityCheck);
                records->MarkUallocatedMemory(m_descriptor.m_markUnallocatedMemory);
            }

            m_isSystemAllocatorOwner = true;
        }
    }

    //=========================================================================
    // RegisterComponentDescriptor
    // [5/30/2012]
    //=========================================================================
    void ComponentApplication::RegisterComponentDescriptor(const ComponentDescriptor* descriptor)
    {
        // reflect the factory if possible
        descriptor->Reflect(m_serializeContext);

        // reflect into behaviorContext
        if (m_behaviorContext)
        {
            descriptor->Reflect(m_behaviorContext);
        }
    }

    //=========================================================================
    // RegisterComponentDescriptor
    //=========================================================================
    void ComponentApplication::UnregisterComponentDescriptor(const ComponentDescriptor* descriptor)
    {
        if (m_behaviorContext)
        {
            m_behaviorContext->EnableRemoveReflection();
            descriptor->Reflect(m_behaviorContext);
            m_behaviorContext->DisableRemoveReflection();
        }

        if (m_serializeContext)
        {
            // \todo: Unreflect from script context: https://issues.labcollab.net/browse/LMBR-17558.

            // Remove all reflected data from this descriptor
            m_serializeContext->EnableRemoveReflection();
            descriptor->Reflect(m_serializeContext);
            m_serializeContext->DisableRemoveReflection();
        }
    }

    //=========================================================================
    // AddEntity
    // [5/30/2012]
    //=========================================================================
    bool ComponentApplication::AddEntity(Entity* entity)
    {
        EBUS_EVENT(ComponentApplicationEventBus, OnEntityAdded, entity);

        return m_entities.insert(AZStd::make_pair(entity->GetId(), entity)).second;
    }

    //=========================================================================
    // RemoveEntity
    // [5/31/2012]
    //=========================================================================
    bool ComponentApplication::RemoveEntity(Entity* entity)
    {
        EBUS_EVENT(ComponentApplicationEventBus, OnEntityRemoved, entity->GetId());

        return (m_entities.erase(entity->GetId()) == 1);
    }

    //=========================================================================
    // DeleteEntity
    // [5/31/2012]
    //=========================================================================
    bool ComponentApplication::DeleteEntity(const EntityId& id)
    {
        Entity* entity = FindEntity(id);
        if (entity)
        {
            delete entity;
            return true;
        }
        return false;
    }

    //=========================================================================
    // FindEntity
    // [5/30/2012]
    //=========================================================================
    Entity* ComponentApplication::FindEntity(const EntityId& id)
    {
        EntitySetType::const_iterator it = m_entities.find(id);
        if (it != m_entities.end())
        {
            return it->second;
        }
        return nullptr;
    }

    //=========================================================================
    // GetEntityName
    // [10/17/2016]
    //=========================================================================
    AZStd::string ComponentApplication::GetEntityName(const EntityId& id)
    {
        Entity* entity = FindEntity(id);
        if (entity)
        {
            return entity->GetName();
        }
        return AZStd::string();
    }

    //=========================================================================
    // EnumerateEntities
    //=========================================================================
    void ComponentApplication::EnumerateEntities(const ComponentApplicationRequests::EntityCallback& callback)
    {
        for (const auto& entityIter : m_entities)
        {
            callback(entityIter.second);
        }
    }

    //=========================================================================
    // CreateSerializeContext
    // [5/30/2012]
    //=========================================================================
    void ComponentApplication::CreateSerializeContext()
    {
        if (m_serializeContext == nullptr)
        {
            m_serializeContext = aznew SerializeContext;
        }
    }

    //=========================================================================
    // DestroySerializeContext
    // [11/9/2012]
    //=========================================================================
    void ComponentApplication::DestroySerializeContext()
    {
        delete m_serializeContext;
        m_serializeContext = nullptr;
    }

    //=========================================================================
    // CreateBehaviorContext
    //=========================================================================
    void ComponentApplication::CreateBehaviorContext()
    {
        if (m_behaviorContext == nullptr)
        {
            m_behaviorContext = aznew BehaviorContext;
        }
    }

    //=========================================================================
    // DestroyBehaviorContext
    //=========================================================================
    void ComponentApplication::DestroyBehaviorContext()
    {
        delete m_behaviorContext;
        m_behaviorContext = nullptr;
    }

    //=========================================================================
    // Tick
    //=========================================================================
    void ComponentApplication::Tick(float deltaOverride /*= -1.f*/)
    {
#if !defined(AZ_PLATFORM_EMSCRIPTEN)
        static const AZStd::chrono::milliseconds maxFrameTime(1000.0f / 60.0f);
#endif
        AZStd::chrono::system_clock::time_point now = AZStd::chrono::system_clock::now();

        m_deltaTime = 0.0f;

        if (now >= m_currentTime)
        {
            AZStd::chrono::duration<float> delta = now - m_currentTime;
            m_deltaTime = deltaOverride >= 0.f ? deltaOverride : delta.count();
        }

        {
            TickSystem();
            TickBus::ExecuteQueuedEvents();
            EBUS_EVENT(TickBus, OnTick, m_deltaTime, ScriptTimePoint(now));
        }

#if !defined(AZ_PLATFORM_EMSCRIPTEN)
        const auto duration = AZStd::chrono::system_clock::now() - now;
        if (duration < maxFrameTime)
        {
            AZStd::this_thread::sleep_for(AZStd::chrono::milliseconds(maxFrameTime - duration));
        }
#endif

        m_currentTime = now;
    }

    //=========================================================================
    // Tick
    //=========================================================================
    void ComponentApplication::TickSystem()
    {
        SystemTickBus::ExecuteQueuedEvents();
        EBUS_EVENT(SystemTickBus, OnSystemTick);
    }

    //=========================================================================
    // WriteApplicationDescriptor
    // [5/31/2012]
    //=========================================================================
    bool ComponentApplication::WriteApplicationDescriptor(const char* fileName)
    {
        Entity* systemEntity = FindEntity(SystemEntityId);
        if (systemEntity == nullptr)
        {
            return false;
        }

        AZStd::string tmpFileName(fileName);
        tmpFileName += ".tmp";

        AZStd::unique_ptr<IO::FileIOStream> fileStream(AZStd::make_unique<IO::FileIOStream>(tmpFileName.c_str(), IO::OpenMode::ModeWrite));
        if (fileStream == nullptr)
        {
            return false;
        }

        AZ_Assert(m_serializeContext, "ComponentApplication::m_serializeContext is NULL!");
        ObjectStream* objStream = ObjectStream::Create(fileStream.get(), *m_serializeContext, ObjectStream::ST_XML);
        bool descWriteOk = objStream->WriteClass(&m_descriptor);
        AZ_Warning("ComponentApplication", descWriteOk, "Failed to write memory descriptor to application descriptor file %s!", fileName);
        bool entityWriteOk = objStream->WriteClass(systemEntity);
        AZ_Warning("ComponentApplication", entityWriteOk, "Failed to write system entity to application descriptor file %s!", fileName);
        bool flushOk = objStream->Finalize();
        AZ_Warning("ComponentApplication", flushOk, "Failed finalizing application descriptor file %s!", fileName);
        fileStream->Close();

        if (descWriteOk && entityWriteOk && flushOk)
        {
            if (IO::SystemFile::Rename(tmpFileName.c_str(), fileName, true))
            {
                return true;
            }
            AZ_Warning("ComponentApplication", false, "Failed to rename %s to %s.", tmpFileName.c_str(), fileName);
        }
        return false;
    }

    //=========================================================================
    // RegisterCoreComponents
    // [5/30/2012]
    //=========================================================================
    void ComponentApplication::RegisterCoreComponents()
    {
        RegisterComponentDescriptor(AzStdReflectionComponent::CreateDescriptor());
        RegisterComponentDescriptor(MemoryComponent::CreateDescriptor());
        RegisterComponentDescriptor(JobManagerComponent::CreateDescriptor());
        RegisterComponentDescriptor(AssetManagerComponent::CreateDescriptor());
        RegisterComponentDescriptor(ObjectStreamComponent::CreateDescriptor());
        RegisterComponentDescriptor(TransformComponent::CreateDescriptor());
        RegisterComponentDescriptor(GameEntityContextComponent::CreateDescriptor());
        RegisterComponentDescriptor(InputSystemComponent::CreateDescriptor());
        RegisterComponentDescriptor(InputEventSystemComponent::CreateDescriptor());
        RegisterComponentDescriptor(SliceComponent::CreateDescriptor());
        RegisterComponentDescriptor(SliceSystemComponent::CreateDescriptor());
        RegisterComponentDescriptor(SliceMetadataInfoComponent::CreateDescriptor());


#if !defined(AZCORE_EXCLUDE_LUA)
        RegisterComponentDescriptor(ScriptSystemComponent::CreateDescriptor());
        RegisterComponentDescriptor(ScriptComponent::CreateDescriptor());
#endif // #if !defined(AZCORE_EXCLUDE_LUA)

    }

    //=========================================================================
    // GetRequiredSystemComponents
    //=========================================================================
    ComponentTypeList ComponentApplication::GetRequiredSystemComponents() const
    {
        return ComponentTypeList();
    }

    bool ComponentApplication::ShouldAddSystemComponent(AZ::ComponentDescriptor* descriptor)
    {
        // NOTE: This is different than modules! All system components must be listed in GetRequiredSystemComponents, and then AZ::Edit::Attributes::SystemComponentTags may be used to filter down from there

        // If the required tags are empty, it is assumed all components with the attribute are required
        if (m_startupParameters.m_systemComponentTags.empty())
        {
            return true;
        }

        const SerializeContext::ClassData* classData = m_serializeContext->FindClassData(descriptor->GetUuid());
        AZ_Warning("ComponentApplication", classData, "Component type %s not reflected to SerializeContext!", descriptor->GetName());
        if (classData)
        {
            if (Attribute* attribute = FindAttribute(Edit::Attributes::SystemComponentTags, classData->m_attributes))
            {
                // Read the tags
                AZStd::vector<AZ::Crc32> tags;
                AZ::AttributeReader reader(nullptr, attribute);
                AZ::Crc32 tag;
                if (reader.Read<AZ::Crc32>(tag))
                {
                    tags.emplace_back(tag);
                }
                else
                {
                    AZ_Verify(reader.Read<AZStd::vector<AZ::Crc32>>(tags), "Attribute \"AZ::Edit::Attributes::SystemComponentTags\" must be of type AZ::Crc32 or AZStd::vector<AZ::Crc32>");
                }

                // Match tags to required tags
                for (const AZ::Crc32& requiredTag : m_startupParameters.m_systemComponentTags)
                {
                    if (AZStd::find(tags.begin(), tags.end(), requiredTag) != tags.end())
                    {
                        // Tag match, add component
                        return true;
                    }
                }

                // If the tags didn't match, don't use the component
                return false;
            }
        }
        // If attribute not present, assume wanted for all contexts
        return true;
    }

    //=========================================================================
    // AddRequiredSystemComponents
    //=========================================================================
    void ComponentApplication::AddRequiredSystemComponents(AZ::Entity* systemEntity)
    {
        //
        // Gather required system components from all modules and the application.
        //
        for (const Uuid& componentId : GetRequiredSystemComponents())
        {
            ComponentDescriptor* componentDescriptor = nullptr;
            EBUS_EVENT_ID_RESULT(componentDescriptor, componentId, ComponentDescriptorBus, GetDescriptor);
            if (!componentDescriptor)
            {
                AZ_Error("Module", false, "Failed to add system component required by application. No component descriptor found for: %s",
                    componentId.ToString<AZStd::string>().c_str());
                continue;
            }

            if (ShouldAddSystemComponent(componentDescriptor))
            {
                // add component if it's not already present
                if (!systemEntity->FindComponent(componentId))
                {
                    systemEntity->AddComponent(componentDescriptor->CreateComponent());
                }
            }
        }

        //
        // Call legacy AddSystemComponents functions.
        // Issue warnings if they're still being used.
        //

        size_t componentCount = systemEntity->GetComponents().size();
        (void)componentCount; // Only used in Warnings, produces "Unused variable" warning in non-trace builds

        componentCount = systemEntity->GetComponents().size();
    }

    //=========================================================================
    // CalculateExecutablePath
    //=========================================================================
    void ComponentApplication::CalculateExecutablePath()
    {
        // Checks if exe directory has already been calculated
        if (strlen(m_exeDirectory) > 0)
        {
            return;
        }

#if   defined(AZ_PLATFORM_ANDROID) || defined(AZ_PLATFORM_EMSCRIPTEN)
        // On Android, all dlopen calls should be relative.
        azstrcpy(m_exeDirectory, AZ_ARRAY_SIZE(m_exeDirectory), "");
#else

        char exeDirectory[AZ_MAX_PATH_LEN];

        // Platform specific get exe path: http://stackoverflow.com/a/1024937
#if defined(AZ_PLATFORM_WINDOWS) || defined(AZ_PLATFORM_XBONE)
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms683197(v=vs.85).aspx
        DWORD pathLen = GetModuleFileNameA(nullptr, exeDirectory, AZ_ARRAY_SIZE(exeDirectory));
#elif defined(AZ_PLATFORM_APPLE)
        // https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/dyld.3.html
        uint32_t bufSize = AZ_ARRAY_SIZE(exeDirectory);
        _NSGetExecutablePath(exeDirectory, &bufSize);
        // _NSGetExecutablePath doesn't update bufSize
        size_t pathLen = strlen(exeDirectory);
#else
        // http://man7.org/linux/man-pages/man5/proc.5.html
        size_t pathLen = readlink("/proc/self/exe", exeDirectory, AZ_ARRAY_SIZE(exeDirectory));
#endif // MSVC

        char* exeDirEnd = exeDirectory + pathLen;

        AZStd::replace(exeDirectory, exeDirEnd, '\\', '/');

        // exeDirectory currently contains full path to EXE.
        // Modify to end the string after the last '/'

        // Iterate until finding last '/'
        char* finalSlash = nullptr;
        for (char* c = exeDirectory; c < exeDirEnd; ++c)
        {
            if (*c == '/')
            {
                finalSlash = c;
            }
        }

        // If no slashes found, path invalid.
        if (finalSlash)
        {
            *(finalSlash + 1) = '\0';
        }
        else
        {
            AZ_Error("ComponentApplication", false, "Failed to process exe directory. Path given by OS: %s", exeDirectory);
        }

        azstrcpy(m_exeDirectory, AZ_ARRAY_SIZE(m_exeDirectory), exeDirectory);
#endif
    }

    //=========================================================================
    // GetFullPathForDescriptor
    //=========================================================================
    AZ::OSString ComponentApplication::GetFullPathForDescriptor(const char* descriptorPath)
    {
        AZ::OSString fullApplicationDescriptorPath = descriptorPath;

        // We don't want to do lowercase here because it will breaks case-sensitive
        // platforms when descriptorPath is already a full path, and descriptorPath
        // has already been lower-cased in GameApplication::GetGameDescriptorPath.
        NormalizePath(fullApplicationDescriptorPath.begin(), fullApplicationDescriptorPath.end(), false);

        bool isFullPath = false;

#if defined (AZ_PLATFORM_X360) || defined (AZ_PLATFORM_WINDOWS) || defined (AZ_PLATFORM_XBONE) // ACCEPTED_USE
        if (nullptr != strstr(fullApplicationDescriptorPath.c_str(), ":"))
        {
            isFullPath = true;
        }
#else
        isFullPath = fullApplicationDescriptorPath[0] == '/';
#endif

        if (!isFullPath)
        {
            fullApplicationDescriptorPath.insert(0, GetAppRoot());
        }

        return fullApplicationDescriptorPath;
    }

    void ComponentApplication::ResolveModulePath(AZ::OSString& modulePath)
    {
        modulePath = m_exeDirectory + modulePath;
    }

    //=========================================================================
    // GetFrameTime
    // [1/22/2016]
    //=========================================================================
    float ComponentApplication::GetTickDeltaTime()
    {
        return m_deltaTime;
    }

    //=========================================================================
    // GetTime
    // [1/22/2016]
    //=========================================================================
    ScriptTimePoint ComponentApplication::GetTimeAtCurrentTick()
    {
        return ScriptTimePoint(m_currentTime);
    }

    //=========================================================================
    // Reflect
    //=========================================================================
    void ComponentApplication::Reflect(ReflectContext* context)
    {
        if (BehaviorContext* behaviorContext = azrtti_cast<BehaviorContext*>(context))
        {
            behaviorContext->Method<>("GetType", &ComponentApplication::GetTypeIdByName);
            behaviorContext->Method<>("SaveDescriptorConfig", &ComponentApplication::SaveDescriptorConfig);
        }

        // reflect default entity
        Entity::Reflect(context);
        // reflect module manager
        ModuleManager::Reflect(context);
        // reflect descriptor
        Descriptor::Reflect(context, this);

        // reflect all registered classes
        EBUS_EVENT(ComponentDescriptorBus, Reflect, context);
    }

    AZ::Uuid ComponentApplication::GetTypeIdByName(const char* name)
    {
        SerializeContext* context = nullptr;
        EBUS_EVENT_RESULT(context, ComponentApplicationBus, GetSerializeContext);
        if (context)
        {
            auto uuids = context->FindClassId(AZ::Crc32(name));
            if (!uuids.empty())
            {
                return uuids.front();
            }
        }
        return AZ::Uuid::CreateNull();
    }

    bool ComponentApplication::SaveDescriptorConfig(const char* path)
    {
        ComponentApplication* app = nullptr;
        EBUS_EVENT_RESULT(app, ComponentApplicationBus, GetApplication);
        if(app)
        {
            return app->WriteApplicationDescriptor(path);
        }
        return false;
    }
} // namespace AZ

#endif // #ifndef AZ_UNITY_BUILD

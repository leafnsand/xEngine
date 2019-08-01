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

#include <AzCore/IO/LocalFileIO.h>
#include <AzCore/IO/SystemFile.h>
#include <AzCore/Math/Crc.h>
#include <AzCore/Component/ComponentApplication.h>

#include <AzCore/Memory/MemoryComponent.h>
#include <AzCore/Jobs/JobManagerComponent.h>
#include <AzCore/Asset/AssetManagerComponent.h>
#include <AzCore/Component/GameEntityContextComponent.h>
#include <AzCore/Input/System/InputSystemComponent.h>
#include <AzCore/InputEvent/InputEventSystemComponent.h>
#include <AzCore/Script/ScriptSystemComponent.h>
#include <AzCore/Slice/SliceSystemComponent.h>
#include <AzCore/Serialization/ObjectStream.h>
#include <AzCore/Serialization/Util.h>

#include <AzCore/Outcome/Outcome.h>

#include <AzCore/FileFunc/FileFunc.h>
#include <AzCore/StringFunc/StringFunc.h>

#include "Application.h"
#include <cctype>
#include <stdio.h>

static const char* s_azFrameworkWarningWindow = "AzFramework";

static const char* s_engineConfigFileName = "game.cfg";
static const char* s_engineConfigExternalEnginePathKey = "ExternalEnginePath";
static const char* s_engineConfigExternalAssetsPathKey = "ExternalAssetsPath";
static const char* s_engineConfigExternalCdnPathKey = "ExternalCdnPath";
static const char* s_engineConfigDefaultExternalCdnPath = "http://localhost/";

#if defined(AZ_PLATFORM_ANDROID) || defined(AZ_PLATFORM_APPLE)
    #include <unistd.h>
#endif //AZ_PLATFORM_ANDROID  ||  AZ_PLATFORM_APPLE

namespace AZ
{
    namespace ApplicationInternal
    {
        // this is used when no argV/ArgC is supplied.
        // in order to have the same memory semantics (writable, non-const)
        // we create a buffer that can be written to (up to AZ_MAX_PATH_LEN) and then
        // pack it with a single param.
        static char  s_commandLineBuffer[AZ_MAX_PATH_LEN] = "no_argv_supplied";
        static char* s_commandLineAsCharPtr = s_commandLineBuffer;
        static char** s_argVUninitialized = &s_commandLineAsCharPtr; // non const, so it is compatible with char***
        static int s_argCUninitialized = 1;

        // A Helper function that can load an app descriptor from file.
        AZ::Outcome<AZStd::unique_ptr<AZ::ComponentApplication::Descriptor>, AZStd::string> LoadDescriptorFromFilePath(const char* appDescriptorFilePath, AZ::SerializeContext& serializeContext)
        {
            AZStd::unique_ptr<AZ::ComponentApplication::Descriptor> loadedDescriptor;

            AZ::IO::SystemFile appDescriptorFile;
            if (!appDescriptorFile.Open(appDescriptorFilePath, AZ::IO::SystemFile::SF_OPEN_READ_ONLY))
            {
                return AZ::Failure(AZStd::string::format("Failed to open file: %s", appDescriptorFilePath));
            }

            AZ::IO::SystemFileStream appDescriptorFileStream(&appDescriptorFile, true);
            if (!appDescriptorFileStream.IsOpen())
            {
                return AZ::Failure(AZStd::string::format("Failed to stream file: %s", appDescriptorFilePath));
            }

            // Callback function for allocating the root elements in the file.
            AZ::ObjectStream::InplaceLoadRootInfoCB inplaceLoadCb =
                [](void** rootAddress, const AZ::SerializeContext::ClassData**, const AZ::Uuid& classId, AZ::SerializeContext*)
            {
                if (rootAddress && classId == azrtti_typeid<AZ::ComponentApplication::Descriptor>())
                {
                    // ComponentApplication::Descriptor is normally a singleton.
                    // Force a unique instance to be created.
                    *rootAddress = aznew AZ::ComponentApplication::Descriptor();
                }
            };

            // Callback function for saving the root elements in the file.
            AZ::ObjectStream::ClassReadyCB classReadyCb =
                [&loadedDescriptor](void* classPtr, const AZ::Uuid& classId, AZ::SerializeContext* context)
            {
                // Save descriptor, delete anything else loaded from file.
                if (classId == azrtti_typeid<AZ::ComponentApplication::Descriptor>())
                {
                    loadedDescriptor.reset(static_cast<AZ::ComponentApplication::Descriptor*>(classPtr));
                }
                else if (const AZ::SerializeContext::ClassData* classData = context->FindClassData(classId))
                {
                    classData->m_factory->Destroy(classPtr);
                }
                else
                {
                    AZ_Error("Application", false, "Unexpected type %s found in application descriptor file. This memory will leak.",
                        classId.ToString<AZStd::string>().c_str());
                }
            };

            // There's other stuff in the file we may not recognize (system components), but we're not interested in that stuff.
            AZ::ObjectStream::FilterDescriptor loadFilter(nullptr, AZ::ObjectStream::FILTERFLAG_IGNORE_UNKNOWN_CLASSES);

            if (!AZ::ObjectStream::LoadBlocking(&appDescriptorFileStream, serializeContext, classReadyCb, loadFilter, inplaceLoadCb))
            {
                return AZ::Failure(AZStd::string::format("Failed to load objects from file: %s", appDescriptorFilePath));
            }

            if (!loadedDescriptor)
            {
                return AZ::Failure(AZStd::string::format("Failed to find descriptor object in file: %s", appDescriptorFilePath));
            }

            return AZ::Success(AZStd::move(loadedDescriptor));
        }
    }
    
    Application::Application()
    : Application(nullptr, nullptr)
    {
    }
    
    Application::Application(int* argc, char*** argv)
        : m_appRootInitialized(false)
    {
        m_appRoot[0] = '\0';
        m_assetRoot[0] = '\0';
        m_engineRoot[0] = '\0';
        m_cdnRoot[0] = '\0';

        if ((argc) && (argv))
        {
            m_argC = argc;
            m_argV = argv;
        }
        else
        {
            // use a "valid" value here.  This is becuase Qt and potentially other third party libraries require
            // that ArgC be 'at least 1' and that (*argV)[0] be a valid pointer to a real null terminated string.
            m_argC = &ApplicationInternal::s_argCUninitialized;
            m_argV = &ApplicationInternal::s_argVUninitialized;
        }

        ApplicationRequests::Bus::Handler::BusConnect();
    }

    int* Application::GetArgC() const
    {
        return m_argC;
    }

    char*** Application::GetArgV() const
    {
        return m_argV;
    }

    Application::~Application()
    {
        if (m_isStarted)
        {
            Stop();
        }

        ApplicationRequests::Bus::Handler::BusDisconnect();
    }

    void Application::ResolveModulePath(AZ::OSString& modulePath)
    {
        const char* modulePathStr = modulePath.c_str();
        if (AZ::StringFunc::Path::IsRelative(modulePathStr))
        {
            const char* searchPaths[] = { m_appRoot , m_engineRoot };
            for (const char* searchPath : searchPaths)
            {
                AZStd::string testPath = AZStd::string::format("%s" AZ_CORRECT_FILESYSTEM_SEPARATOR_STRING AZ_DYNAMIC_LIBRARY_PREFIX "%s", searchPath, modulePathStr);
                AZStd::string testPathWithPrefixAndExt = AZStd::string::format("%s" AZ_DYNAMIC_LIBRARY_EXTENSION, testPath.c_str(), modulePathStr);
                if (AZ::IO::SystemFile::Exists(testPathWithPrefixAndExt.c_str()))
                {
                    modulePath = testPath.c_str();
                    return;
                }
            }
            ComponentApplication::ResolveModulePath(modulePath);
        }
    }

    void Application::Start(const Descriptor& descriptor, const StartupParameters& startupParameters /*= StartupParameters()*/)
    {
        CalculateAppRoot(startupParameters.m_appRootOverride);
        AZ::Entity* systemEntity = Create(descriptor, startupParameters);
        StartCommon(systemEntity);
    }

    void Application::Start(const char* applicationDescriptorFile, const StartupParameters& startupParameters /*= StartupParameters()*/)
    {
        if (!AZ::AllocatorInstance<AZ::OSAllocator>::IsReady())
        {
            AZ::AllocatorInstance<AZ::OSAllocator>::Create();
            m_isOSAllocatorOwner = true;
        }

        CalculateAppRoot(startupParameters.m_appRootOverride);

        AZ::OSString descriptorFile = GetFullPathForDescriptor(applicationDescriptorFile);
        azstrcpy(m_configFilePath, AZ_MAX_PATH_LEN, descriptorFile.c_str());

        AZ::Entity* systemEntity = nullptr;

        bool descriptorFound = true;
        if (!AZ::IO::SystemFile::Exists(m_configFilePath))
        {
            descriptorFound = false;

            const bool continueStartup = OnFailedToFindConfiguration(m_configFilePath);
            if (!continueStartup)
            {
                return;
            }
        }

        if (descriptorFound)
        {
            // Pass applicationDescriptorFile instead of m_configFilePath because Create() expects AppRoot-relative path.
            systemEntity = Create(applicationDescriptorFile, startupParameters);
        }
        else
        {
            systemEntity = Create(Descriptor(), startupParameters);
        }
        StartCommon(systemEntity);
    }

    bool Application::OnFailedToFindConfiguration(const char* configFilePath)
    {
        AZ::Debug::Trace::Error(__FILE__, __LINE__, AZ_FUNCTION_SIGNATURE, "Application", "Failed to find app descriptor file \"%s\".", configFilePath);
        return true;
    }

    bool Application::ReflectModulesFromAppDescriptor(const char* appDescriptorFilePath)
    {
        auto loadDescriptorOutcome = ApplicationInternal::LoadDescriptorFromFilePath(appDescriptorFilePath, *GetSerializeContext());
        if (!loadDescriptorOutcome.IsSuccess())
        {
            AZ_Error("Application", false, loadDescriptorOutcome.GetError().c_str());
            return false;
        }

        AZ::ComponentApplication::Descriptor& descriptor = *loadDescriptorOutcome.GetValue();

        AZ::ModuleManagerRequests::LoadModulesResult loadModuleOutcomes;
        AZ::ModuleManagerRequestBus::BroadcastResult(loadModuleOutcomes, &AZ::ModuleManagerRequests::LoadDynamicModules, descriptor.m_modules, AZ::ModuleInitializationSteps::RegisterComponentDescriptors, true);

        for (const auto& loadModuleOutcome : loadModuleOutcomes)
        {
            AZ_Error("Application", loadModuleOutcome.IsSuccess(), loadModuleOutcome.GetError().c_str());
            if (!loadModuleOutcome.IsSuccess())
            {
                return false;
            }
        }

        return true;
    }

    void Application::StartCommon(AZ::Entity* systemEntity)
    {
        // Startup default local FileIO (hits OSAllocator) if not already setup.
        if (AZ::IO::FileIOBase::GetInstance() == nullptr)
        {
            m_defaultFileIO.reset(aznew AZ::IO::LocalFileIO());
            AZ::IO::FileIOBase::SetInstance(m_defaultFileIO.get());
        }

        m_pimpl.reset(Implementation::Create());

        if ((m_argC) && (m_argV))
        {
            m_commandLine.Parse(*m_argC, *m_argV);
        }
        else
        {
            m_commandLine.Parse();
        }

#if defined(AZ_PLATFORM_EMSCRIPTEN)
        if (m_commandLine.HasSwitch("cdn") && m_commandLine.GetNumSwitchValues("cdn") > 0)
        {
            AZ_TracePrintf(s_azFrameworkWarningWindow, "Set the cdn root to '%s'", m_commandLine.GetSwitchValue("cdn", 0).c_str());
            SetRootPath(RootPathType::CdnRoot, m_commandLine.GetSwitchValue("cdn", 0).c_str());
        }
        else
        {
            AZ_TracePrintf(s_azFrameworkWarningWindow, "Defaulting the cdn root to '%s'", s_engineConfigDefaultExternalCdnPath);
            SetRootPath(RootPathType::CdnRoot, s_engineConfigDefaultExternalCdnPath);
        }
#endif

        systemEntity->Init();
        systemEntity->Activate();
        AZ_Assert(systemEntity->GetState() == AZ::Entity::ES_ACTIVE, "System Entity failed to activate.");

        m_isStarted = true;
    }

    void Application::PreModuleLoad()
    {
        AZ_Assert(m_appRootInitialized, "App Root not initialized yet");

#if !defined(AZ_PLATFORM_EMSCRIPTEN)
        // Calculate the engine root by reading the game.json file
        AZStd::string  engineJsonPath = AZStd::string::format("%s%s", m_appRoot, s_engineConfigFileName);
        AZ::IO::LocalFileIO localFileIO;
        auto readJsonResult = AZ::FileFunc::ReadJsonFile(engineJsonPath, &localFileIO);

        if (readJsonResult.IsSuccess())
        {
            rapidjson::Document engineJson = readJsonResult.TakeValue();

            auto externalEnginePath = engineJson.FindMember(s_engineConfigExternalEnginePathKey);
            if (externalEnginePath != engineJson.MemberEnd())
            {
                // Initialize the engine root to the value of the external engine path key in the json file if it exists
                auto engineExternalPathString = externalEnginePath->value.GetString();
                SetRootPath(RootPathType::EngineRoot, engineExternalPathString);
            }
            else
            {
                // If the external engine path key does not exist in the json file, then set the engine root to be the same
                // as the app root
                SetRootPath(RootPathType::EngineRoot, m_appRoot);
            }
            AZ_TracePrintf(s_azFrameworkWarningWindow, "Engine Path: %s\n", m_engineRoot);

            auto externalAssetsPath = engineJson.FindMember(s_engineConfigExternalAssetsPathKey);
            if (externalAssetsPath != engineJson.MemberEnd())
            {
                // Initialize the asset root to the value of the external engine path key in the json file if it exists
                auto assetsExternalPathString = externalAssetsPath->value.GetString();
                SetRootPath(RootPathType::AssetRoot, assetsExternalPathString);
            }
            else
            {
                // If the external engine path key does not exist in the json file, then set the engine root to be the same
                // as the app root
                SetRootPath(RootPathType::AssetRoot, m_appRoot);
            }
#if defined(AZ_PLATFORM_ANDROID) || defined(AZ_PLATFORM_APPLE_IOS)
            // On iOS and Android, we should always use default public storage path
            AZStd::string publicStoragePath = Implementation::GetPublicStoragePath();
            NormalizePathKeepCase(publicStoragePath);
            SetRootPath(RootPathType::AssetRoot, publicStoragePath.c_str());
#endif
            AZ_TracePrintf(s_azFrameworkWarningWindow, "Asset Path: %s\n", m_assetRoot);

            auto externalCdnPath = engineJson.FindMember(s_engineConfigExternalCdnPathKey);
            if (externalCdnPath != engineJson.MemberEnd())
            {
                // Initialize the cdn root to the value of the external engine path key in the json file if it exists
                auto cdnExternalPathString = externalCdnPath->value.GetString();
                SetRootPath(RootPathType::CdnRoot, cdnExternalPathString);
            }
            else
            {
                // If the external cdn path key does not exist in the json file, then set the engine root to be localhost
                SetRootPath(RootPathType::CdnRoot, s_engineConfigDefaultExternalCdnPath);
            }
            AZ_TracePrintf(s_azFrameworkWarningWindow, "CDN Path: %s\n", m_cdnRoot);
        }
        else
        {
            // If there is any problem reading the engine.json file, then default to engine root to the app root
            AZ_Warning(s_azFrameworkWarningWindow, false, "Unable to read game.cfg file '%s' (%s).\n"
                "\tDefaulting the engine root to '%s'\n"
                "\tDefaulting the asset root to '%s'\n"
                "\tDefaulting the cdn root to '%s'", engineJsonPath.c_str(), readJsonResult.GetError().c_str(),
                m_appRoot, m_appRoot, s_engineConfigDefaultExternalCdnPath);
            SetRootPath(RootPathType::EngineRoot, m_appRoot);
            SetRootPath(RootPathType::AssetRoot, m_appRoot);
            SetRootPath(RootPathType::CdnRoot, s_engineConfigDefaultExternalCdnPath);
        }
#endif
    }

    void Application::SaveConfiguration()
    {
        if (m_configFilePath[0])
        {
            WriteApplicationDescriptor(m_configFilePath);
        }
    }

    void Application::Stop()
    {
        if (m_isStarted)
        {
            m_pimpl.reset();

            if (AZ::IO::FileIOBase::GetInstance() == m_defaultFileIO.get())
            {
                AZ::IO::FileIOBase::SetInstance(nullptr);
            }
            m_defaultFileIO.reset();

            // Free any memory owned by the command line container.
            m_commandLine = CommandLine();

            Destroy();

            m_isStarted = false;
        }
    }

    void Application::Reflect(AZ::ReflectContext* context)
    {
        AZ::ComponentApplication::Reflect(context);

        AZ::BehaviorEntity::Reflect(context);
        AZ::EntityContext::Reflect(context);
        AZ::UtilOprate::Reflect(context);
    }

    AZ::ComponentTypeList Application::GetRequiredSystemComponents() const
    {
        AZ::ComponentTypeList components = ComponentApplication::GetRequiredSystemComponents();

        components.insert(components.end(), {
            azrtti_typeid<AZ::MemoryComponent>(),
            azrtti_typeid<AZ::AssetManagerComponent>(),
            azrtti_typeid<AZ::JobManagerComponent>(),
            azrtti_typeid<AZ::GameEntityContextComponent>(),
            azrtti_typeid<AZ::InputSystemComponent>(),
            azrtti_typeid<AZ::InputEventSystemComponent>(),
            azrtti_typeid<AZ::SliceSystemComponent>(),

#if !defined(AZCORE_EXCLUDE_LUA)
            azrtti_typeid<AZ::ScriptSystemComponent>(),
#endif // #if !defined(AZCORE_EXCLUDE_LUA)
        });

        return components;
    }

    AZ::Component* Application::EnsureComponentAdded(AZ::Entity* systemEntity, const AZ::Uuid& typeId)
    {
        AZ::Component* component = systemEntity->FindComponent(typeId);

        if (!component)
        {
            if (systemEntity->IsComponentReadyToAdd(typeId))
            {
                component = systemEntity->CreateComponent(typeId);
            }
            else
            {
                AZ_Assert(false, "Failed to add component of type %s because conditions are not met.", typeId.ToString<AZStd::string>().c_str());
            }
        }

        return component;
    }

    void Application::CalculateAppRoot(const char* appRootOverride) // = nullptr
    {
        if (appRootOverride)
        {
            SetRootPath(RootPathType::AppRoot, appRootOverride);
        }
        else
        {
            CalculateExecutablePath();
#if   defined(AZ_PLATFORM_ANDROID)
            // On Android, all file reads should be relative.
            SetRootPath(RootPathType::AppRoot, "");
#elif defined(AZ_PLATFORM_APPLE_IOS) || defined(AZ_PLATFORM_APPLE_TV)
            AZ_Assert(appRootOverride != nullptr, "App root must be set explicitly for apple platforms.");
#elif defined(AZ_PLATFORM_EMSCRIPTEN)
            // On Emscripten, all file reads should be relative.
            SetRootPath(RootPathType::AppRoot, "");
#else
            // We need to reliably calculate the root path here prior to global engine init.
            // The system allocator is not yet initialized, so it's important we don't try
            // to allocate from the heap or use AZStd::string in here.

            // getcwd (and GetCurrentDirectoryA) return the directory from which the
            // application was launched, not the directory containing the executable.
            // This should be changed, but this will need to be implemented for each
            // platform that we support:
            // http://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from
            // http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe/1024937#1024937
            static char currentDirectory[AZ_MAX_PATH_LEN];
            static const char* engineRootFile = "game.cfg";
            static const size_t engineRootFileLength = strlen(engineRootFile);

            CalculateExecutablePath();
            const char* exeFolderPath = GetExecutableFolder();
            azstrncpy(currentDirectory, AZ_ARRAY_SIZE(currentDirectory), exeFolderPath, strlen(exeFolderPath));

            AZ_Assert(strlen(currentDirectory) > 0, "Failed to get current working directory.");

            AZStd::replace(currentDirectory, currentDirectory + AZ_MAX_PATH_LEN, '\\', '/');

            // Add a trailing slash if one does not already exist
            if (currentDirectory[strlen(currentDirectory) - 1] != '/')
            {
                azstrncat(currentDirectory, AZ_ARRAY_SIZE(currentDirectory), "/", 1);
            }

            // this closures assumes that there is a '/' character at the end
            auto cdPathUp = [](char path[])
            {
                size_t separatorCutoff = StringFunc::Find(path, "/", 1, true, false);
                if (separatorCutoff != AZStd::string::npos)
                {
                    path[separatorCutoff + 1] = '\0';
                }

                return separatorCutoff;
            };

            size_t separatorCutoff = AZStd::string::npos;
            do
            {
                // Add the file to check for into the directory path
                azstrncat(currentDirectory, AZ_ARRAY_SIZE(currentDirectory), engineRootFile, engineRootFileLength);

                // Look for the file that identifies the engine root
                bool engineFileExists = AZ::IO::SystemFile::Exists(currentDirectory);

                // remove the filename appended previously to do the engine root file check
                cdPathUp(currentDirectory);

                if (engineFileExists)
                {
                    break;
                }

                separatorCutoff = cdPathUp(currentDirectory);

            } while (separatorCutoff != AZStd::string::npos);

            // Add a trailing slash if one does not already exist
            if (currentDirectory[strlen(currentDirectory) - 1] != '/')
            {
                azstrncat(currentDirectory, AZ_ARRAY_SIZE(currentDirectory), "/", 1);
            }
            SetRootPath(RootPathType::AppRoot, currentDirectory);
#endif
        }

        // Asset root defaults to application root. The SetAssetRoot Ebus message can be used
        // to set a game-specific asset root.
        SetRootPath(RootPathType::AssetRoot, m_appRoot);
    }

    const char* Application::GetCurrentConfigurationName() const
    {
#if defined(_RELEASE)
        return "Release";
#elif defined(_DEBUG)
        return "Debug";
#else
        return "Profile";
#endif
    }

    ////////////////////////////////////////////////////////////////////////////
    AZ::Uuid Application::GetComponentTypeId(const AZ::EntityId& entityId, const AZ::ComponentId& componentId)
    {
        AZ::Uuid uuid(AZ::Uuid::CreateNull());
        AZ::Entity* entity = nullptr;
        EBUS_EVENT_RESULT(entity, AZ::ComponentApplicationBus, FindEntity, entityId);
        if (entity)
        {
            AZ::Component* component = entity->FindComponent(componentId);
            if (component)
            {
                uuid = component->RTTI_GetType();
            }
        }
        return uuid;
    }

    bool Application::IsEngineExternal() const
    {
        return (azstrnicmp(m_engineRoot, m_appRoot, AZ_ARRAY_SIZE(m_engineRoot)) != 0);
    }

    void Application::ResolveEnginePath(AZStd::string& engineRelativePath) const
    {
        AZStd::string fullPath = AZStd::string(m_engineRoot) + AZStd::string(AZ_CORRECT_FILESYSTEM_SEPARATOR_STRING) + engineRelativePath;
        engineRelativePath = fullPath;
    }

    void Application::CalculateBranchTokenForAppRoot(AZStd::string& token) const
    {
        AZ::StringFunc::AssetPath::CalculateBranchToken(AZStd::string(m_appRoot), token);
    }

    ////////////////////////////////////////////////////////////////////////////
    void Application::SetAssetRoot(const char* assetRoot)
    {
        SetRootPath(RootPathType::AssetRoot, assetRoot);
    }

    ////////////////////////////////////////////////////////////////////////////
    void Application::MakePathRootRelative(AZStd::string& fullPath) 
    { 
        MakePathRelative(fullPath, m_appRoot); 
    }

    ////////////////////////////////////////////////////////////////////////////
    void Application::MakePathAssetRootRelative(AZStd::string& fullPath) 
    { 
        MakePathRelative(fullPath, m_assetRoot); 
    }

    ////////////////////////////////////////////////////////////////////////////
    void Application::MakePathRelative(AZStd::string& fullPath, const char* rootPath)
    {
        AZ_Assert(rootPath, "Provided root path is null.");

        NormalizePathKeepCase(fullPath);

        if (strstr(fullPath.c_str(), rootPath) == fullPath.c_str())
        {
            const size_t len = strlen(rootPath);
            fullPath = fullPath.substr(len);
        }

        while (!fullPath.empty() && fullPath[0] == '/')
        {
            fullPath.erase(fullPath.begin());
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    void Application::NormalizePath(AZStd::string& path)
    {
        ComponentApplication::NormalizePath(path.begin(), path.end(), true);
    }

    ////////////////////////////////////////////////////////////////////////////
    void Application::NormalizePathKeepCase(AZStd::string& path)
    {
        ComponentApplication::NormalizePath(path.begin(), path.end(), false);
    }

    ////////////////////////////////////////////////////////////////////////////
    void Application::PumpSystemEventLoopOnce()
    {
        if (m_pimpl)
        {
            m_pimpl->PumpSystemEventLoopOnce();
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    void Application::PumpSystemEventLoopUntilEmpty()
    {
        if (m_pimpl)
        {
            m_pimpl->PumpSystemEventLoopUntilEmpty();
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    void Application::RunMainLoop()
    {
        while (!m_exitMainLoopRequested)
        {
            PumpSystemEventLoopUntilEmpty();
            Tick();
        }
    }

    void Application::SetRootPath(RootPathType type, const char* source)
    {
        size_t sourceLen = strlen(source);

        // Determine if we need to append a trailing path separator
        bool appendTrailingPathSep = (sourceLen>0) ? ((source[sourceLen - 1] != AZ_CORRECT_FILESYSTEM_SEPARATOR) && (source[sourceLen - 1] != AZ_WRONG_FILESYSTEM_SEPARATOR)) : false;

        // Copy the source path to the intended root path and correct the path separators as well
        switch (type)
        {
        case RootPathType::AppRoot:
            if (sourceLen == 0)
            {
                m_appRoot[0] = '\0';
            }
            else
            {
                AZ_Assert((sourceLen + 1) < AZ_ARRAY_SIZE(m_appRoot) - 1, "String overflow for App Root: %s", source);
                azstrncpy(m_appRoot, AZ_ARRAY_SIZE(m_appRoot) - 1, source, sourceLen);

                AZStd::replace(std::begin(m_appRoot), std::end(m_appRoot), AZ_WRONG_FILESYSTEM_SEPARATOR, AZ_CORRECT_FILESYSTEM_SEPARATOR);
                m_appRoot[sourceLen] = appendTrailingPathSep ? AZ_CORRECT_FILESYSTEM_SEPARATOR : '\0';
                m_appRoot[sourceLen + 1] = '\0';
            }
            m_appRootInitialized = true;
            break;
        case RootPathType::AssetRoot:
            if (sourceLen == 0)
            {
                m_assetRoot[0] = '\0';
            }
            else
            {
                AZ_Assert((sourceLen + 1) < AZ_ARRAY_SIZE(m_assetRoot) - 1, "String overflow for Asset Root: %s", source);
                azstrncpy(m_assetRoot, AZ_ARRAY_SIZE(m_assetRoot) - 1, source, sourceLen);

                AZStd::replace(std::begin(m_assetRoot), std::end(m_assetRoot), AZ_WRONG_FILESYSTEM_SEPARATOR, AZ_CORRECT_FILESYSTEM_SEPARATOR);
                m_assetRoot[sourceLen] = appendTrailingPathSep ? AZ_CORRECT_FILESYSTEM_SEPARATOR : '\0';
                m_assetRoot[sourceLen + 1] = '\0';
            }
            break;
        case RootPathType::EngineRoot:
            if (sourceLen == 0)
            {
                m_engineRoot[0] = '\0';
            }
            else
            {
                AZ_Assert((sourceLen + 1) < AZ_ARRAY_SIZE(m_engineRoot) - 1, "String overflow for Engine Root: %s", source);
                azstrncpy(m_engineRoot, AZ_ARRAY_SIZE(m_engineRoot) - 1, source, sourceLen);

                AZStd::replace(std::begin(m_engineRoot), std::end(m_engineRoot), AZ_WRONG_FILESYSTEM_SEPARATOR, AZ_CORRECT_FILESYSTEM_SEPARATOR);
                m_engineRoot[sourceLen] = appendTrailingPathSep ? AZ_CORRECT_FILESYSTEM_SEPARATOR : '\0';
                m_engineRoot[sourceLen + 1] = '\0';
            }
            break;
        case RootPathType::CdnRoot:
            if (sourceLen == 0)
            {
                m_cdnRoot[0] = '\0';
            }
            else
            {
                AZ_Assert((sourceLen + 1) < AZ_ARRAY_SIZE(m_cdnRoot) - 1, "String overflow for cdn Root: %s", source);
                azstrncpy(m_cdnRoot, AZ_ARRAY_SIZE(m_cdnRoot) - 1, source, sourceLen);

                m_cdnRoot[sourceLen] = appendTrailingPathSep ? '/' : '\0';
                m_cdnRoot[sourceLen + 1] = '\0';
            }
            break;
        default:
            AZ_Assert(false, "Invalid RootPathType (%d)", static_cast<int>(type));
        }
    }


} // namespace AZ

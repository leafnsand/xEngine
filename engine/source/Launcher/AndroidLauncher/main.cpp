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
#include <AzCore/Android/AndroidEnv.h>
#include <AzCore/Android/Utils.h>

#include <AzCore/API/ApplicationAPI_android.h>
#include <AzCore/Application/Application.h>
#include <AzCore/Module/Module.h>

#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <android/native_activity.h>
#include <android/native_window.h>
#include <android_native_app_glue/android_native_app_glue.h>

#include <sys/resource.h>
#include <sys/types.h>

#if !defined(_RELEASE) || defined(RELEASE_LOGGING)
    #define ENABLE_LOGGING
#endif

#if defined(ENABLE_LOGGING)
    #define LOG_TAG "N3H5"
    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#else
    #define LOGI(...)
    #define LOGE(...)
#endif // !defined(_RELEASE)

#define MAIN_EXIT_FAILURE() exit(1)

AZ::Application* gApp = nullptr;

extern void CreateStaticModules(AZStd::vector<AZ::Module*>& modules);

namespace
{
    bool g_windowInitialized = false;

    //////////////////////////////////////////////////////////////////////////
    bool IncreaseResourceToMaxLimit(int resource)
    {
        const rlim_t maxLimit = RLIM_INFINITY;

        struct rlimit limit;
        if (getrlimit(resource, &limit) != 0)
        {
            LOGE("[ERROR] Failed to get limit for resource %d", resource);
            return false;
        }

        if (limit.rlim_max != maxLimit)
        {
            limit.rlim_max = maxLimit;
            if (setrlimit(resource, &limit) != 0)
            {
                LOGE("[ERROR] Failed to update limit for resource %d with value %ld", resource, maxLimit);
                return false;
            }
        }

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    void RegisterSignalHandler(int sig, void (*handler)(int))
    {
        struct sigaction action;
        sigaction(sig, NULL, &action);
        if (action.sa_handler == SIG_DFL && action.sa_sigaction == (void*)SIG_DFL)
        {
            action.sa_handler = handler;
            sigaction(sig, &action, NULL);
        }
    }

    // ----
    // System handlers
    // ----

    //////////////////////////////////////////////////////////////////////////
    void HandleSignal(int sig)
    {
        LOGE("[ERROR] Signal (%d) was just raised", sig);
        signal(sig, HandleSignal);
    }

    //////////////////////////////////////////////////////////////////////////
    // this callback is triggered on the same thread the events are pumped
    void HandleApplicationLifecycleEvents(android_app* appState, int32_t command)
    {
    #if !defined(_RELEASE)
        const char* commandNames[] = {
            "APP_CMD_INPUT_CHANGED",
            "APP_CMD_INIT_WINDOW",
            "APP_CMD_TERM_WINDOW",
            "APP_CMD_WINDOW_RESIZED",
            "APP_CMD_WINDOW_REDRAW_NEEDED",
            "APP_CMD_CONTENT_RECT_CHANGED",
            "APP_CMD_GAINED_FOCUS",
            "APP_CMD_LOST_FOCUS",
            "APP_CMD_CONFIG_CHANGED",
            "APP_CMD_LOW_MEMORY",
            "APP_CMD_START",
            "APP_CMD_RESUME",
            "APP_CMD_SAVE_STATE",
            "APP_CMD_PAUSE",
            "APP_CMD_STOP",
            "APP_CMD_DESTROY",
        };
        LOGI("Engine command received: %s", commandNames[command]);
    #endif

        AZ::Android::AndroidEnv* androidEnv = static_cast<AZ::Android::AndroidEnv*>(appState->userData);
        switch (command)
        {
            case APP_CMD_PAUSE:
            {
                EBUS_EVENT(AZ::AndroidLifecycleEvents::Bus, OnPause);
                androidEnv->SetIsRunning(false);
            }
            break;

            case APP_CMD_RESUME:
            {
                androidEnv->SetIsRunning(true);
                EBUS_EVENT(AZ::AndroidLifecycleEvents::Bus, OnResume);
            }
            break;

            case APP_CMD_DESTROY:
            {
                EBUS_EVENT(AZ::AndroidLifecycleEvents::Bus, OnDestroy);
            }
            break;

            case APP_CMD_INIT_WINDOW:
            {
                g_windowInitialized = true;
                androidEnv->SetWindow(appState->window);

                EBUS_EVENT(AZ::AndroidLifecycleEvents::Bus, OnWindowInit);
            }
            break;

            case APP_CMD_TERM_WINDOW:
            {
                EBUS_EVENT(AZ::AndroidLifecycleEvents::Bus, OnWindowDestroy);

                androidEnv->SetWindow(nullptr);
            }
            break;

            case APP_CMD_LOW_MEMORY:
            {
                EBUS_EVENT(AZ::AndroidLifecycleEvents::Bus, OnLowMemory);
            }
            break;
        }
    }
}


////////////////////////////////////////////////////////////////
// This is the main entry point of a native application that is using android_native_app_glue.  
// It runs in its own thread, with its own event loop for receiving input events and doing other things.
void android_main(android_app* appState)
{
    app_dummy();

    // Adding a start up banner so you can see when the game is starting up in amongst the logcat spam
    LOGI("******************************************************");
    LOGI("*              N3H5App      - Launching Game....     *");
    LOGI("******************************************************");

    if (    !IncreaseResourceToMaxLimit(RLIMIT_CORE) 
        ||  !IncreaseResourceToMaxLimit(RLIMIT_STACK))
    {
        MAIN_EXIT_FAILURE();
    }

    // register our signal handlers
    RegisterSignalHandler(SIGINT, HandleSignal);
    RegisterSignalHandler(SIGTERM, HandleSignal);

    // setup the system command handler
    appState->onAppCmd = HandleApplicationLifecycleEvents;

    // setup the android environment
    AZ::AllocatorInstance<AZ::OSAllocator>::Create();
    {
        AZ::Android::AndroidEnv::Descriptor descriptor;

        descriptor.m_jvm = appState->activity->vm;
        descriptor.m_activityRef = appState->activity->clazz;
        descriptor.m_assetManager = appState->activity->assetManager;

        descriptor.m_appPrivateStoragePath = appState->activity->internalDataPath;
        descriptor.m_appPublicStoragePath = appState->activity->externalDataPath;
        descriptor.m_obbStoragePath = appState->activity->obbPath;

        if (!AZ::Android::AndroidEnv::Create(descriptor))
        {
            AZ::Android::AndroidEnv::Destroy();

            LOGE("[ERROR] Failed to create the AndroidEnv\n");
            MAIN_EXIT_FAILURE();
        }

        AZ::Android::AndroidEnv* androidEnv = AZ::Android::AndroidEnv::Get();
        appState->userData = androidEnv;
        androidEnv->SetIsRunning(true);
    }

    // sync the window creation
    {
        // While not ideal to have the event pump code duplicated here and in AZ, this 
        // at least solves the splash screen issues when the window creation sync happened later
        // in initialization.  It's also the lesser of 2 evils, the other requiring a change in
        // how the platform specific private Application implementation is created for ALL 
        // platforms
        while (!g_windowInitialized)
        {
            int events;
            android_poll_source* source;

            while (ALooper_pollAll(0, NULL, &events, reinterpret_cast<void**>(&source)) >= 0)
            {
                if (source != NULL)
                {
                    source->process(appState, source);
                }
            }
        }

        // Now that the window has been created we can show the java splash screen.  We need
        // to do it here and not in the window init event because every time the app is 
        // backgrounded/foregrounded the window is destroyed/created, respectively.  So, we
        // don't want to show the splash screen when we resumed from a paused state.
        AZ::Android::Utils::ShowSplashScreen();
    }

    // Engine Config (game.cfg)
    const char* assetsPath = AZ::Android::Utils::FindAssetsDirectory();
    if (!assetsPath)
    {
        LOGE("#################################################");
        LOGE("[ERROR] Unable to locate game.cfg - Exiting!");
        LOGE("#################################################");
        MAIN_EXIT_FAILURE();
    }

    char pathToAssets[AZ_MAX_PATH_LEN] = { 0 };
    azsnprintf(pathToAssets, AZ_MAX_PATH_LEN, "%s%s", assetsPath, "assets");

    char pathToDescriptor[AZ_MAX_PATH_LEN] = { 0 };
    azsnprintf(pathToDescriptor, AZ_MAX_PATH_LEN, "%s/%s", pathToAssets, "game.xml");

    AZ::Application application;
    gApp = &application;

    AZ::Application::StartupParameters params;
    params.m_createStaticModulesCallback = CreateStaticModules;
    params.m_appRootOverride = pathToAssets;
    params.m_loadDynamicModules = false;

    application.Start(pathToDescriptor, params);

    // set the native app state with the application framework
    AZ::AndroidAppRequests::Bus::Broadcast(&AZ::AndroidAppRequests::SetAppState, appState);

    AZ::Android::Utils::DismissSplashScreen();

    application.RunMainLoop();

    // Shutdown
    AZ::Android::AndroidEnv::Destroy();

    application.Stop();
}


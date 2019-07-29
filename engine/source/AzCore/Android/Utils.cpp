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
#include <AzCore/PlatformDef.h>

#if defined(AZ_PLATFORM_ANDROID)

#include <AzCore/Android/Utils.h>
#include <AzCore/Android/AndroidEnv.h>
#include <AzCore/Android/JNI/Object.h>

#include <AzCore/Debug/Trace.h>

#include <AzCore/Memory/OSAllocator.h>


namespace AZ
{
    namespace Android
    {
        namespace Utils
        {
            namespace
            {
                ////////////////////////////////////////////////////////////////
                const char* GetApkAssetsPrefix()
                {
                    return "/APK/";
                }
            }


            ////////////////////////////////////////////////////////////////
            jclass GetActivityClassRef()
            {
                return AndroidEnv::Get()->GetActivityClassRef();
            }

            ////////////////////////////////////////////////////////////////
            jobject GetActivityRef()
            {
                return AndroidEnv::Get()->GetActivityRef();
            }

            ////////////////////////////////////////////////////////////////
            AAssetManager* GetAssetManager()
            {
                return AndroidEnv::Get()->GetAssetManager();
            }

            ////////////////////////////////////////////////////////////////
            const char* GetAppPrivateStoragePath()
            {
                return AndroidEnv::Get()->GetAppPrivateStoragePath();
            }

            ////////////////////////////////////////////////////////////////
            const char* GetAppPublicStoragePath()
            {
                return AndroidEnv::Get()->GetAppPublicStoragePath();
            }

            ////////////////////////////////////////////////////////////////
            const char* GetObbStoragePath()
            {
                return AndroidEnv::Get()->GetObbStoragePath();
            }

            ////////////////////////////////////////////////////////////////
            const char* GetPackageName()
            {
                return AndroidEnv::Get()->GetPackageName();
            }

            ////////////////////////////////////////////////////////////////
            int GetAppVersionCode()
            {
                return AndroidEnv::Get()->GetAppVersionCode();
            }

            ////////////////////////////////////////////////////////////////
            const char* GetObbFileName(bool mainFile)
            {
                return AndroidEnv::Get()->GetObbFileName(mainFile);
            }

            ////////////////////////////////////////////////////////////////
            bool IsApkPath(const char* filePath)
            {
                return (strncmp(filePath, GetApkAssetsPrefix(), 4) == 0); // +3 for "APK", +1 for '/' starting slash
            }

            ////////////////////////////////////////////////////////////////
            const char* StripApkPrefix(const char* filePath)
            {
                const int prefixLength = 5; // +3 for "APK", +2 for '/' on either end
                if (!IsApkPath(filePath))
                {
                    return filePath;
                }

                return filePath + prefixLength;
            }

            ////////////////////////////////////////////////////////////////
            const char* FindAssetsDirectory()
            {
                return GetApkAssetsPrefix();
            }

            ////////////////////////////////////////////////////////////////
            void ShowSplashScreen()
            {
                JNI::Internal::Object<AZ::OSAllocator> activity(GetActivityClassRef(), GetActivityRef());

                activity.RegisterMethod("ShowSplashScreen", "()V");
                activity.InvokeVoidMethod("ShowSplashScreen");
            }

            ////////////////////////////////////////////////////////////////
            void DismissSplashScreen()
            {
                JNI::Internal::Object<AZ::OSAllocator> activity(GetActivityClassRef(), GetActivityRef());

                activity.RegisterMethod("DismissSplashScreen", "()V");
                activity.InvokeVoidMethod("DismissSplashScreen");
            }

            ////////////////////////////////////////////////////////////////
            ANativeWindow* GetWindow()
            {
                return AndroidEnv::Get()->GetWindow();
            }

            ////////////////////////////////////////////////////////////////
            bool GetWindowSize(int& widthPixels, int& heightPixels)
            {
                ANativeWindow* window = GetWindow();
                if (window)
                {
                    widthPixels = ANativeWindow_getWidth(window);
                    heightPixels = ANativeWindow_getHeight(window);

                    // should an error occur from the above functions a negative value will be returned
                    return (widthPixels > 0 && heightPixels > 0);
                }
                return false;
            }
        }
    }
}

#endif // defined(AZ_PLATFORM_ANDROID)


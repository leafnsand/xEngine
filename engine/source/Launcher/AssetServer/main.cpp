#include <Windows.h>

#include <AzCore/Application/Application.h>
#include <AzCore/Module/Module.h>

AZ::Application* gApp = nullptr;

extern void CreateStaticModules(AZStd::vector<AZ::Module*>& modules);

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    AZ::Application application(&__argc, &__argv);
    gApp = &application;

    application.CalculateExecutablePath();

    char pathToAssets[AZ_MAX_PATH_LEN] = { 0 };
    azsnprintf(pathToAssets, AZ_MAX_PATH_LEN, "%s%s", application.GetExecutableFolder(), "assets");

    char pathToDescriptor[AZ_MAX_PATH_LEN] = { 0 };
    azsnprintf(pathToDescriptor, AZ_MAX_PATH_LEN, "%s/%s", pathToAssets, "game.xml");

    AZ::Application::StartupParameters params;
    params.m_createStaticModulesCallback = CreateStaticModules;
    params.m_appRootOverride = pathToAssets;
    params.m_loadDynamicModules = false;

    application.Start(pathToDescriptor, params);
    application.RunMainLoop();
    application.Stop();

    return 0;
}

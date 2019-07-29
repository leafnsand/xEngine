#include <AzCore/Application/Application.h>

#include <emscripten/emscripten.h>

AZ::Application* gApp = nullptr;

extern void CreateStaticModules(AZStd::vector<AZ::Module*>& modules);

void main_loop()
{
    gApp->Tick();
}

int main(int argc, char* argv[])
{
    AZ::Application application(&argc, &argv);
    gApp = &application;

    AZ::Application::Descriptor desc;

    AZ::Application::StartupParameters params;
    params.m_createStaticModulesCallback = CreateStaticModules;
    params.m_loadDynamicModules = false;

    application.Start(desc, params);
    emscripten_set_main_loop(main_loop, 0, 1);

    return 0;
}
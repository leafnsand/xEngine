#include <AzCore/Module/Module.h>

#define DECLARE_MODULE(MODULE_NAME) extern "C" AZ::Module * CreateModuleClass_##MODULE_NAME()
#define INITIAL_MODULE(MODULE_NAME) CreateModuleClass_##MODULE_NAME()

// DECLARE_MODULE(FairyModule);
// DECLARE_MODULE(FontModule);
DECLARE_MODULE(LevelModule);
DECLARE_MODULE(RendererModule);
// DECLARE_MODULE(SpineModule);
DECLARE_MODULE(WindowModule);
// DECLARE_MODULE(GfxModule);
DECLARE_MODULE(NetworkModule);
// DECLARE_MODULE(Particle2dModule);
// DECLARE_MODULE(Physics2dModule);


void CreateStaticModules(AZStd::vector<AZ::Module*>& modules)
{
    // modules.push_back(INITIAL_MODULE(FairyModule));
    // modules.push_back(INITIAL_MODULE(FontModule));
    modules.push_back(INITIAL_MODULE(LevelModule));
    modules.push_back(INITIAL_MODULE(RendererModule));
    // modules.push_back(INITIAL_MODULE(SpineModule));
    modules.push_back(INITIAL_MODULE(WindowModule));
    // modules.push_back(INITIAL_MODULE(GfxModule));
    modules.push_back(INITIAL_MODULE(NetworkModule));
    // modules.push_back(INITIAL_MODULE(Particle2dModule));
    // modules.push_back(INITIAL_MODULE(Physics2dModule));
}
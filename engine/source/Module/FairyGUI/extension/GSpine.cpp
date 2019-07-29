#include "GSpine.h"
//#include "n3spine/spinefacade.h"
#include "render/RenderDataProcess.h"

//using namespace Spine;

NS_FGUI_BEGIN

GSpine::~GSpine()
{
//    SpineFacade::Instance()->DestroySpine(_spineId);
}

void GSpine::Init(const AZStd::string& spineName)
{
//    _spineId = SpineFacade::Instance()->CreateSpine(spineName);
}

void GSpine::RefreshTransform()
{
    GObject::RefreshTransform();
    
//    Math::transform44* transform = SpineFacade::Instance()->GetSpineTransform(_spineId); 
//    if(transform)
//    {
//        Math::matrix44 globalM = this->GetGlobalMatrix44();
//        Math::float4 scalef4 = Math::float4(RenderDataProcess::Zoom, RenderDataProcess::Zoom, 0, 1);
//        globalM.scale(scalef4);
//        Math::float4 posf4 = globalM.get_position();
//        posf4.Y = RenderDataProcess::Height - posf4.Y;
//        globalM.set_position(posf4);
//        transform->setmatrix(globalM);
//    }
//
//    SpineFacade::Instance()->Update(_spineId);
}

NS_FGUI_END
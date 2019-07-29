#include "GParticle2d.h"
//#include "Particle/particle2dfacade.h"
#include "render/RenderDataProcess.h"

//using namespace Particle;

NS_FGUI_BEGIN

GParticle2d::~GParticle2d()
{
//    Particle2DFacade::Instance()->DestroyParticle2D(_particleId);
}

void GParticle2d::Init(const AZStd::string& particleName)
{
//    _particleId = Particle2DFacade::Instance()->CreateParticle2D(particleName);
}

void GParticle2d::RefreshTransform()
{
    GObject::RefreshTransform();
    
//    Math::transform44* transform = Particle2DFacade::Instance()->GetParticle2DTransform(_particleId);
//    if (transform)
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
//    Particle2DFacade::Instance()->UpdateParticle2d(_particleId);
}

NS_FGUI_END
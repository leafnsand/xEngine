#pragma once

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "ui/GObject.h"
//#include "Particle/particle2dtypes.h"

//using namespace Particle;

NS_FGUI_BEGIN

class GParticle2d : public GObject
{
    FUI_RTTI_DERIVED(GParticle2d, GObject)
public:
    GParticle2d(){}
    virtual ~GParticle2d();

    CREATE_FUNC(GParticle2d);

    void Init(const AZStd::string& particleName);

    void RefreshTransform() override;
//    Particle2DId GetParticleId() { return _particleId; }
//
//private:
//    Particle2DId _particleId;
};

NS_FGUI_END

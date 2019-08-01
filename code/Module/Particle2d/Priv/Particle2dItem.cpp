#include "Particle2d/Priv/Particle2dItem.h"
#include "Particle2d/Component/Particle2dComponent.h"

namespace Particle2d
{
    void Particle2DItem::Setup(Module::Particle2dComponent* componentPtr, Particle2dNode* nodePtr, Particle2DLauncher* launcherPtr, float createTime, float lifeTime)
    {
        this->m_componentPtr = componentPtr;
        this->m_createTime = createTime;
        this->m_lifeTime = lifeTime;
        this->m_deadTime = lifeTime + createTime;
        this->m_unit = aznew Particle2DUnit;
        this->m_unit->Setup(this);
        this->m_fatherLauncher = launcherPtr;

        int launcherNum = nodePtr->m_childNodeVector.size();
        if (launcherNum > 0)
        {
            for (int i = 0; i < launcherNum; i++)
            {
                Particle2DLauncher* pl = aznew Particle2DLauncher();
                pl->Setup(this, componentPtr->m_nodeVector[nodePtr->m_childNodeVector[i]]);
                this->m_launcherVector.push_back(pl);
            }
        }
    }

    void Particle2DItem::Setup(Module::Particle2dComponent* componentPtr, float createTime, float lifeTime)
    {
        this->m_componentPtr = componentPtr;
        this->m_createTime = createTime;
        this->m_deadTime = lifeTime;
        for (int i = 0; i < m_componentPtr->m_childNodeVector.size(); i++)
        {
            Particle2DLauncher* pl = new Particle2DLauncher();
            pl->Setup(this, m_componentPtr->m_nodeVector[m_componentPtr->m_childNodeVector[i]]);
            this->m_launcherVector.push_back(pl);
        }
    }

    bool Particle2DItem::CheckLive()
    {
        return (this->m_deadTime > this->m_componentPtr->m_liveTime);
    }

    void Particle2DItem::Discard()
    {
        if (!this->m_launcherVector.empty())
        {
            for (int i = 0;i< m_launcherVector.size();i++)
            {
               m_launcherVector[i]->Discard();
               delete m_launcherVector[i];
            }
        }
        m_launcherVector.clear();

        if (this->m_unit)
        {
            this->m_unit->Discard();
            delete this->m_unit;
            this->m_unit = nullptr;
        }
        
        this->m_fatherLauncher = nullptr;
        this->m_componentPtr = nullptr;
     
    }

    void Particle2DItem::OnUpdate(float deltaTime)
    {
        for (int i = 0; i < m_launcherVector.size(); i++)
        {
            m_launcherVector[i]->OnUpdate(deltaTime);
        }
        if (this->m_unit != nullptr && this->m_componentPtr->m_liveTime < m_deadTime)
        {
            m_unit->OnUpdate();
        }
    }

    void Particle2DItem::BRUpdate(float deltaTime)
    {
        for (int i = 0; i < m_launcherVector.size(); i++)
        {
            m_launcherVector[i]->BRUpdate(deltaTime);
        }
    }
}
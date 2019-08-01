#pragma once

#include "Ref.h"
#include "AzCore/std/containers/vector.h"

class AutoreleasePool
{
    FUI_Declare_Singleton(AutoreleasePool)
public:
    ~AutoreleasePool()
    {
        Clear();
    }

    void Clear()
    {
        auto it = _vecRef.begin();
        while(it != _vecRef.end())
        {
            (*it)->release();
            ++it;
        }
        _vecRef.clear();
    }

    void AddObject(fairygui::Ref* ref)
    {
        _vecRef.push_back(ref);
    }

private:
    AZStd::vector<fairygui::Ref*> _vecRef;
};

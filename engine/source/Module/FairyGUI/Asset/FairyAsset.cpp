#include "FairyGUI/Asset/FairyAsset.h"

namespace Module
{
    FairyAsset::~FairyAsset()
    {
        if (m_data && m_length > 0)
        {
            azfree(m_data);
            m_data = nullptr;
            m_length = 0;
        }
    }
}

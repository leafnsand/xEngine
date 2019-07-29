#include "Font/Asset/FontAsset.h"

namespace Module
{
    FontAsset::~FontAsset()
    {
        if (m_data && m_length > 0)
        {
            azfree(m_data);
            m_data = nullptr;
            m_length = 0;
        }
    }
}

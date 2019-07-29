/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/IO/GenericStreams.h>

#include <AzCore/std/containers/vector.h>

namespace AZ
{
    /**
    * Script Asset - contains the source code for a script
    */
    class ScriptAsset
        : public Data::AssetData
    {
    public:
        AZ_CLASS_ALLOCATOR(ScriptAsset, AZ::SystemAllocator, 0);
        AZ_RTTI(ScriptAsset, "{82557326-4AE3-416C-95D6-C70635AB7588}", Data::AssetData);

        ScriptAsset(const Data::AssetId& assetId = Data::AssetId());
        ~ScriptAsset() override = default;

        ScriptAsset(const ScriptAsset& rhs) = delete;
        ScriptAsset& operator=(const ScriptAsset& rhs) = delete;

        //////////////////////////////////////////////////////////////////////////
        // Script data
        const AZStd::vector<char>& GetScriptBuffer() { return m_scriptBuffer; }
        IO::MemoryStream CreateMemoryStream() { return IO::MemoryStream(m_scriptBuffer.data(), m_scriptBuffer.size()); }

        const char* GetDebugName() { return m_debugName.empty() ? nullptr : m_debugName.c_str(); }

    private:
        // This will be the buffer of either the precompiled script or the source (depending on asset selected)
        AZStd::vector<char> m_scriptBuffer;
        // Debug name of the script. Will be empty if script is precompiled
        AZStd::string m_debugName;

        friend class ScriptSystemComponent;
    };
}

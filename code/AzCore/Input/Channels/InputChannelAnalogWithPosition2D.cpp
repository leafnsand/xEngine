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

#include <AzCore/Input/Channels/InputChannelAnalogWithPosition2D.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace AZ
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputChannelAnalogWithPosition2D::InputChannelAnalogWithPosition2D(
        const InputChannelId& inputChannelId,
        const InputDevice& inputDevice)
    : InputChannelAnalog(inputChannelId, inputDevice)
    , m_positionData()
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    const InputChannel::CustomData* InputChannelAnalogWithPosition2D::GetCustomData() const
    {
        return &m_positionData;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputChannelAnalogWithPosition2D::ResetState()
    {
        m_positionData = PositionData2D();
        InputChannelAnalog::ResetState();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputChannelAnalogWithPosition2D::ProcessRawInputEvent(const RawInputEvent& rawValues)
    {
        const AZ::Vector2 newPosition = AZ::Vector2(rawValues.m_normalizedX, rawValues.m_normalizedY);
        const AZ::Vector2 oldPosition = m_positionData.m_normalizedPosition;

        m_positionData.m_normalizedPosition = newPosition;
        m_positionData.m_normalizedPositionDelta = newPosition - oldPosition;

        InputChannelAnalog::ProcessRawInputEvent(rawValues.m_analogValue);
    }
} // namespace AZ

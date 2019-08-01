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

#include <AzCore/PlatformDef.h>

#if defined(AZ_PLATFORM_EMSCRIPTEN)

#include <AzCore/Input/Devices/Touch/InputDeviceTouch.h>

#include <emscripten/html5.h>
#include <emscripten/emscripten.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace AZ
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //! Platform specific implementation for web touch input devices
    class InputDeviceTouchWeb : public InputDeviceTouch::Implementation
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////
        // Allocator
        AZ_CLASS_ALLOCATOR(InputDeviceTouchWeb, AZ::SystemAllocator, 0);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Constructor
        //! \param[in] inputDevice Reference to the input device being implemented
        InputDeviceTouchWeb(InputDeviceTouch& inputDevice);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Destructor
        ~InputDeviceTouchWeb() override;

    private:
        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceTouch::Implementation::IsConnected
        bool IsConnected() const override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceTouch::Implementation::TickInputDevice
        void TickInputDevice() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Touch event callback
        static EM_BOOL OnTouchEvent(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputDeviceTouch::Implementation* InputDeviceTouch::Implementation::Create(InputDeviceTouch& inputDevice)
    {
        return aznew InputDeviceTouchWeb(inputDevice);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputDeviceTouchWeb::InputDeviceTouchWeb(InputDeviceTouch& inputDevice)
        : InputDeviceTouch::Implementation(inputDevice)
    {
        emscripten_set_touchstart_callback("canvas", this, true, InputDeviceTouchWeb::OnTouchEvent);
        emscripten_set_touchend_callback("canvas", this, true, InputDeviceTouchWeb::OnTouchEvent);
        emscripten_set_touchmove_callback("canvas", this, true, InputDeviceTouchWeb::OnTouchEvent);
        emscripten_set_touchcancel_callback("canvas", this, true, InputDeviceTouchWeb::OnTouchEvent);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputDeviceTouchWeb::~InputDeviceTouchWeb()
    {
        emscripten_set_touchstart_callback("canvas", this, true, nullptr);
        emscripten_set_touchend_callback("canvas", this, true, nullptr);
        emscripten_set_touchmove_callback("canvas", this, true, nullptr);
        emscripten_set_touchcancel_callback("canvas", this, true, nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool InputDeviceTouchWeb::IsConnected() const
    {
        // Touch input is always available on web
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDeviceTouchWeb::TickInputDevice()
    {
        // Process the raw event queues once each frame
        ProcessRawEventQueues();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    EM_BOOL InputDeviceTouchWeb::OnTouchEvent(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData)
    {
        InputDeviceTouchWeb* self = (InputDeviceTouchWeb*)userData;

        float pressure = 1.0f;

        // Get the touch event state
        RawTouchEvent::State rawTouchState = RawTouchEvent::State::Began;
        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_TOUCHSTART:
            {
                rawTouchState = RawTouchEvent::State::Began;
            }
            break;
            case EMSCRIPTEN_EVENT_TOUCHMOVE:
            {
                rawTouchState = RawTouchEvent::State::Moved;
            }
            break;
            case EMSCRIPTEN_EVENT_TOUCHEND:
            case EMSCRIPTEN_EVENT_TOUCHCANCEL:
            {
                rawTouchState = RawTouchEvent::State::Ended;
                pressure = 0.0f;
            }
            break;
            default:
            {
                return false;
            }
            break;
        }

        double windowWidth, windowHeight;
        emscripten_get_element_css_size("canvas", &windowWidth, &windowHeight);

        for (int pointerId = 0; pointerId < touchEvent->numTouches; ++pointerId)
        {
            const RawTouchEvent rawTouchEvent(touchEvent->touches[pointerId].canvasX / static_cast<float>(windowWidth),
                                              touchEvent->touches[pointerId].canvasY / static_cast<float>(windowHeight),
                                              pressure,
                                              pointerId,
                                              rawTouchState);
            self->QueueRawTouchEvent(rawTouchEvent);
        }

        return true;
    }
}

#endif // defined(AZ_PLATFORM_EMSCRIPTEN)

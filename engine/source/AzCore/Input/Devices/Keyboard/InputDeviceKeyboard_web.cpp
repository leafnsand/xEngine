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

#include <AzCore/Input/Devices/Keyboard/InputDeviceKeyboard.h>

#include <emscripten/html5.h>
#include <emscripten/emscripten.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace
{
    using namespace AZ;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Table of key ids indexed by their web key code
    const AZStd::array<const InputChannelId*, 256> InputChannelIdByKeyCodeTable =
    {{
        nullptr,                                                        // 0x00
        nullptr,                                                        // 0x01
        nullptr,                                                        // 0x02
        nullptr,                                                        // 0x03
        nullptr,                                                        // 0x04
        nullptr,                                                        // 0x05
        nullptr,                                                        // 0x06
        nullptr,                                                        // 0x07
        &InputDeviceKeyboard::Key::EditBackspace,                       // 0x08
        &InputDeviceKeyboard::Key::EditTab,                             // 0x09
        nullptr,                                                        // 0x0A
        nullptr,                                                        // 0x0B
        nullptr,                                                        // 0x0C
        &InputDeviceKeyboard::Key::EditEnter,                           // 0x0D
        nullptr,                                                        // 0x0E
        nullptr,                                                        // 0x0F
        &InputDeviceKeyboard::Key::ModifierShiftL,                      // 0x10
        &InputDeviceKeyboard::Key::ModifierCtrlL,                       // 0x11
        &InputDeviceKeyboard::Key::ModifierAltL,                        // 0x12
        &InputDeviceKeyboard::Key::WindowsSystemPause,                  // 0x13
        &InputDeviceKeyboard::Key::EditCapsLock,                        // 0x14
        nullptr,                                                        // 0x15
        nullptr,                                                        // 0x16
        nullptr,                                                        // 0x17
        nullptr,                                                        // 0x18
        nullptr,                                                        // 0x19
        nullptr,                                                        // 0x1A
        &InputDeviceKeyboard::Key::Escape,                              // 0x1B
        nullptr,                                                        // 0x1C
        nullptr,                                                        // 0x1D
        nullptr,                                                        // 0x1E
        nullptr,                                                        // 0x1F
        &InputDeviceKeyboard::Key::EditSpace,                           // 0x20
        &InputDeviceKeyboard::Key::NavigationPageUp,                    // 0x21
        &InputDeviceKeyboard::Key::NavigationPageDown,                  // 0x22
        &InputDeviceKeyboard::Key::NavigationEnd,                       // 0x23
        &InputDeviceKeyboard::Key::NavigationHome,                      // 0x24
        &InputDeviceKeyboard::Key::NavigationArrowLeft,                 // 0x25
        &InputDeviceKeyboard::Key::NavigationArrowUp,                   // 0x26
        &InputDeviceKeyboard::Key::NavigationArrowRight,                // 0x27
        &InputDeviceKeyboard::Key::NavigationArrowDown,                 // 0x28
        nullptr,                                                        // 0x29
        nullptr,                                                        // 0x2A
        nullptr,                                                        // 0x2B
        &InputDeviceKeyboard::Key::WindowsSystemPrint,                  // 0x2C
        &InputDeviceKeyboard::Key::NavigationInsert,                    // 0x2D
        &InputDeviceKeyboard::Key::NavigationDelete,                    // 0x2E
        nullptr,                                                        // 0x2F
        &InputDeviceKeyboard::Key::Alphanumeric0,                       // 0x30
        &InputDeviceKeyboard::Key::Alphanumeric1,                       // 0x31
        &InputDeviceKeyboard::Key::Alphanumeric2,                       // 0x32
        &InputDeviceKeyboard::Key::Alphanumeric3,                       // 0x33
        &InputDeviceKeyboard::Key::Alphanumeric4,                       // 0x34
        &InputDeviceKeyboard::Key::Alphanumeric5,                       // 0x35
        &InputDeviceKeyboard::Key::Alphanumeric6,                       // 0x36
        &InputDeviceKeyboard::Key::Alphanumeric7,                       // 0x37
        &InputDeviceKeyboard::Key::Alphanumeric8,                       // 0x38
        &InputDeviceKeyboard::Key::Alphanumeric9,                       // 0x39
        nullptr,                                                        // 0x3A
        &InputDeviceKeyboard::Key::PunctuationSemicolon,                // 0x3B
        nullptr,                                                        // 0x3C
        nullptr,                                                        // 0x3D
        nullptr,                                                        // 0x3E
        nullptr,                                                        // 0x3F
        nullptr,                                                        // 0x40
        &InputDeviceKeyboard::Key::AlphanumericA,                       // 0x41
        &InputDeviceKeyboard::Key::AlphanumericB,                       // 0x42
        &InputDeviceKeyboard::Key::AlphanumericC,                       // 0x43
        &InputDeviceKeyboard::Key::AlphanumericD,                       // 0x44
        &InputDeviceKeyboard::Key::AlphanumericE,                       // 0x45
        &InputDeviceKeyboard::Key::AlphanumericF,                       // 0x46
        &InputDeviceKeyboard::Key::AlphanumericG,                       // 0x47
        &InputDeviceKeyboard::Key::AlphanumericH,                       // 0x48
        &InputDeviceKeyboard::Key::AlphanumericI,                       // 0x49
        &InputDeviceKeyboard::Key::AlphanumericJ,                       // 0x4A
        &InputDeviceKeyboard::Key::AlphanumericK,                       // 0x4B
        &InputDeviceKeyboard::Key::AlphanumericL,                       // 0x4C
        &InputDeviceKeyboard::Key::AlphanumericM,                       // 0x4D
        &InputDeviceKeyboard::Key::AlphanumericN,                       // 0x4E
        &InputDeviceKeyboard::Key::AlphanumericO,                       // 0x4F
        &InputDeviceKeyboard::Key::AlphanumericP,                       // 0x50
        &InputDeviceKeyboard::Key::AlphanumericQ,                       // 0x51
        &InputDeviceKeyboard::Key::AlphanumericR,                       // 0x52
        &InputDeviceKeyboard::Key::AlphanumericS,                       // 0x53
        &InputDeviceKeyboard::Key::AlphanumericT,                       // 0x54
        &InputDeviceKeyboard::Key::AlphanumericU,                       // 0x55
        &InputDeviceKeyboard::Key::AlphanumericV,                       // 0x56
        &InputDeviceKeyboard::Key::AlphanumericW,                       // 0x57
        &InputDeviceKeyboard::Key::AlphanumericX,                       // 0x58
        &InputDeviceKeyboard::Key::AlphanumericY,                       // 0x59
        &InputDeviceKeyboard::Key::AlphanumericZ,                       // 0x5A
        &InputDeviceKeyboard::Key::ModifierSuperL,                      // 0x5B
        nullptr,                                                        // 0x5C
        nullptr,                                                        // 0x5D
        nullptr,                                                        // 0x5E
        nullptr,                                                        // 0x5F
        &InputDeviceKeyboard::Key::NumPad0,                             // 0x60
        &InputDeviceKeyboard::Key::NumPad1,                             // 0x61
        &InputDeviceKeyboard::Key::NumPad2,                             // 0x62
        &InputDeviceKeyboard::Key::NumPad3,                             // 0x63
        &InputDeviceKeyboard::Key::NumPad4,                             // 0x64
        &InputDeviceKeyboard::Key::NumPad5,                             // 0x65
        &InputDeviceKeyboard::Key::NumPad6,                             // 0x66
        &InputDeviceKeyboard::Key::NumPad7,                             // 0x67
        &InputDeviceKeyboard::Key::NumPad8,                             // 0x68
        &InputDeviceKeyboard::Key::NumPad9,                             // 0x69
        &InputDeviceKeyboard::Key::NumPadMultiply,                      // 0x6A
        &InputDeviceKeyboard::Key::NumPadAdd,                           // 0x6B
        nullptr,                                                        // 0x6C
        &InputDeviceKeyboard::Key::NumPadSubtract,                      // 0x6D
        &InputDeviceKeyboard::Key::NumPadDecimal,                       // 0x6E
        &InputDeviceKeyboard::Key::NumPadDivide,                        // 0x6F
        &InputDeviceKeyboard::Key::Function01,                          // 0x70
        &InputDeviceKeyboard::Key::Function02,                          // 0x71
        &InputDeviceKeyboard::Key::Function03,                          // 0x72
        &InputDeviceKeyboard::Key::Function04,                          // 0x73
        &InputDeviceKeyboard::Key::Function05,                          // 0x74
        &InputDeviceKeyboard::Key::Function06,                          // 0x75
        &InputDeviceKeyboard::Key::Function07,                          // 0x76
        &InputDeviceKeyboard::Key::Function08,                          // 0x77
        &InputDeviceKeyboard::Key::Function09,                          // 0x78
        &InputDeviceKeyboard::Key::Function10,                          // 0x79
        &InputDeviceKeyboard::Key::Function11,                          // 0x7A
        &InputDeviceKeyboard::Key::Function12,                          // 0x7B
        &InputDeviceKeyboard::Key::Function13,                          // 0x7C
        &InputDeviceKeyboard::Key::Function14,                          // 0x7D
        &InputDeviceKeyboard::Key::Function15,                          // 0x7E
        &InputDeviceKeyboard::Key::Function16,                          // 0x7F
        &InputDeviceKeyboard::Key::Function17,                          // 0x80
        &InputDeviceKeyboard::Key::Function18,                          // 0x81
        &InputDeviceKeyboard::Key::Function19,                          // 0x82
        &InputDeviceKeyboard::Key::Function20,                          // 0x83
        nullptr,                                                        // 0x84
        nullptr,                                                        // 0x85
        nullptr,                                                        // 0x86
        nullptr,                                                        // 0x87
        nullptr,                                                        // 0x88
        nullptr,                                                        // 0x89
        nullptr,                                                        // 0x8A
        nullptr,                                                        // 0x8B
        nullptr,                                                        // 0x8C
        nullptr,                                                        // 0x8D
        nullptr,                                                        // 0x8E
        nullptr,                                                        // 0x8F
        &InputDeviceKeyboard::Key::NumLock,                             // 0x90
        &InputDeviceKeyboard::Key::WindowsSystemScrollLock,             // 0x91
        nullptr,                                                        // 0x92
        nullptr,                                                        // 0x93
        nullptr,                                                        // 0x94
        nullptr,                                                        // 0x95
        nullptr,                                                        // 0x96
        nullptr,                                                        // 0x97
        nullptr,                                                        // 0x98
        nullptr,                                                        // 0x99
        nullptr,                                                        // 0x9A
        nullptr,                                                        // 0x9B
        nullptr,                                                        // 0x9C
        nullptr,                                                        // 0x9D
        nullptr,                                                        // 0x9E
        nullptr,                                                        // 0x9F
        nullptr,                                                        // 0xA0
        nullptr,                                                        // 0xA1
        nullptr,                                                        // 0xA2
        nullptr,                                                        // 0xA3
        nullptr,                                                        // 0xA4
        nullptr,                                                        // 0xA5
        nullptr,                                                        // 0xA6
        nullptr,                                                        // 0xA7
        nullptr,                                                        // 0xA8
        nullptr,                                                        // 0xA9
        nullptr,                                                        // 0xAA
        nullptr,                                                        // 0xAB
        nullptr,                                                        // 0xAC
        &InputDeviceKeyboard::Key::PunctuationHyphen,                   // 0xAD
        nullptr,                                                        // 0xAE
        nullptr,                                                        // 0xAF
        nullptr,                                                        // 0xB0
        nullptr,                                                        // 0xB1
        nullptr,                                                        // 0xB2
        nullptr,                                                        // 0xB3
        nullptr,                                                        // 0xB4
        nullptr,                                                        // 0xB5
        nullptr,                                                        // 0xB6
        nullptr,                                                        // 0xB7
        nullptr,                                                        // 0xB8
        nullptr,                                                        // 0xB9
        nullptr,                                                        // 0xBA
        &InputDeviceKeyboard::Key::PunctuationEquals,                   // 0xBB
        &InputDeviceKeyboard::Key::PunctuationComma,                    // 0xBC
        nullptr,                                                        // 0xBD
        &InputDeviceKeyboard::Key::PunctuationPeriod,                   // 0xBE
        &InputDeviceKeyboard::Key::PunctuationSlash,                    // 0xBF
        &InputDeviceKeyboard::Key::PunctuationTilde,                    // 0xC0
    }};
}

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace AZ
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //! Platform specific implementation for web keyboard input devices
    class InputDeviceKeyboardWeb : public InputDeviceKeyboard::Implementation
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////
        // Allocator
        AZ_CLASS_ALLOCATOR(InputDeviceKeyboardWeb, AZ::SystemAllocator, 0);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Constructor
        //! \param[in] inputDevice Reference to the input device being implemented
        InputDeviceKeyboardWeb(InputDeviceKeyboard& inputDevice);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Destructor
        ~InputDeviceKeyboardWeb() override;

    private:
        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceKeyboard::Implementation::IsConnected
        bool IsConnected() const override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceKeyboard::Implementation::HasTextEntryStarted
        bool HasTextEntryStarted() const override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceKeyboard::Implementation::TextEntryStart
        void TextEntryStart(const InputTextEntryRequests::VirtualKeyboardOptions& options) override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceKeyboard::Implementation::TextEntryStop
        void TextEntryStop() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceKeyboard::Implementation::TickInputDevice
        void TickInputDevice() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Keyboard event callback
        static EM_BOOL OnKeyboardEvent(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Has text entry been started?
        bool m_hasTextEntryStarted = false;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputDeviceKeyboard::Implementation* InputDeviceKeyboard::Implementation::Create(InputDeviceKeyboard& inputDevice)
    {
        return aznew InputDeviceKeyboardWeb(inputDevice);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputDeviceKeyboardWeb::InputDeviceKeyboardWeb(InputDeviceKeyboard& inputDevice)
        : InputDeviceKeyboard::Implementation(inputDevice)
        , m_hasTextEntryStarted(false)
    {
        emscripten_set_keypress_callback(nullptr, this, true, InputDeviceKeyboardWeb::OnKeyboardEvent);
        emscripten_set_keydown_callback(nullptr, this, true, InputDeviceKeyboardWeb::OnKeyboardEvent);
        emscripten_set_keyup_callback(nullptr, this, true, InputDeviceKeyboardWeb::OnKeyboardEvent);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputDeviceKeyboardWeb::~InputDeviceKeyboardWeb()
    {
        emscripten_set_keypress_callback(nullptr, this, true, nullptr);
        emscripten_set_keydown_callback(nullptr, this, true, nullptr);
        emscripten_set_keyup_callback(nullptr, this, true, nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool InputDeviceKeyboardWeb::IsConnected() const
    {
        // Keyboard input is always available on web
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool InputDeviceKeyboardWeb::HasTextEntryStarted() const
    {
        return m_hasTextEntryStarted;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDeviceKeyboardWeb::TextEntryStart(const InputTextEntryRequests::VirtualKeyboardOptions&)
    {
        m_hasTextEntryStarted = true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDeviceKeyboardWeb::TextEntryStop()
    {
        m_hasTextEntryStarted = false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDeviceKeyboardWeb::TickInputDevice()
    {
        // Process the raw event queues once each frame
        ProcessRawEventQueues();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    EM_BOOL InputDeviceKeyboardWeb::OnKeyboardEvent(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
    {
        InputDeviceKeyboardWeb* self = (InputDeviceKeyboardWeb*)userData;

        const InputChannelId* channelId = (keyEvent->keyCode < InputChannelIdByKeyCodeTable.size()) ?
                                          InputChannelIdByKeyCodeTable[keyEvent->keyCode] : nullptr;

        if (channelId)
        {
            if (eventType == EMSCRIPTEN_EVENT_KEYDOWN)
            {
                self->QueueRawKeyEvent(*channelId, true);
            }
            else if (eventType == EMSCRIPTEN_EVENT_KEYUP)
            {
                self->QueueRawKeyEvent(*channelId, false);
            }
        }

        return true;
    }
} // namespace AZ

#endif // defined(AZ_PLATFORM_EMSCRIPTEN)

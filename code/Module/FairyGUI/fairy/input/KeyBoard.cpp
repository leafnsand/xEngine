#include "KeyBoard.h"


//using namespace Input;

NS_FGUI_BEGIN

bool isShift = false;
bool isCtrl = false;

char* toUpper(char* c)
{
    if (*c >= 'a' && *c <= 'z')
        *c += 'A' - 'a';

    return c;
}

char* toLower(char* c)
{
    if (*c >= 'A' && *c <= 'Z')
        *c += 'a' - 'A';

    return c;
}
/*
void KeyBoard::OperateStringKeyDown(AZStd::string& text, Key::Code keyCode, int& cursorPos)
{
    if (keyCode == Key::Code::Capital)
        isShift = !isShift;
    if (keyCode == Key::Code::LeftShift || keyCode == Key::Code::RightShift)
        isShift = !isShift;
    else if (keyCode == Key::Code::LeftControl || keyCode == Key::Code::RightControl)
        isCtrl = true;
    else if (keyCode == Key::Code::Space)    
        text.append(" ");
    else if (keyCode == Key::Code::Back)
    {
        if (cursorPos == 0) return;

        text.erase(cursorPos - 1, 1);
        --cursorPos;
    }
    else if (keyCode == Key::Code::Delete)
    {
        if (cursorPos == text.size()) return;

        text.erase(cursorPos, 1);
    }
    else if (keyCode == Key::Code::Left)
    {
        if (cursorPos == 0) return;

        --cursorPos;
    }
    else if (keyCode == Key::Code::Right)
    {
        if (cursorPos == text.size()) return;

        ++cursorPos;
    }
    else if (keyCode >= Key::Code::A && keyCode <= Key::Code::Z)
    {
        if(isCtrl)
        {
            
        }
        else
        {

            if (isShift)
            {
                AZStd::string s = Key::ToString(keyCode);
                text.append(toUpper(const_cast<char*>(s.c_str())));

                ++cursorPos;
            }
            else
            {
                AZStd::string s = Key::ToString(keyCode);
                text.append(toLower(const_cast<char*>(s.c_str())));

                ++cursorPos;
            }
        }
    }
    else if (keyCode >= Key::Code::Key0 && keyCode <= Key::Code::Key9)
    {
        if (isShift)
        {

        }
        else
        {
            AZStd::string s = Key::ToString(keyCode);
            s = s.substr(3);
            text.append(s.c_str());

            ++cursorPos;
        }
    }
}

void KeyBoard::OperateKeyUp(Key::Code keyCode)
{
    if (keyCode == Key::Code::LeftShift || keyCode == Key::Code::RightShift)
        isShift = !isShift;
    else if (keyCode == Key::Code::LeftControl || keyCode == Key::Code::RightControl)
        isCtrl = false;
    
}
*/
NS_FGUI_END
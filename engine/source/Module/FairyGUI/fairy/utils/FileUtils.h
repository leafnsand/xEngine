#pragma once

#include "FairyGUIMacros.h"
#include "base/Data.h"
#include <AzCore/std/string/string.h>

NS_FGUI_BEGIN

class FileUtils
{
    FUI_Declare_Singleton(FileUtils)
public:
    enum class Status
    {
        OK = 0,
        NotExists = 1, // File not exists
        OpenFailed = 2, // Open file failed.
        ReadFailed = 3, // Read failed
        NotInitialized = 4, // FileUtils is not initializes
        TooLarge = 5, // The file is too large (great than 2^32-1)
        ObtainSizeFailed = 6 // Failed to obtain the file size.
    };

    virtual Status loadAsset(const AZStd::string& path, Data& data);

    virtual Status loadAsset(const AZStd::string& path, AZStd::string& str);

    bool isFileExist(const AZStd::string& filename);

    AZStd::string getFileExtension(const AZStd::string& filePath)
    {
        AZStd::string s = filePath;
        char * p = (char*)s.c_str();
        char *ext = strrchr(p, '.');
        if (ext)
        {
            *ext = '\0';
            ext++;
        }
        return ext;
    }
};

NS_FGUI_END

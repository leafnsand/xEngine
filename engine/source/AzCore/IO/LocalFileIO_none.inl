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

namespace AZ
{
    namespace IO
    {
        bool LocalFileIO::IsDirectory(const char* filePath)
        {
            return false;
        }

        Result LocalFileIO::Copy(const char* sourceFilePath, const char* destinationFilePath)
        {
            return ResultCode::Success;
        }

        Result LocalFileIO::FindFiles(const char* filePath, const char* filter, FindFilesCallbackType callback)
        {
            return ResultCode::Success;
        }

        Result LocalFileIO::CreatePath(const char* filePath)
        {
            return ResultCode::Success;
        }

        bool LocalFileIO::IsAbsolutePath(const char* path) const
        {
            return path && path[0] == '/';
        }

        bool LocalFileIO::ConvertToAbsolutePath(const char* path, char* absolutePath, AZ::u64 maxLength) const
        {
            return true;
        }
    } // namespace IO
} // namespace AZ

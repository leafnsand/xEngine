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
#if !defined(AZCORE_EXCLUDE_LUA)

#include <stdlib.h>

// Functions that Lua expects to be externally defined
float script_frand0_1()
{
    return (float)rand() / (float)(RAND_MAX);
}

void script_randseed(unsigned int seed)
{
    srand(seed);
}

// AZCORE_COMPILES_LUA means Lua source is compiled by AzCore.
// If undefined then Lua must be in a linked library.
#if defined (AZCORE_COMPILES_LUA)

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable:4324) // structure was padded due to __declspec(align())
#   pragma warning(disable:4702) // unreachable code
#endif

// Note: First merge with the CryLua, we should probably remove loslib, print, etc. libs that we already provide functionality for

// Handle CryTek defines


#if defined(__ANDROID__)
#   ifndef ANDROID
#       define ANDROID
#   endif
#endif

#if defined(__APPLE__)
#   ifndef APPLE
#       define APPLE
#   endif
#endif

#if defined(__linux__)
#   ifndef LINUX
#       define LINUX
#   endif
#endif

#   include <lua/lapi.c>
#   include <lua/lauxlib.c>
#   include <lua/lbaselib.c>
#   include <lua/lbitlib.c>
#   include <lua/lcode.c>
#   include <lua/ldblib.c>
#   include <lua/ldo.c>
#   include <lua/ldump.c>
#   include <lua/lfunc.c>
#   include <lua/lgc.c>
#   include <lua/linit.c>
#   include <lua/liolib.c>
#   include <lua/llex.c>
#   include <lua/lmathlib.c>
#   include <lua/lmem.c>
#   include <lua/loadlib.c>
#   include <lua/lobject.c>
#   include <lua/lopcodes.c>
#   include <lua/lparser.c>
#   include <lua/lstate.c>
#   include <lua/lstring.c>
#   include <lua/lstrlib.c>
#   include <lua/ltable.c>
#   include <lua/ltablib.c>
#   include <lua/ltm.c>
#   include <lua/lundump.c>
#   include <lua/lvm.c>
#   include <lua/lzio.c>
#   include <lua/print.c>
#   include <lua/ldebug.c>

#define LUA_TMPNAMBUFSIZE   L_tmpnam
#define lua_tmpnam(b, e)     { e = (tmpnam(b) == NULL); }
#   include <lua/loslib.c>

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

#endif // AZCORE_COMPILES_LUA
#endif // AZCORE_EXCLUDE_LUA

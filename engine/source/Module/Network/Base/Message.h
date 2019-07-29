#pragma once

#include <AzCore/std/containers/vector.h>

#define SERVER_BIG_ENDIAN (0)

#if defined(AZ_BIG_ENDIAN) && SERVER_BIG_ENDIAN
#define SWAP_ENDIAN(value)
#elif !defined(AZ_BIG_ENDIAN) && !SERVER_BIG_ENDIAN
#define SWAP_ENDIAN(value)
#else
#include <AzCore/std/algorithm.h>
#define SWAP_ENDIAN(value) AZStd::endian_swap(value)
#endif

namespace Module
{
    template<typename T>
    size_t Encoder(char **out, T value)
    {
        if (out && *out)
        {
            SWAP_ENDIAN(value);
            *(T*)*out = value;
            *out += sizeof(T);
            return sizeof(T);
        }
        return 0;
    }

    template<typename T>
    size_t Decoder(const char **in, T &value)
    {
        if (in && *in)
        {
            value = *(T*)*in;
            *in += sizeof(T);
            SWAP_ENDIAN(value);
            return sizeof(T);
        }
        return 0;
    }

    struct Message
    {
        AZ::u32 flag{ 0 };
        AZ::u32 length{ 0 };
        AZ::u32 rpc{ 0 };
        AZ::u32 id{ 0 };
        AZStd::vector<char> data;

        AZ::u32 size() const { return length + sizeof(AZ::u32) * 4; }

        bool Encode(char *buffer, size_t buffer_length);

        bool Decode(const char *buffer, size_t buffer_length);
    };
}
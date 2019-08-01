#include "Message.h"

namespace Module
{
    bool Message::Encode(char *buffer, size_t buffer_length)
    {
        if (buffer_length < size()) return false;
        auto encode_length = Encoder(&buffer, flag);
        encode_length += Encoder(&buffer, length);
        encode_length += Encoder(&buffer, rpc);
        encode_length += Encoder(&buffer, id);
        if (length > 0)
        {
            memcpy(buffer, data.data(), data.size());
            buffer += length;
            encode_length += length;
        }
        return true;
    }

    bool Message::Decode(const char *buffer, size_t buffer_length)
    {
        if (buffer_length < size()) return false;
        auto decoded_length = Decoder(&buffer, flag);
        decoded_length += Decoder(&buffer, length);
        decoded_length += Decoder(&buffer, rpc);
        decoded_length += Decoder(&buffer, id);
        if (length + decoded_length > buffer_length) return false;
        if (length > 0)
        {
            data.resize(length);
            memcpy(data.data(), buffer, length);
            buffer += length;
            decoded_length += length;
        }
        return true;
    }
}
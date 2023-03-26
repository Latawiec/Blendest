#pragma once

#include <cstdint>
#include <cstddef>

namespace Network {

struct Buffer
{
    uint8_t          type = 0;
    const std::byte* data = nullptr;
    uint64_t         size = 0;
};

}
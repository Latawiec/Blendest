#pragma once

#include <string>

namespace Network {

struct Error
{
    int         code = 0;
    std::string message;
    operator bool() const
    {
        return code != 0;
    }
};

}
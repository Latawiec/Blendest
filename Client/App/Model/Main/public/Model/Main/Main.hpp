#pragma once

#include <Model/Server/Connection.hpp>

namespace Model {
namespace Main {
    
class Main {

public:
    Main();

    Server::Connection Connection;
};

} // namespace Main
} // namespace Model
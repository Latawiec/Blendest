#pragma once

#include <Model/Server/Connection.hpp>
#include <Model/Files/FileManager.hpp>

namespace Model {
namespace Main {
    
class Main {

public:
    Main();

    Server::Connection Connection;
    Files::FileManager FileManager;
};

} // namespace Main
} // namespace Model
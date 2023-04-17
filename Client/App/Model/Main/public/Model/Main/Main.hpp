#pragma once

#include <Model/Server/Connection.hpp>
#include <Model/Files/FileManager.hpp>
#include <Model/Blender/Settings.hpp>

namespace Model {
namespace Main {
    
class Main {

public:
    Main();

    Server::Connection Connection;
    Files::FileManager FileManager;
    Blender::Settings BlenderSettings;
};

} // namespace Main
} // namespace Model
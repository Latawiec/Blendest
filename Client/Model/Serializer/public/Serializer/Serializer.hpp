#pragma once

#include <Model/Main.hpp>

#include <string>

namespace Model {

class Serializer {
public:
    static bool writeConfigFile(const Model::Main& model);
    static bool readConfigFile(Model::Main& model);
};

}
#pragma once

#include <Model/Files/FileManager.hpp>
#include <View/View/Main.hpp>

namespace ViewModel {
namespace Serializer {

class Serializer {

    const Model::Files::FileManager& _fileManager;

public:
    Serializer(const Model::Files::FileManager& fileManager);

    bool Serialize(const View::View::Main& mainView) const ;
    bool Deserialize(View::View::Main& mainView) const ;
};

} // Serializer
} // ViewModel
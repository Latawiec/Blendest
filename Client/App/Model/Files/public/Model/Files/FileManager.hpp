#pragma once

#include <filesystem>

namespace Model {
namespace Files {

class FileManager {

    const std::filesystem::path _exeDir;
    std::filesystem::path _tmpStorageDir;
    std::filesystem::path _configStorageDir;

public:
    FileManager();

    const std::filesystem::path& GetTmpStorageDir() const;
    const std::filesystem::path& GetConfigStorageDir() const;
};

}
}
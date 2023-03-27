#include "public/Model/Files/FileManager.hpp"

#include <boost/dll/runtime_symbol_info.hpp>

#include <filesystem>

namespace Model {
namespace Files {

namespace /* anonymous */ {
    void createDirIfNotFound(const std::filesystem::path& dirPath) {
        std::filesystem::file_status dirStatus = std::filesystem::status(dirPath);
        if (dirStatus.type() == std::filesystem::file_type::not_found || 
            dirStatus.type() != std::filesystem::file_type::directory )
        {
            std::filesystem::create_directory(dirPath);
        }
    }
}


FileManager::FileManager()
: _exeDir(boost::dll::program_location().parent_path().string())
, _tmpStorageDir(_exeDir / "tmp")
, _configStorageDir(_exeDir / "config")
{
    createDirIfNotFound(_tmpStorageDir);
    createDirIfNotFound(_configStorageDir);
}

const std::filesystem::path& FileManager::GetTmpStorageDir() const
{
    return _tmpStorageDir;
}

const std::filesystem::path& FileManager::GetConfigStorageDir() const
{
    return _configStorageDir;
}

}
}
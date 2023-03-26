#include "public/Serializer/Serializer.hpp"

#include <Model/Settings.hpp>
#include <Model/Connection.hpp>

#include <nlohmann/json.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

#include "Fields.hpp"

#include <fstream>

namespace Model {

namespace /* anonymous*/
{
using json = nlohmann::json;

json parse(const Model::Connection& model) {
    const auto& data = model.getData();
    json output;
    output[FieldName::HostServerAddress] = data.hostAddress;
    output[FieldName::HostServerPort] = data.hostPort;

    return output;
}

json parse(const Model::Settings& model) {
    const auto& data = model.getData();
    json output;
    output[FieldName::ThreadCount] = data.threadsCount;
    output[FieldName::BlenderDirPath] = data.blenderPath;

    return output;
}
}

bool Serializer::writeConfigFile(const Model::Main& model)
{
    const std::filesystem::path programDir(boost::dll::program_location().parent_path().string());
    const std::filesystem::path configFile = programDir / "config.json";
    std::ofstream ofs(configFile);

    if (!ofs.is_open()) {
        return false;
    }

    json output;
    output[FieldName::Connection] = parse(model.connectionModel);
    output[FieldName::Settings] = parse(model.settingsModel);

    ofs << nlohmann::to_string(output);
    ofs.close();

    return true;
}

bool Serializer::readConfigFile(Model::Main& model)
{
    const std::filesystem::path programDir(boost::dll::program_location().parent_path().string());
    const std::filesystem::path configFile = programDir / "config.json";
    std::ifstream ifs(configFile);

    if (!ifs.is_open()) {
        return false;
    }
    const json data = json::parse(ifs);
    ifs.close();

    // Connection
    {
        const auto& connection = data[FieldName::Connection];
        Model::Connection::Data newData;
        newData.hostAddress = connection[FieldName::HostServerAddress].get<std::string>();
        newData.hostPort = connection[FieldName::HostServerPort].get<std::string>();

        model.connectionModel.setData(std::move(newData));
    }

    // Settings
    {
        const auto& settings = data[FieldName::Settings];
        Model::Settings::Data newData;
        newData.threadsCount = settings[FieldName::ThreadCount].get<int>();
        newData.blenderPath = settings[FieldName::BlenderDirPath].get<std::string>();

        model.settingsModel.setData(std::move(newData));
    }
    return true;
}

}
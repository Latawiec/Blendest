#include "public/ViewModel/Serializer/Serializer.hpp"

#include <View/Component/Settings.hpp>
#include <View/Component/Connection.hpp>

#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace ViewModel {
namespace Serializer {

namespace /* anonymous*/
{
    const std::string ConfigFileName = "serializedState.json";

namespace FieldNames{

// Settings
    const std::string Settings = "settings";
    const std::string BlenderPath = "blenderPath";
    const std::string ThreadsCount = "threadsCount";

// Connection
    const std::string Connection = "connection";
    const std::string HostAddress = "hostAddress";
    const std::string HostPort = "hostPort";
}

namespace Serialize {

    void serialize(json& output, const View::Component::Settings& view)
    {
        json obj;
        obj[FieldNames::BlenderPath] = view.GetBlenderDir();
        obj[FieldNames::ThreadsCount] = view.GetThreadsSelected();

        output[FieldNames::Settings] = std::move(obj);
    }

    void serialize(json& output, const View::Component::Connection& view)
    {
        json obj;
        obj[FieldNames::HostAddress] = view.GetHost();
        obj[FieldNames::HostPort] = view.GetPort();

        output[FieldNames::Connection] = std::move(obj);
    }

}

namespace Deserialize {

    void deserialize(View::Component::Settings& output, const json& object)
    {
        if (!object.contains(FieldNames::Settings)) {
            return;
        }

        const auto& settingsObj = object[FieldNames::Settings];
        if (settingsObj.contains(FieldNames::BlenderPath)) {
            output.SetBlenderDir(settingsObj[FieldNames::BlenderPath].get<std::string>());
        }
        if (settingsObj.contains(FieldNames::ThreadsCount)) {
            output.SetThreadsSelected(settingsObj[FieldNames::ThreadsCount].get<int>());
        }
    }

    void deserialize(View::Component::Connection& output, const json& object)
    {
        if (!object.contains(FieldNames::Connection)) {
            return;
        }

        const auto& connectionObj = object[FieldNames::Connection];
        if (connectionObj.contains(FieldNames::HostAddress)) {
            output.SetHost(connectionObj[FieldNames::HostAddress].get<std::string>());
        }
        if (connectionObj.contains(FieldNames::HostPort)) {
           output.SetPort(connectionObj[FieldNames::HostPort].get<std::string>()); 
        }
    }

}

} // namespace anonymous

Serializer::Serializer(const Model::Files::FileManager& fileManager)
: _fileManager(fileManager)
{}

bool Serializer::Serialize(const View::View::Main& mainView) const
{
    const std::filesystem::path configFile = _fileManager.GetConfigStorageDir() / ConfigFileName;
    std::ofstream ofs(configFile);

    if (!ofs.is_open()) {
        return false;
    }

    json output;
    Serialize::serialize(output, mainView.components.connection);
    Serialize::serialize(output, mainView.components.settings);

    ofs << nlohmann::to_string(output);
    ofs.close();

    return true;
}

bool Serializer::Deserialize(View::View::Main& mainView) const
{
    const std::filesystem::path configFile = _fileManager.GetConfigStorageDir() / ConfigFileName;
    std::ifstream ifs(configFile);

    if (!ifs.is_open()) {
        return false;
    }

    json object;
    try {
        object = json::parse(ifs);
    } catch (...) {
        return false;
    }

    Deserialize::deserialize(mainView.components.connection, object);
    Deserialize::deserialize(mainView.components.settings, object);

    return true;
}

}
}
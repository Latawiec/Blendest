#include "public/Model/Blender/Handle.hpp"

#include <System/Process.hpp>

#include <future>
#include <regex>
#include <filesystem>
#include <sstream>

namespace Model::Blender {

std::string Handle::Version::string() const
{
    std::stringstream ss;
    ss << major << '.' << minor << '.' << patch;
    return ss.str();
}

Handle::Handle(std::filesystem::path blenderPath, Version version)
: _blenderPath(std::move(blenderPath))
, _version(std::move(version))
{}

Handle::Handle(Handle&& other)
{
    std::swap(_blenderPath, other._blenderPath);
    std::swap(_version, other._version);
}

Handle& Handle::operator=(Handle&& other)
{
    std::swap(_blenderPath, other._blenderPath);
    std::swap(_version, other._version);
    return *this;
}

const Handle::Version& Handle::GetVersion() const
{
    return _version;
}

namespace /* anonymous */ {
const std::regex BlenderVersionRegex("Blender (\\d)\\.(\\d)\\.(\\d+)");
}

std::optional<Handle> Handle::TryCreateHandle(const std::string& blenderPathStr)
{
    

    if (blenderPathStr.empty()) {
        return std::nullopt;
    }

    std::filesystem::path path{ blenderPathStr };

    if (!std::filesystem::exists(path)) {
        return std::nullopt;
    }

    if (std::filesystem::is_directory(path)) {
        path = path / "blender.exe";
        if (!std::filesystem::exists(path)) {
            return std::nullopt;
        }
    }

    std::filesystem::file_status status = std::filesystem::status(path);
    if ((status.permissions() & std::filesystem::perms::owner_exec) == std::filesystem::perms::none) {
        return std::nullopt;
    }

    System::Process process{ path.string(), std::vector<std::string>{ "--version "} };
    std::future<int>& ret = process.Start();

    if ((ret.wait_for(5s) == std::future_status::ready) && ret.get() == 0) {
        std::string output = process.ReadStdOut();
        std::smatch m;

        if (std::regex_search(output, m, BlenderVersionRegex)) {
            Version v {
                .major = std::stoi(m[1].str()),
                .minor = std::stoi(m[2].str()),
                .patch = std::stoi(m[3].str())
            };

            return Handle(path, v);
        }
    }

    return std::nullopt;
}

}
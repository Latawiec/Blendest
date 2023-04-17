#pragma once

#include <filesystem>

using namespace std::chrono_literals;

namespace Model {
namespace Blender {

class Handle {
public:
    struct Version {
        int major, minor, patch;
        std::string string() const;
    };

private:
    std::filesystem::path _blenderPath;
    Version _version;

    Handle(std::filesystem::path blenderPath, Version version);
public:
    Handle(Handle&&);
    Handle& operator=(Handle&&);

    static std::optional<Handle> TryCreateHandle(const std::string& blenderPath);

    const Version& GetVersion() const;
    const std::filesystem::path& GetPath() const;
};

} // namespace Blender
} // namespace Model    
#pragma once

#include "Handle.hpp"

#include <optional>

namespace Model {
namespace Blender {

class Settings {

    struct {
        std::string deviceOverride;
    } _cycesPreferenceOverride;

    int _threadsCount = 0;
    std::optional<Model::Blender::Handle> _handle;

public:

    void ResetHandle(const std::string& newBlenderPath);
    void SetThreadsCount(int threadsCount);

    std::optional<Model::Blender::Handle>& GetHandle();
    int GetThreadsCount();
};

}
}
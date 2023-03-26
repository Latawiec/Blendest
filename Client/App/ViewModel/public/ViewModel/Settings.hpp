#pragma once

#include <Model/Blender/Handle.hpp>
#include <View/Component/Settings.hpp>

#include <nod.hpp>

namespace ViewModel {

class Settings {

    std::optional<Model::Blender::Handle> _blenderHandle;
    View::Component::Settings& _view;

    struct {
        nod::connection onThreadsChangedHandle;

    } _threadsSetting;

    struct {
        nod::connection onBlenderDirChangedHandle;
    } _blenderDirSetting;

public:
    Settings(View::Component::Settings& view);

private:
    void onThreadsChanged(int threadsCount);
    void onBlenderDirChanged(const std::string& blenderDir);
};

}
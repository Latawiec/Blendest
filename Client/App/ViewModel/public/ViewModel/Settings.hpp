#pragma once

#include <Model/Blender/Settings.hpp>
#include <View/Component/Settings.hpp>

#include <nod.hpp>

namespace ViewModel {

class Settings {

    Model::Blender::Settings& _model;
    View::Component::Settings& _view;

    struct {
        nod::connection onThreadsChangedHandle;

    } _threadsSetting;

    struct {
        nod::connection onBlenderDirChangedHandle;
    } _blenderDirSetting;

public:
    Settings(Model::Blender::Settings& model, View::Component::Settings& view);

private:
    void onThreadsChanged(int threadsCount);
    void onBlenderDirChanged(const std::string& blenderDir);
};

}
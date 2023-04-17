#include "public/ViewModel/Settings.hpp"

#include <thread>

namespace ViewModel {

Settings::Settings(Model::Blender::Settings& model, View::Component::Settings& view)
: _model(model)
, _view(view)
{
    _threadsSetting.onThreadsChangedHandle = _view.OnThreadsChanged([&](const int threadsCount) {
        onThreadsChanged(threadsCount);
    });

    _blenderDirSetting.onBlenderDirChangedHandle = _view.OnBlenderDirChanged([&](const std::string& dir) {
        onBlenderDirChanged(dir);
    });

    _view.SetMaxThreads(std::thread::hardware_concurrency());
    _view.SetThreadsSelected(std::thread::hardware_concurrency());
}

void Settings::onThreadsChanged(int threadsCount)
{

}

void Settings::onBlenderDirChanged(const std::string& blenderDir)
{
    _model.ResetHandle(blenderDir);
    auto& blenderHandleOpt = _model.GetHandle();

    if (!blenderHandleOpt.has_value()) {
        _view.SetBlenderDirStatus("Wrong path");
    } else {
        const auto& version = blenderHandleOpt.value().GetVersion();
        _view.SetBlenderDirStatus(version.string());
    }
}

}
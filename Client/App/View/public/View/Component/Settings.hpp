#pragma once

#include <string>
#include <memory>
#include <functional>

#include <nod.hpp>

namespace ftxui {
class ComponentBase;
}

namespace View {
namespace Component {

class Settings {
    using Component = std::shared_ptr<ftxui::ComponentBase>;

// Threads settings
    int _maxThreadsAvailable = 1;
    int _threadsSelected = 1;
    int _threadsCount = 1;
    Component _threadSlider;
    Component _threadsSetting;

// Blender path settings
    std::string _blenderDirInputStr;
    std::string _blenderDirStatusStr = "...";
    Component _blenderDirStatus;
    Component _blenderDirInput;
    Component _blenderDirSetting;

    Component _component;

    using ThreadsSettingCallbackSignature = void(const int threadsCount);
    using BlenderDirCallbackSignature = void(const std::string&);

    nod::signal<ThreadsSettingCallbackSignature> _threadsSignal;
    nod::signal<BlenderDirCallbackSignature> _blenderDirSignal;

public:
    Settings();
    Component GetComponent();

// Threads
    void SetMaxThreads(int maxThreads);
    void SetThreadsSelected(int threads);
    int GetThreadsSelected() const;

// Blender Dir
    void SetBlenderDir(const std::string& blenderDir);
    void SetBlenderDirStatus(const std::string& status);
    const std::string& GetBlenderDir() const;


    using ThreadsSettingCallbackT = decltype(_threadsSignal)::slot_type;
    using BlenderDirSettingCallbackT = decltype(_blenderDirSignal)::slot_type;
    [[nodiscard]] nod::connection OnThreadsChanged(ThreadsSettingCallbackT&&);
    [[nodiscard]] nod::connection OnBlenderDirChanged(BlenderDirSettingCallbackT&&);

private:
    void threadsSelectChanged();
    void blenderDirChanged();

    Component threadsSlider();
    Component threadsSetting();

    Component blenderDirInput();
    Component blenderDirStatus();
    Component blenderDirSetting();
};


} // namespace Component
} // namespace View
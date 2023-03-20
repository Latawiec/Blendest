#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include <functional>

namespace ftxui {
class ComponentBase;
}

namespace UI {
namespace Component {

class Settings {
public:
    using ThreadsCountChangeCallbackT = std::function<void(int)>;
    using BlenderDirChangeCallbackT = std::function<void(const std::string&)>;

private:
    using Component = std::shared_ptr<ftxui::ComponentBase>;
    
    // Threads settings
    int _maxThreadsAvailable = 12;
    int _threadsSelected = 1; // Currently selected
    int _threadsCount = 1; // Currently published. Just for diff so I can pop OnChange.
    Component _threadSlider;

    // Blender path settings
    std::filesystem::path _blenderDirPath;
    std::string _blenderDirInputStr;
    Component _blenderDirInput;
    std::string _blenderDirStatusStr = "...";
    Component _blenderDirStatus;
    bool _isBlenderDirCorrect = false;

    Component _settingsContainer;
    Component _component;

    ThreadsCountChangeCallbackT _onThreadsCountChange;
    BlenderDirChangeCallbackT _onBlenderDirChange;

public:
    Settings();
    Component getComponent();

    void setOnBlenderDirChange(BlenderDirChangeCallbackT cb, bool invokeAfterSet = true);
    void setOnThreadsCountChange(ThreadsCountChangeCallbackT cb, bool invokeAfterSet = true);

    void setMaxThreadsCount(const int count);
    void setUtilizedThreadsCount(const int count);

    void setBlenderDir(const std::string& path);
    void setBlenderDirStatus(const std::string& status);
};

}
}
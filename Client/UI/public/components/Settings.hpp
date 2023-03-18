#pragma once

#include <filesystem>
#include <string>
#include <memory>

namespace ftxui {
class ComponentBase;
}

namespace UI {
namespace Component {

class Settings {
    using Component = std::shared_ptr<ftxui::ComponentBase>;
    
    // Threads settings
    const int _maxThreadsAvailable;
    int _threadsSelected;
    Component _threadSlider;

    // Blender path settings
    std::filesystem::path _blenderDirPath;
    std::string _blenderDirInputStr;
    Component _blenderDirInput;
    bool _isBlenderDirCorrect = false;

    Component _settingsContainer;
    Component _component;

public:
    Settings();
    Component getComponent();

    int getThreadsCount() const;
};

}
}
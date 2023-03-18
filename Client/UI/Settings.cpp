#include "./public/components/Settings.hpp"

#include "ftxui/dom/elements.hpp"
#include "ftxui/component/component.hpp"

#include <string>
#include <thread>

using namespace ftxui;

namespace UI {
namespace Component {

Settings::Settings()
: _maxThreadsAvailable(std::thread::hardware_concurrency())
, _threadsSelected(std::thread::hardware_concurrency())
{
    _threadSlider = Slider("", &_threadsSelected, 1, _maxThreadsAvailable, 1);
    InputOption blenderDirInputOption;
    blenderDirInputOption.on_change = [&] {
        if (_onBlenderDirChange) {
            _onBlenderDirChange(_blenderDirInputStr);
        }
    };
    _blenderDirInput = Input(&_blenderDirInputStr, "...Input Blender directory...", blenderDirInputOption);

    auto threadsSetting = Renderer(_threadSlider, [&] {
        if (_threadsCount != _threadsSelected) {
            if (_onThreadsCountChange) {
                _threadsCount = _threadsSelected;
                _onThreadsCountChange(_threadsSelected);
            }
        }
        return vbox({
            hbox({ text("Threads count:"), filler() }),
            hbox({ _threadSlider->Render() | flex_grow, text(std::to_string(_threadsSelected) + '/' + std::to_string(_maxThreadsAvailable)) | align_right | size(WIDTH, EQUAL, 5) }),
        });
    });


    _blenderDirStatus = Renderer([&]{
        return text(std::string("[") + _blenderDirStatusStr + "]");
    });

    auto blenderDirSetting = Container::Vertical({
        Renderer(_blenderDirStatus, [&]{ return hbox({ text("Blender path:"), filler(), _blenderDirStatus->Render()}); }),
        Renderer(_blenderDirInput, [&]{ return hbox({ _blenderDirInput->Render() }); })
    });

    _settingsContainer = Container::Vertical({
        threadsSetting,
        Renderer([]{ return separator(); }),
        blenderDirSetting
    });

    _component = Renderer(_settingsContainer, [&] {
        return window(
            text("Settings") | hcenter | bold,
            _settingsContainer->Render()
        ) | size(WIDTH, EQUAL, 50);
    });
}

ftxui::Component Settings::getComponent()
{
    return this->_component;
}

void Settings::setBlenderDirStatus(const std::string& status) {
    this->_blenderDirStatusStr = status;
}

void Settings::setOnBlenderDirChange(BlenderDirChangeCallbackT cb, bool invokeAfterSet) {
    _onBlenderDirChange = cb;
    if (invokeAfterSet) {
        _onBlenderDirChange(_blenderDirInputStr);
    }
}

void Settings::setOnThreadsCountChange(ThreadsCountChangeCallbackT cb, bool invokeAfterSet) {
    _onThreadsCountChange = cb;
    if (invokeAfterSet) {
        _onThreadsCountChange(_threadsCount);
    }
}

}
}
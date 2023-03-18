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
    _blenderDirInput = Input(&_blenderDirInputStr, "...");

    auto threadsSetting = Renderer(_threadSlider, [&] {
        return vbox({
            hbox({ text("Threads count:"), filler() }),
            hbox({ _threadSlider->Render() | flex_grow, text(std::to_string(_threadsSelected) + '/' + std::to_string(_maxThreadsAvailable)) | align_right | size(WIDTH, EQUAL, 5) }),
        });
    });

    auto blenderDirSetting = Renderer(_blenderDirInput, [&] {
        return vbox({
            hbox({ text("Blender path:")}),
            hbox({ _blenderDirInput->Render() })
        });
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

}
}
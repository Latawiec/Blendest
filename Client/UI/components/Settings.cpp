#include "./public/Settings.hpp"

#include <string>
#include <thread>

using namespace ftxui;

namespace UI {

Settings::Settings()
: _maxThreadsAvailable(std::thread::hardware_concurrency())
, _threadsSelected(std::thread::hardware_concurrency())
{
    _slider = Slider("", &_threadsSelected, 1, _maxThreadsAvailable, 1);
    _component = Renderer(_slider, [&] {
        return window(
            text("Settings") | hcenter | bold,
            vbox({
                hbox({ text("Threads count:"), filler(), text(std::to_string(_threadsSelected) + '/' + std::to_string(_maxThreadsAvailable)) }),
                hbox({ _slider->Render() }),
                separator()
            })
        ) | size(WIDTH, GREATER_THAN, 40);
    });
}

ftxui::Component Settings::getComponent()
{
    return this->_component;
}

}
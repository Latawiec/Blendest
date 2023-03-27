#include "public/View/Component/Settings.hpp"

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include <string>

using namespace ftxui;

namespace View {
namespace Component {

Settings::Settings()
{
    _threadSlider = threadsSlider();
    _threadsSetting = threadsSetting();

    _blenderDirInput = blenderDirInput();
    _blenderDirStatus = blenderDirStatus();
    _blenderDirSetting = blenderDirSetting();

    _component = Container::Vertical({
        _threadsSetting,
        Renderer([]{ return separator(); }),
        _blenderDirSetting
    });
}

ftxui::Component Settings::GetComponent()
{
    return Renderer(_component, [&] {
        return window(
            text("Settings") | hcenter | bold,
            _component->Render()
        ) | size(WIDTH, EQUAL, 50);
    });
}

void Settings::SetMaxThreads(int maxThreads)
{
    _maxThreadsAvailable = maxThreads;
}

void Settings::SetThreadsSelected(int threads)
{
    _threadsSelected = std::min(_maxThreadsAvailable, threads);
    _threadsCount = std::min(_maxThreadsAvailable, threads);
    threadsSelectChanged();
}

int Settings::GetThreadsSelected() const
{
    return _threadsSelected;
}

void Settings::SetBlenderDir(const std::string& blenderDir)
{
    _blenderDirInputStr = blenderDir;
    blenderDirChanged();
}

void Settings::SetBlenderDirStatus(const std::string& status)
{
    _blenderDirStatusStr = status;
}

const std::string& Settings::GetBlenderDir() const 
{
    return _blenderDirInputStr;
}

nod::connection Settings::OnThreadsChanged(ThreadsSettingCallbackT&& cb)
{
    return _threadsSignal.connect(std::move(cb));
}

nod::connection Settings::OnBlenderDirChanged(BlenderDirSettingCallbackT&& cb)
{
    return _blenderDirSignal.connect(std::move(cb));
}

ftxui::Component Settings::threadsSlider()
{
    return Slider("", &_threadsSelected, 1, &_maxThreadsAvailable, 1);
}

ftxui::Component Settings::threadsSetting()
{
    return Renderer(_threadSlider, [&] {
        if (_threadsCount != _threadsSelected) {
            _threadsCount = std::min(_threadsSelected, _maxThreadsAvailable);
            threadsSelectChanged();
        }

        return vbox({
            hbox({ text("Threads count: "), filler() }),
            hbox({ _threadSlider->Render() | flex_grow, text(std::to_string(_threadsSelected) + '/' + std::to_string(_maxThreadsAvailable)) | align_right | size(WIDTH, EQUAL, 5) })
        });
    });
}

ftxui::Component Settings::blenderDirInput()
{
    InputOption blenderDirInputOption;
    blenderDirInputOption.on_change = [&] {
        blenderDirChanged();
    };
    return Input(&_blenderDirInputStr, "...Input Blender directory...", blenderDirInputOption);
}

ftxui::Component Settings::blenderDirStatus()
{
    return Renderer([&]{ return text(std::string("[") + _blenderDirStatusStr + "]"); });
}

ftxui::Component Settings::blenderDirSetting()
{
    return Container::Vertical({
        Renderer(_blenderDirStatus, [&]{ return hbox({ text("Blender path:"), filler(), _blenderDirStatus->Render()}); }),
        Renderer(_blenderDirInput, [&]{ return hbox({ _blenderDirInput->Render() }); })
    });
}


void Settings::threadsSelectChanged()
{
    _threadsSignal(_threadsSelected);
}

void Settings::blenderDirChanged()
{
    _blenderDirSignal(_blenderDirInputStr);
}



} // namespace Component
} // namespace View
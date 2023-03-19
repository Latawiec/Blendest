#pragma once

#include <components/Settings.hpp>
#include <Process.hpp>

namespace Model {

class Settings {

    UI::Component::Settings& _view;
    std::optional<System::Process> opProcess = std::nullopt;
public:
    Settings(UI::Component::Settings& view);
};

}
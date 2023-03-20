#pragma once

#include <UI/Component/Settings.hpp>
#include <System/Process.hpp>

namespace Model {

class Settings {
public:
    struct Data {
        uint16_t threadsCount;
        std::string blenderPath;
    };

private:
    UI::Component::Settings& _view;
    std::optional<System::Process> opProcess = std::nullopt;

    Data _data;

public:
    Settings(UI::Component::Settings& view);

    const Data& getData() const;
    void setData(Data newData);
};

}
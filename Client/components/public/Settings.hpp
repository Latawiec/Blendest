#pragma once
#include "ftxui/component/component.hpp"  // for Slider, Checkbox, Vertical, Renderer, Button, Input, Menu, Radiobox, Toggle

class Settings {
    const int _maxThreadsAvailable;
    int _threadsSelected;

    ftxui::Component _component;
    ftxui::Component _slider;

public:
    Settings();

    ftxui::Component& getComponent();
};
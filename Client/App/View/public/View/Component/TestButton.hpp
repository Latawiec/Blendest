#pragma once

#include <string>
#include <memory>

#include <nod.hpp>

namespace ftxui {
class ComponentBase;
}

namespace View {
namespace Component {

class TestButton {
    std::shared_ptr<ftxui::ComponentBase> _component;
    nod::signal<void()> _clickSignal;
public:
    TestButton(std::string text);
    std::shared_ptr<ftxui::ComponentBase> GetComponent();

    using ClickCallbackT = decltype(_clickSignal)::slot_type;
    [[nodiscard]] nod::connection OnClick(ClickCallbackT&&);   
};

}
}
#include "public/View/Component/TestButton.hpp"

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

using namespace ftxui;

namespace View {
namespace Component {

TestButton::TestButton(std::string text)
{
    _component = Button(text, [&]{ this->_clickSignal(); });
}

ftxui::Component TestButton::GetComponent()
{
    return _component;
}

nod::connection TestButton::OnClick(ClickCallbackT&& cb)
{
    return _clickSignal.connect(std::move(cb));
}

}
}
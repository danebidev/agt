#pragma once

#include "agt/ui/widgets/label.hpp"

namespace agt::widget {

struct Button {
private:
    ui::Node text;

public:
    uint32_t padding;
    
    Button(std::string s = "Click me!", uint32_t padding_ = 15)
        : text(Label(s)), padding(padding_) {}

    size measure(ui::constraints, ui::Node&, ui::Data&);
    void layout(rect, ui::Node&, ui::Data&);
    void draw(ui::Node&, ui::Data&);

    void set_text(std::string s) {
        text = Label(s);
    }
};

}

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include <agt/ui/draw.hpp>

namespace agt::ui {

// Must be extended to make new UI elements.
// Extended classes should override compute_layout_internal
class UIElement {
protected:
    // Populated when computing the layout
    std::optional<uint32_t> width, height;
    std::optional<uint32_t> x, y;

    bool dirty = true;

    virtual void compute_layout_internal() {};

public:
    // If not set, these are ignored when computing the layout
    std::optional<uint32_t> minimum_width, minimum_height;
    std::optional<uint32_t> maximum_width, maximum_height;

    virtual void compute_layout() {
        if(!dirty)
            return;
        dirty = false;
        compute_layout_internal();
    }
};

typedef std::shared_ptr<UIElement> Element;

class LayoutElement : public UIElement {
protected:
    std::vector<Element> elements;

public:
    void compute_layout() override {
        for(auto elem : elements) {
            elem->compute_layout();
        }
    }

    void add(const Element& element) {
        elements.push_back(element);
    }

    void remove(size_t ind) {
        elements.erase(elements.begin() + ind);
    }

    // TODO: add all container-related methods
    // Maybe just expose the elements vector?
};

class UIRoot : public UIElement {
private:
    Element element;

public:
    void set_root(Element elem) {
        element = elem;
    }

    const draw::DrawCtx& compute_draw_ctx();
};

// TODO: Mainly for testing. Remove later
class Rectangle : public UIElement {
protected:
    void compute_layout_internal() override {};

public:
    Rectangle(uint32_t width_, uint32_t height_) {
        width = width_;
        minimum_width = width_;
        maximum_width = width_;

        height = height_;
        minimum_height = height_;
        maximum_height = height_;
    }
};

class HBox : public LayoutElement {
protected:
    void compute_layout_internal() override;
};

}

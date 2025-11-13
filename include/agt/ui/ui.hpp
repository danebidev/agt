#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace agt::ui {

class LayoutElement {
protected:
    // Populated when computing the layout
    std::optional<uint32_t> width, height;
    std::optional<uint32_t> x, y;

    bool dirty = true;

    // If you are creating a widget, override this
    // If you are instead creating a layout element with other
    // elements inside you might want to override compute_layout
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

typedef std::shared_ptr<LayoutElement> Element;

// TODO: Mainly for testing. Remove
class Rectangle : public LayoutElement {
private:
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
private:
    std::vector<Element> elements;

public:
    void compute_layout() override {
        for(auto& element : elements) {
            element->compute_layout();
        }
    }

    void add(Element& element) {
        elements.push_back(element);
    }

    void remove(size_t ind) {
        elements.erase(elements.begin() + ind);
    }

    // TODO: add all container-related methods
    // Maybe just expose the elements vector?
};

}

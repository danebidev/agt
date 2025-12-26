#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <glm/vec2.hpp>

#include <agt/ui/draw.hpp>

namespace agt::ui {

struct size {
    uint32_t width = 0;
    uint32_t height = 0;

    friend size operator+(size lhs, const size &rhs) {
        lhs.width += rhs.width;
        lhs.height += rhs.height;
        return lhs;
    }

    friend size operator-(size lhs, const size &rhs) {
        lhs.width -= rhs.width;
        lhs.height -= rhs.height;
        return lhs;
    }

    operator glm::vec2() const {
        return { width, height };
    }
};

struct rect {
    // `z` is for depth testing later during rendering
    uint32_t x, y, z;
    struct size size;
};

// Must be extended to make new UI elements.
class UIElement {
protected:
    rect element_rect;

public:
    // Returns the preferred size of the UI component
    virtual size measure(size constraint) const = 0;

    // Indicates to the component its computed position
    virtual void layout(rect rect) {
        element_rect = rect;
    }

    virtual void paint(draw::DrawCtx &draw_ctx) const = 0;

    virtual ~UIElement() = default;
};

using Element = std::shared_ptr<UIElement>;

class UIRoot : public UIElement {
private:
    Element element;
    std::unique_ptr<draw::DrawCtx> draw_ctx;

    size root_size;

protected:
    // These 3 are not really meant to be used by any other element
    size measure(size constraint) const override {
        return element->measure(constraint);
    }

    void layout(rect rect) override {
        element->layout(rect);
    }

    void paint(draw::DrawCtx& ctx) const override {
        element->paint(*draw_ctx);
    }

public:
    glm::vec3 color;

    UIRoot(Element elem, size size) 
        : element(elem),
          root_size(size) {}

    void set_root(Element element_) {
        element = element_;
    }

    void set_size(uint32_t width, uint32_t height) {
        root_size = { width, height };
        if(draw_ctx)
            draw_ctx->update_proj(root_size);
    }

    const draw::DrawCtx& compute_layout();
};

// TODO: Mainly for testing. Remove later
class Rectangle : public UIElement {
    size rect_size;

public:
    Rectangle(uint32_t width, uint32_t height) {
        rect_size = { width, height };
    }

    size measure(size constraint) const override {
        return rect_size;
    }

    void paint(draw::DrawCtx& ctx) const override;
};

class HBox : public UIElement {
public:
    std::vector<Element> elements;
    
    size measure(size constraint) const override;
    void layout(rect rect) override; 
};

}

#pragma once

#include <agt/ui/draw.hpp>
#include <agt/utils.hpp>

#include <memory>

namespace agt::ui {

struct constraints {
    uint32_t min_w, max_w;
    uint32_t min_h, max_h;
};


// Data that will get passed to all elements during layout and drawing
struct Data {
    draw::DrawCtx& draw_ctx;
    struct TextRendering& text_render;
};

struct Node {
    rect layout_rect;

    struct Concept {
        virtual ~Concept() = default;

        virtual size measure(constraints, Node&, Data&) = 0;
        virtual void layout(rect, Node&, Data&) = 0;
        virtual void draw(Node&, Data&) = 0;

        virtual const std::type_info& type() const = 0;
        virtual void* data() = 0;
    };

    template <typename T>
    struct Model final : Concept {
        T value;

        Model(T v) : value(std::move(v)) {}

        size measure(constraints s, Node& n, Data& data) override {
            return value.measure(s, n, data);
        }

        void layout(rect r, Node& n, Data& data) override {
            n.layout_rect = r;
            value.layout(r, n, data);
        }

        void draw(Node& n, Data& data) override {
            value.draw(n, data);
        }

        const std::type_info& type() const override { return typeid(T); }
        void* data() override { return &value; }
    };

    // TODO: small buffer optimizations?
    std::unique_ptr<Concept> self;

    template <typename T>
    Node(T v) : self(std::make_unique<Model<T>>(std::move(v))) {}

    template <typename T>
    T* as() {
        if (typeid(T) == self->type()) {
            return static_cast<T*>(self->data());
        }
        return nullptr;
    }

    size measure(constraints s, Node& n, Data& data) { return self->measure(s, n, data); }
    void layout(rect r, Node& n, Data& data) { self->layout(r, n, data); }
    void draw(Node& n, Data& data) { self->draw(n, data); }
};

struct UIRoot {
    Node& node;
    Data data;
    glm::vec3 bg_color;
    size s;
    draw::DrawCtx draw_ctx;

    UIRoot(Node& n, glm::vec3 bg, size s_, TextRendering& text)
        : node(n),
          data(draw_ctx, text),
          bg_color(bg),
          s(s_),
          draw_ctx(s) {}

    void compute_layout() {
        node.layout({ 0, 0, 1, s }, node, data);
    }

    draw::DrawCtx& draw() {
        draw_ctx.cmds.clear();
        draw_ctx.indices.clear();
        draw_ctx.vertices.clear();
        draw_ctx.clear_color = bg_color;

        node.draw(node, data);
        return draw_ctx;
    }

    void set_size(uint32_t width, uint32_t height) {
        s = { width, height };
        draw_ctx.update_size(s);
    }
};

}

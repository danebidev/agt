#pragma once

#include <cstdint>
#include <memory>
#include <glm/vec2.hpp>

#include <agt/ui/draw.hpp>
#include <agt/utils.hpp>

namespace agt::ui {

struct constraints {
    uint32_t min_w, max_w;
    uint32_t min_h, max_h;
};

struct Node {
    rect layout_rect;

    struct Concept {
        virtual ~Concept() = default;

        virtual size measure(constraints, Node&) const = 0;
        virtual void layout(rect, Node&) const = 0;
        virtual void draw(draw::DrawCtx&, Node&) const = 0;

        virtual const std::type_info& type() const = 0;
        virtual void* data() = 0;
    };

    template <typename T>
    struct Model final : Concept {
        T value;

        Model(T v) : value(std::move(v)) {}

        size measure(constraints s, Node& n) const override {
            return value.measure(s, n);
        }

        void layout(rect r, Node& n) const override {
            n.layout_rect = r;
            value.layout(r, n);
        }

        void draw(draw::DrawCtx& ctx, Node& n) const override {
            value.draw(ctx, n);
        }

        const std::type_info& type() const override { return typeid(T); }
        void* data() override { return &value; }
    };

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

    size measure(constraints s, Node& n) const { return self->measure(s, n); }
    void layout(rect r, Node& n) const { self->layout(r, n); }
    void draw(draw::DrawCtx& ctx, Node& n) const { self->draw(ctx, n); }
};

struct UIRoot {
    Node& node;
    glm::vec3 bg_color;
    size s;
    draw::DrawCtx draw_ctx;

    UIRoot(Node& n, glm::vec3 bg, size s_)
        : node(n), bg_color(bg), s(s_), draw_ctx(s) {}

    void compute_layout() {
        node.layout({ 0, 0, 1, s }, node);
    }

    draw::DrawCtx& draw() {
        draw_ctx.cmds.clear();
        draw_ctx.indices.clear();
        draw_ctx.vertices.clear();
        draw_ctx.clear_color = bg_color;

        node.draw(draw_ctx, node);
        return draw_ctx;
    }

    void set_size(uint32_t width, uint32_t height) {
        s = { width, height };
        draw_ctx.update_size(s);
    }
};

}

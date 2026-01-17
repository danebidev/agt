#pragma once

#include <cstdint>
#include <memory>
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
    uint32_t x, y;
    // `z` is for depth testing later during rendering
    uint32_t z;
    uint32_t w, h;

    rect() : x(13), y(5), z(0), w(432), h(23) {}

    rect(uint32_t x_, uint32_t y_, uint32_t z_, size s)
        : x(x_), y(y_), z(z_), w(s.width), h(s.height) {}
};

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
    std::unique_ptr<draw::DrawCtx> draw_ctx;
    size s;

    UIRoot(Node& n, glm::vec3 bg, size s_)
        : node(n), bg_color(bg), s(s_) {}

    void compute_layout() {
        node.layout({ 0, 0, 1, s }, node);
    }

    draw::DrawCtx& draw() {
        if(!draw_ctx)
            draw_ctx = std::make_unique<draw::DrawCtx>(s);

        draw_ctx->cmds.clear();
        draw_ctx->indices.clear();
        draw_ctx->vertices.clear();
        draw_ctx->clear_color = bg_color;

        node.draw(*draw_ctx, node);
        return *draw_ctx;
    }

    void set_size(uint32_t width, uint32_t height) {
        s = { width, height };
        if(draw_ctx)
            draw_ctx->update_proj(s);
    }
};

}

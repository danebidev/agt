#include <agt/ui/ui.hpp>

namespace agt::ui {

const draw::DrawCtx& UIRoot::compute_draw_ctx() {
    static draw::DrawCtx ctx({ 0, 0 });

    return ctx;
}

}; // namespace agt::ui

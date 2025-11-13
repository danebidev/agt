#include <agt/ui/draw.hpp>

namespace agt::draw {

void DrawList::add_clear(glm::vec3 color) {
    cmds.push_back({ .type = CmdType::CLEAR_SCREEN, .clear_color = color });
}

} // namespace agt::draw

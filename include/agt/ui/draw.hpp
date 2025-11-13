#pragma once

#include <glm/vec3.hpp>
#include <vector>

namespace agt::draw {

enum class CmdType {
    /*
     * The backend should clear the screen to the color set in `clear_color`.
     * Equivalent to OpenGL's glClearColor followed by glClear(GL_COLOR_BUFFER_BIT)
     */
    CLEAR_SCREEN
    // TODO: ...
};

struct DrawCmd {
    // Depending on the value of type, various other fields will be set
    CmdType type;
    
    glm::vec3 clear_color;
    // TODO: ...
};

// Contains cmd_n commands that have to be executed in
// the order they are found in the cmds vector.
// The backend must not modify this
class DrawList {
public:
    std::vector<DrawCmd> cmds;

    void add_clear(glm::vec3 color);
};

} // namespace agt::draw

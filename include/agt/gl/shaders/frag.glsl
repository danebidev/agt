#version 330 core

in vec3 color;
out vec4 fragColor;

uniform mat4 proj;

void main() {
    fragColor = vec4(color, 1.0);
}

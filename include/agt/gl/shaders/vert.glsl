#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;
layout (location = 2) in vec2 uv;
// To avoid branching while still using a single shader
// 1 to use textures, 0 otherwise
layout (location = 3) in float texWeight; 

out vec3 vertColor;
out vec2 vertUV;
out float vertTexWeight;

uniform mat4 proj;

void main() {
    gl_Position = proj * vec4(pos, 1.0);
    vertColor = col;
    vertUV = uv;
    vertTexWeight = texWeight;
}

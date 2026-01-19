#version 330 core

in vec3 vertColor;
in vec2 vertUV;
in float vertTexWeight;

out vec4 fragColor;

uniform sampler2D tex;

void main() {
    vec4 solid = vec4(vertColor, 1.0);
    vec4 texture = texture(tex, vertUV);

    fragColor = mix(solid, texture, vertTexWeight);
}

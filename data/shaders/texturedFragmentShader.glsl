#version 330 compatibility
in vec4 vSmoothColor;
in vec2 vTex;
out vec4 vFragColor;

uniform sampler2D u_texture;

void main()
{
    vFragColor = texture(u_texture, vTex);
}
#version 330 compatibility
in vec4 vSmoothColor;
out vec4 vFragColor;

void main()
{
    vFragColor = vSmoothColor;
}
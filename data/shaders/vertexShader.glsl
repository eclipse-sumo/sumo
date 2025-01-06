#version 330 compatibility
layout(location = 0) in vec3 vVertex;
layout(location = 1) in uvec4 vColor;
out vec4 vSmoothColor;

uniform mat4 u_MVP;

void main()
{
    vSmoothColor = vec4(vColor) / 255.;
    gl_Position = u_MVP * vec4(vVertex, 1.0);
}
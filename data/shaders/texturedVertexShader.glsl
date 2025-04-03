#version 330 compatibility
layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec2 vTex;
out vec4 vSmoothColor;
out vec2 vTexCoord;

uniform mat4 u_MVP;

void main()
{
    vSmoothColor = vColor;
    vTexCoord = vTex;
    gl_Position = u_MVP * vec4(vVertex, 1.0);
}
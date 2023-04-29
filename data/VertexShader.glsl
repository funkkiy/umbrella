#version 460 core

layout (location = 0) in vec3 PositionAttrib;
layout (location = 1) in vec3 VertexColorAttrib;
out vec3 VertexColor;

void main()
{
    gl_Position = vec4(PositionAttrib, 1.0);
    VertexColor = VertexColorAttrib;
}

#version 460 core

layout (location = 0) in vec3 PositionAttrib;
layout (location = 1) in vec3 ColorAttrib;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 Color;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(PositionAttrib, 1.0f);
    Color = ColorAttrib;
}

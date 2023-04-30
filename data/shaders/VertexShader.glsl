#version 460 core

layout (location = 0) in vec3 PositionAttrib;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(PositionAttrib, 1.0);
}

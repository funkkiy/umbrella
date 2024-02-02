#version 460 core

layout (location = 0) in vec3 PositionAttrib;
layout (location = 1) in vec2 TexCoordAttrib;
layout (location = 2) in vec3 NormalAttrib;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 FragPos;
out vec2 TexCoord;
flat out vec3 Normal;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(PositionAttrib, 1.0f);
    FragPos = vec3(uModel * vec4(PositionAttrib, 1.0f));
    TexCoord = TexCoordAttrib;
    Normal = NormalAttrib;
}

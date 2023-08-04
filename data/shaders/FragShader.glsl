#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D diffuseTexture;

void main()
{
    FragColor = texture(diffuseTexture, TexCoord);
}

#version 460 core

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;
out vec4 FragColor;

uniform sampler2D diffuseTexture;

void main()
{
    vec4 objectColor = vec4(1.0f, 0.0f, 0.5f, 1.0f);
    vec3 Normal = normalize(Normal);
    vec3 lightPos = vec3(1.0f, 2.0f, 5.0f);
    vec3 lightDir = normalize(lightPos - FragPos);
    float nDotL = max(0.0f, dot(Normal, lightDir));
    
    FragColor = objectColor *
    (
        vec4(vec3(0.1f), 1.0f) +
        vec4(nDotL, nDotL, nDotL, 1.0)
    );
}

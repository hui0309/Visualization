#version 430

in vec2 TEXCOORD;

uniform sampler2D texture0;
uniform sampler1D texture1;
uniform sampler2D texture2;
uniform float blend;

out vec4 Draw;

void main()
{
    vec4 texCoord = texture(texture2, TEXCOORD);
    Draw = mix(texture(texture0, TEXCOORD), texture(texture1, texCoord.a), blend);
} 
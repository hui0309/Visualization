#version 430
layout(location = 0) in vec2 vertexCor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TEXCOORD;

void main()
{
    TEXCOORD = vertexCor + 0.5f;
    gl_Position = projection * view * model * vec4(vertexCor, 0.0, 1.0);
}
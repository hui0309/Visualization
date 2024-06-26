#version 430
layout(location = 0) in vec3 vertexCor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(vertexCor, 1.0);
}
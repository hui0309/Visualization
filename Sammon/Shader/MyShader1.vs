#version 430
layout(location = 0) in vec2 vertexCor;
layout(location = 1) in vec4 vertexCol;
layout(location = 2) in vec2 axisX;
layout(location = 2) in vec2 axisY;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec4 color;
    vec2 axixX;
    vec2 axixY;
} vs_out;

void main()
{
    vs_out.color = vertexCol;
    vs_out.axisX = axisX;
    vs_out.axisY = axisY;
    gl_Position = projection * view * model * vec4(vertexCor, 0.0, 1.0);
}
#version 430
layout(location = 0) in vec3 vertexCor;
layout(lovation = 1) in vec3 textureCor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec3 GLOBAL;
out mat4 MODEL;

void main()
{
    gl_TexCoord[0] = textureCor;
    // GLOBAL = vertexCor;
    GLOBAL = (model * vec4(vertexCor, 1)).xyz;
    MODEL = model;
    gl_Position = projection * view * model * vec4(vertexCor, 1.0);
}
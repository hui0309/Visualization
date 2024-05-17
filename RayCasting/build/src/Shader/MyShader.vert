#version 430
layout (location = 0) in vec3 vertexCor;
layout (location = 1) in vec3 textureCor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 GLOBAL;
out vec3 TEXCOORD;

void main(){
    GLOBAL = (model * vec4(vertexCor, 1)).xyz;
    TEXCOORD = textureCor;
    gl_Position = projection * view * model * vec4(vertexCor, 1.0f);
};
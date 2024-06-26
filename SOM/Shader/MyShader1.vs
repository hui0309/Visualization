#version 430
layout(location = 0) in vec3 vertexCor;
layout(location = 1) in vec3 vertexNorm;
layout(location = 2) in vec2 vertexTex;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 NORM;
out vec3 GLOBAL;
out vec2 TEXCOORD;

void main()
{
    
    //discard;
    TEXCOORD = vertexTex;
    GLOBAL = (model * vec4(vertexCor, 1)).xyz;
    NORM = mat3(transpose(inverse(model))) * vertexNorm;
    gl_Position = projection * view * model * vec4(vertexCor, 1.0);
}
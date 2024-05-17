#version 430
layout(location = 0) in vec3 vertexCor;
//layout(location = 1) in vec3 normalCor;

//layout(location = 0) uniform mat4 view;
//layout(location = 1) uniform mat4 projection;
//layout(location = 2) uniform mat4 model;

//out vec3 GLOBAL;
//out vec3 NORM;

void main()
{
    //GLOBAL = (model * vec4(vertexCor, 1)).xyz;
    //NORM = mat3(transpose(inverse(model))) * normalCor;
    gl_Position = vec4(vertexCor, 1.0);
    //gl_Position = projection * view * model * vec4(vertexCor, 1.0);
}
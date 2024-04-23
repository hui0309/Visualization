#version 430
layout(location = 0) in vec3 vertexCor;
layout(location = 1) in vec3 normalCor;

layout(location = 0) uniform mat4 view;
layout(location = 1) uniform mat4 projection;
layout(location = 2) uniform mat4 model;
layout(location = 3) uniform vec4 coff;

out vec3 GLOBAL;
out vec3 NORM;
out vec3 CHECK;

void main()
{
    if(dot(vertexCor + vec3(126.0f, 126.0f, 126.0f), coff.xyz) > coff.w){
        CHECK = vec3(1.0f, 1.0f, 1.0f);
    }
    else{
        CHECK = vec3(0.0f, 0.0f, 0.0f);
    }
    GLOBAL = (model * vec4(vertexCor, 1)).xyz;
    NORM = mat3(transpose(inverse(model))) * normalCor;
    //gl_Position = vec4(vertexCor, 1.0);
    gl_Position = projection * view * model * vec4(vertexCor, 1.0);
}
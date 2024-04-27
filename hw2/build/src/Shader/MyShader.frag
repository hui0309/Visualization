#version 430

struct LIGHT {
    //vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct AABB{
    vec3 Pmax;
    vec3 Pmin;
};

in vec3 GLOBAL;
in mat4 MODEL;
in vec3 TEXCOORD;

uniform mat4 view;
uniform sampler3D texture0;
uniform sampler1D texture1;
uniform vec3 eyePos;
uniform vec3 rayDir;
uniform LIGHT Light;
uniform AABB aabb;

out vec4 FragColor;

vec3 PhongShade(vec3 P, vec3 texCoord, vec4 texel0, vec4 texel1){

    // ambient
    vec3 ambient = Light.ambient;

    // diffuse 
    vec3 norm = normalize(texel0.xyz * 2.0f - 1.0f);
    //vec3 norm = normalize((MODEL * vec4(normalize(texel0.xyz) * 2.0f - 1.0f, 1.0f)).xyz);
    vec3 lightDir = normalize(-Light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = Light.diffuse * diff;
    
    // specular
    vec3 viewDir = normalize(eyePos - P);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float shininess = 32.0f;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = Light.specular * spec;
    return (ambient + diffuse + specular) * texel1.rgb * texel1.a;
}

vec3 WorldToCoord(vec3 P){
    vec3 TexP;
    TexP.x = (P.x - aabb.Pmin.x) / (aabb.Pmax.x - aabb.Pmin.x);
    TexP.y = (P.y - aabb.Pmin.y) / (aabb.Pmax.y - aabb.Pmin.y);
    TexP.z = (P.z - aabb.Pmin.z) / (aabb.Pmax.z - aabb.Pmin.z);

    return (P - aabb.Pmin) / (aabb.Pmax - aabb.Pmin);
}

bool Outside(vec3 texCoord){
    if(texCoord.x < 0.0f || texCoord.x > 1.0f) return true;
    if(texCoord.y < 0.0f || texCoord.y > 1.0f) return true;
    if(texCoord.z < 0.0f || texCoord.z > 1.0f) return true;
    return false;
}

void main()
{
    vec3 raydir = normalize(rayDir);
    float T = 0.0f; //accumulate Opacities
    float rate = 0.1f;
    vec3 P = GLOBAL;
    vec3 texCoord = TEXCOORD;
    vec3 Color = vec3(0.0f);
    vec4 texel0 = texture(texture0, texCoord);
    vec4 texel1 = texture(texture1, texel0.a);
    //if(texel0.a > 60.0f / 255.0f) discard;
    while(true){   
        texel0 = texture(texture0, texCoord);
        texel1 = texture(texture1, texel0.a);     
        //vec3 myColor = PhongShade(P, texCoord, texel0, texel1);
        Color = Color + texel1.rgb * texel1.a * (1 - T);
        T = T + (1 - T) * texel1.a;
        P = P + raydir * rate;
        //texCoord = WorldToCoord(P);
        texCoord = TEXCOORD + (P - GLOBAL) / 255.0f;
        if(Outside(texCoord)) break;
        if(T > 0.8) break;
    }
    FragColor = vec4(Color, min(T, 1.0f));
    //FragColor = texel1;
}
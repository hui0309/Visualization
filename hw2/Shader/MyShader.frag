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

uniform sampler3D texture0;
uniform sampler1D texture1;
uniform vec3 eyePos;
uniform LIGHT Light;
uniform AABB aabb;

vec3 PhongShade(vec3 P, vec3 texCoord){
    texel0 = texture3D(texture0, texCoord);
    texel1 = texture3D(texture1, texel0.a);

    // ambient
    vec3 ambient = Light.ambient;

    // diffuse 
    vec3 norm = texel0.xyz * 2.0f - 1.0f;
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
    vec3 newP;
    // vec3 Pmax = aabb.Pmax;
    // vec3 Pmin = aabb.Pmin;
    vec3 Pmax = (model * vec4(aabb.Pmax, 1)).xyz;
    vec3 Pmin = (model * vec4(aabb.Pmin, 1)).xyz;
    newP.x = (P.x - Pmax.x) / (Pmax.x - Pmin.x);
    newP.y = (P.y - Pmax.y) / (Pmax.y - Pmin.y);
    newP.z = (P.z - Pmax.z) / (Pmax.z - Pmin.z);
    return newP;
}

bool Outside(vec3 texCoord){
    if(texCoord.x < 0.0f || texCoord.x > 1.0f) return false;
    if(texCoord.y < 0.0f || texCoord.y > 1.0f) return false;
    if(texCoord.z < 0.0f || texCoord.z > 1.0f) return false;
    return true;
}

void main()
{
    vec3 raydir;
    float T; //accumulate Opacities
    float rate;
    vec3 P = GLOBAL;
    vec3 texCoord, Color;
    vec3 myColor;
    vec4 texel0, texel1;

    raydir = normalize(P - eyePos);
    rate = 1.0f / length(P - eyePos) / 2.0f;
    T = 0.0f;
    Color = vec3(0.0f, 0.0f, 0.0f);
    texCoord = gl_TexCoord[0];
    while(true){        
        myColor = PhongShade(P, texCoord);

        Color = Color + (1 - T) * myColor;
        T = T + (1 - T) * texture1.a;
        P = P + rate * raydir;
        texCoord = WorldToCoord(P);
        if(Outside(texCoord)) break;
        if(T >= 1.0f) break;
    }
    gl_FragColor = Color;
} 
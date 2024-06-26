#version 430

struct LIGHT {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 NORM;
in vec3 GLOBAL;
in vec2 TEXCOORD;

uniform sampler2D texture0;
uniform vec3 viewPos;
uniform vec4 color;
uniform LIGHT Light;
uniform float isTex;

out vec4 Draw;

void main()
{
    vec4 texColor = texture(texture0, TEXCOORD);
    vec4 COLOR = mix(color, texColor, isTex);
    // ambient
    vec3 ambient = Light.ambient;
    
    // diffuse 
    vec3 norm = normalize(NORM);
    vec3 lightDir = normalize(-Light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = Light.diffuse * diff;
    
    // specular
    vec3 viewDir = normalize(viewPos - GLOBAL);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float shininess = 32.0f;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = Light.specular * spec;  
    
    vec3 result = (ambient + diffuse + specular) * COLOR.rgb;
    Draw = vec4(result, COLOR.a);
}

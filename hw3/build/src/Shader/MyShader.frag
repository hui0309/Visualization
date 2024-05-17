#version 430

struct LIGHT {
    //vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//in vec3 NORM;
//in vec3 GLOBAL;
//in vec3 CHECK;
//in vec3 Color;


//layout(location = 4) uniform vec3 viewPos;
//layout(location = 5) uniform vec3 objColor;
//layout(location = 6) uniform LIGHT Light;

out vec4 Draw;

void main()
{
    // ambient
    //vec3 ambient = Light.ambient;
  	
    // diffuse 
    //vec3 norm = normalize(NORM);
    //vec3 lightDir = normalize(-Light.direction);
    //float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = Light.diffuse * diff;
    
    // specular
    //vec3 viewDir = normalize(viewPos - GLOBAL);
    //vec3 reflectDir = reflect(-lightDir, norm);  
    //float shininess = 32.0f;
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    //vec3 specular = Light.specular * spec;  
    
    //vec3 result = (ambient + diffuse + specular) * objColor;
    //Draw = vec4(result, 1.0);
} 
#version 330 core
#define PI 3.14159265359


struct Light{
    vec3 lightPosition;
    vec4 lightColor;
};

out vec4 FragColor;
in vec3 color;
in vec2 texCoords;
in vec3 Normal;
in vec3 crntPosition;
in vec4 vertloc;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 camPos;
uniform vec2 iResolution;
uniform float iTime;
uniform vec3 skyColor;
uniform bool blinn;

const float density = 0.05;
const float gradient = 0.8;

float fog(){
    float distance = distance(vertloc.xyz, camPos.xyz);
    float visibility = clamp(exp(-pow((distance*density), gradient)), 0.0, 1.0);
    return visibility;
}

vec4 pointLight(){
    vec3 lightVec = lightPosition - crntPosition;
    float lightDist = length(lightVec);
    float a = 1;
    float b = 1;
    float intensity = 1.0f/(a*lightDist*lightDist + b*lightDist + 1.0f);

    float ambient = 0.20f;
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightVec);

    float diffuse = max(dot(normal, lightDirection), 0.0f);
    float specular = 0.0f;

    if(diffuse != 0.0f){
        float specularLight = 0.5f;
        vec3 viewDirection = normalize(camPos - crntPosition);
        vec3 reflectDirection = reflect(-lightDirection, normal);

        vec3 halfVec = normalize(viewDirection+lightDirection);

        //Increasing the power has the effect of making the specular light more point like
        float specAmount = pow(max(dot(normal, halfVec), 0.0f), 50);
        float specular = specAmount * specularLight;
    }

    return texture(diffuse0, texCoords) * lightColor * (diffuse * intensity + ambient) + texture(specular0, texCoords).r * specular;
}


vec4 directionalLight(){
    float ambient = 0.1f;
    vec3 normal = normalize(Normal);

    //Specify the direction in the opposite direction you want the light to affect.
    //Eg if you want the directional light coming DOWN from ABOVE
    //   you need to specify the light direction as straight up
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));

    float diffuse = max(dot(normal, lightDirection), 0.0f);
    
    float specularLight = 0.5f;
    vec3 viewDirection = normalize(camPos - crntPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    //Increasing the power has the effect of making the specular light more point like
    float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    return texture(diffuse0, texCoords) * lightColor * (diffuse + ambient) + texture(specular0, texCoords).r * specular;
}


vec4 spotLight(){

    //We write these values as the cosine of the angle, instead of the angle itself.
    //cosine calculations are slow. Womp Womp :(

    float outerCone = 0.90f;
    float innerCone = 0.95f;


    float ambient = 0.10f;
    vec3 normal = normalize(Normal);

    vec3 lightDirection = normalize(lightPosition - crntPosition);

    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specular = 0.0f;
    if(blinn){
        if(diffuse != 0.0f){
            float specularLight = 0.5f;
            vec3 viewDirection = normalize(camPos - crntPosition);
            vec3 reflectDirection = reflect(-lightDirection, normal);

            vec3 halfVec = normalize(viewDirection+lightDirection);

            //Increasing the power has the effect of making the specular light more point like
            float specAmount = pow(max(dot(normal, halfVec), 0.0f), 50);
            specular = specAmount * specularLight;
        }
    }else{
        float specularLight = 0.5f;
        vec3 viewDirection = normalize(camPos - crntPosition);
        vec3 reflectDirection = reflect(-lightDirection, normal);

        //Increasing the power has the effect of making the specular light more point like
        float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
        specular = specAmount * specularLight;
    }

    float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
    float intensity = clamp((angle - outerCone)/(innerCone - outerCone), 0.0f, 1.0f);

    return texture(diffuse0, texCoords) * lightColor * (diffuse*intensity + ambient) + texture(specular0, texCoords).r * specular*intensity;
}

vec4 returnDiffuseTexture(){
    return texture(diffuse0, texCoords);
}

vec4 returnSpecularTexture(){
    return texture(specular0, texCoords);
}


void main(){
    vec4 lightingDirectional = directionalLight();
    vec4 lightingSpot = spotLight();
    vec4 lighting = lightingDirectional + lightingSpot;
    float visibility = fog();
    //float visibility = 1.0;
    FragColor = mix(vec4(skyColor, 1.0f), lighting, visibility);
};
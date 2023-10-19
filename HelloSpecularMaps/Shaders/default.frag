#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 texCoords;
in vec3 Normal;
in vec3 crntPosition;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 camPos;


vec4 pointLight(){
    vec3 lightVec = lightPosition - crntPosition;
    float lightDist = length(lightVec);
    float a = 1;
    float b = 1;
    float intensity = 1.0f/(a*lightDist*lightDist + b*lightDist + 1.0f);

    float ambient = 0.01f;
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightVec);

    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specularLight = 0.5f;
    vec3 viewDirection = normalize(camPos - crntPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    //Increasing the power has the effect of making the specular light more point like
    float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    return texture(tex0, texCoords) * lightColor * (diffuse * intensity + ambient) + texture(tex1, texCoords).r * specular;
}


vec4 directionalLight(){
    float ambient = 0.20f;
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

    return texture(tex0, texCoords) * lightColor * (diffuse + ambient) + texture(tex1, texCoords).r * specular;
}


vec4 spotLight(){

    //We write these values as the cosine of the angle, instead of the angle itself.
    //cosine calculations are slow. Womp Womp :(

    float outerCone = 0.90f;
    float innerCone = 0.95f;


    float ambient = 0.20f;
    vec3 normal = normalize(Normal);

    vec3 lightDirection = normalize(lightPosition - crntPosition);

    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specularLight = 0.5f;
    vec3 viewDirection = normalize(camPos - crntPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    //Increasing the power has the effect of making the specular light more point like
    float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
    float intensity = clamp((angle - outerCone)/(innerCone - outerCone), 0.0f, 1.0f);

    return texture(tex0, texCoords) * lightColor * (diffuse*intensity + ambient) + texture(tex1, texCoords).r * specular*intensity;
}






void main(){


    FragColor = pointLight();
};
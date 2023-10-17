#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 texCoords;
in vec3 Normal;
in vec3 crntPosition;

uniform sampler2D tex0;
uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 camPos;

void main(){
    float ambient = 0.20f;
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightPosition - crntPosition);

    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specularLight = 0.5f;
    vec3 viewDirection = normalize(camPos - crntPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    //Increasing the power has the effect of making the specular light more point like
    float specAmount = pow(max(dot(viewDirection, reflectDirection), 0.0f), 8);
    float specular = specAmount * specularLight;

    FragColor = texture(tex0, texCoords) * lightColor * (diffuse + ambient + specular);
};
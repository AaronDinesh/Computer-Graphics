#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 texCoords;
in vec3 Normal;
in vec3 crntPosition;

uniform sampler2D tex0;
uniform vec4 lightColor;
uniform vec3 lightPosition;

void main(){
    float ambient = 0.20f;
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightPosition - crntPosition);

    float diffuse = max(dot(normal, lightPosition), 0.0f);

    FragColor = texture(tex0, texCoords) * lightColor * (diffuse+ambient);
};
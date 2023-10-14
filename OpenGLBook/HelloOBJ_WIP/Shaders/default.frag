#version 330 core
out vec4 FragColor;
in vec3 color;
//in vec2 texCoords;

//uniform sampler2D tex0;

void main(){
    //FragColor = texture(tex0, texCoords);
    FragColor = vec4(color, 1.0f);
};
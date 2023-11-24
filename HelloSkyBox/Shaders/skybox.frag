#version 330 core
out vec4 FragColor;

uniform vec2 resolution;

void main()
{    
    FragColor = vec4(gl_FragCoord.xy / resolution.xy, 1.0, 1.0); 
}
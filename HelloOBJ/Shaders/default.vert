#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTex;

out vec3 color;
out vec2 texCoords;
out vec3 Normal;
out vec3 crntPosition;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 myTotalTransformation;

void main(){
  crntPosition = vec3(model * translation * rotation * myTotalTransformation * scale * vec4(aPos, 1.0f));
  color = aColor;
  texCoords = mat2(0.0, -1.0, 1.0, 0.0) * aTex;
  Normal = aNormal;
  gl_Position = camMatrix * vec4(crntPosition, 1.0);
};
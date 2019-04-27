#version 330 core
layout (location = 0) in vec3 Position;

out vec3 texCoords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 scaleMatrix;

void main() {
    texCoords = vec3(scaleMatrix * vec4(Position, 1.0));
    gl_Position = projectionMatrix * viewMatrix * scaleMatrix * vec4(Position, 1.0);
}  
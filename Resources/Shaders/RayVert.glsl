#version 330 core

in vec4 vertex;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main(){

	gl_Position =  projectionMatrix * viewMatrix * vertex;
}
#version 330 core

in vec4 vVertex;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main(){
	gl_Position = lightSpaceMatrix * model * vVertex;
}
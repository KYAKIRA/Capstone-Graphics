#version 330 core

in vec4 vVertex;
in vec4 vNormal;

out float visiblility;
out vec3 vertPos;
out vec3 vertNormal;
 
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix; 

uniform float density;
uniform float gradient;	

void main(){
	vertNormal= mat3(transpose(inverse(modelMatrix))) * vNormal.xyz;
	vertPos = vec3((modelMatrix * vVertex));
	mat4 modelViewMatrix = viewMatrix * modelMatrix;
	gl_Position = projectionMatrix * modelViewMatrix * vVertex;
	
	//Fog calculations
	vec4 positionRelativeToCam = viewMatrix * modelMatrix * vVertex;
	float distance = length(positionRelativeToCam.xyz);
	visiblility = exp(-pow((distance * density), gradient));
	visiblility = clamp(visiblility, 0.0, 1.0);
}
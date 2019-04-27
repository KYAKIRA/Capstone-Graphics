#version 330 core

in vec4 vVertex;
in vec4 vNormal;
in vec2 vTexture;

out vec3 vertPos;
out vec3 vertNormal;
out vec2 texCoords;
out float visiblility;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

uniform float density;
uniform float gradient;

void main(){
	texCoords = vTexture;
	
	vertNormal = normalize(normalMatrix * vNormal.xyz); //Rotate the normal to the correct orientation
	
	mat4 modelViewMatrix = viewMatrix * modelMatrix;
	
	vertPos = vec3((modelMatrix * vVertex));
	
	gl_Position =  projectionMatrix * modelViewMatrix * vVertex;
	
	//Fog calculations
	vec4 positionRelativeToCam = viewMatrix * modelMatrix * vVertex;
	float distance = length(positionRelativeToCam.xyz);
	visiblility = exp(-pow((distance * density), gradient));
	visiblility = clamp(visiblility, 0.0, 1.0);
}
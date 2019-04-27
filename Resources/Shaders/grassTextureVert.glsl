#version 330 core

#define pi 3.1415926535897932384626433832795

in vec4 vVertex;
in vec4 vNormal;
in vec2 vTexture;

out vec3 vertPos;
out vec3 vertNormal;
out vec2 texCoords;
out float visibility;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

uniform vec3 sway;
uniform float time;
uniform float frequency;

uniform float fogDensity;
uniform float fogGradient;

void main(){
	float phase = 2*pi*frequency*time;
	texCoords = vTexture;
	
	vec3 movement = sway;
	movement.x = sway.x * (1.0 - texCoords.y) * sin(vVertex.y + phase);
	movement.y = sway.y * sin(vVertex.x + 1.0 * vVertex.z) * (1.0 - texCoords.y);
	movement.z = sway.z * (cos(time) * (1.0 - texCoords.y)) * sin(vVertex.z + phase);
	
	vec4 grass_movement = vec4(
	vVertex.x + movement.x, 
	vVertex.y + movement.y,
	vVertex.z + movement.z + movement.x / 5.0 + movement.y / 2.0, 1.0);
	
	vertNormal = normalize(normalMatrix * vNormal.xyz); //Rotate the normal to the correct orientation
	
	mat4 modelViewMatrix = viewMatrix * modelMatrix; 

	vertPos = vec3((modelMatrix * vVertex));
	
	gl_Position =  projectionMatrix * modelViewMatrix * grass_movement;
	
	//Fog calculations
	vec4 positionRelativeToCam = viewMatrix * modelMatrix * vVertex;
	float distance = length(positionRelativeToCam.xyz);
	visibility = exp(-pow((distance * fogDensity), fogGradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
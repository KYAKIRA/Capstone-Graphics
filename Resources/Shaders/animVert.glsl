#version 330 core

layout (location = 0) in vec4 vVertex;
layout (location = 1) in vec4 vNormal;
layout (location = 2) in vec2 vTexture;
layout (location = 3) in uvec4 boneIDs;
layout (location = 4) in vec4 boneWeights;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

uniform float fogDensity;
uniform float fogGradient;

uniform mat4 bones[128];

out vec3 vertPos;
out vec3 vertNormal;
out vec2 texCoords;
out float visibility;

void main(){
	mat4 modelViewMatrix = viewMatrix * modelMatrix;
	
	mat4 boneTransform = bones[boneIDs[0]] * boneWeights[0];
	boneTransform += bones[boneIDs[1]] * boneWeights[1];
	boneTransform += bones[boneIDs[2]] * boneWeights[2];
	boneTransform += bones[boneIDs[3]] * boneWeights[3];
	
	vertPos = vec3((modelMatrix * (boneTransform * vVertex)));
	vertNormal = normalize(normalMatrix * vNormal.xyz); //Rotate the normal to the correct orientation
	texCoords = vTexture;
	
	gl_Position =  projectionMatrix * viewMatrix * modelMatrix * (boneTransform * vVertex);
	
	//Fog calculations
	vec4 positionRelativeToCam = viewMatrix * modelMatrix * vVertex;
	float distance = length(positionRelativeToCam.xyz);
	visibility = exp(-pow((distance * fogDensity), fogGradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
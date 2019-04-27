#version 330 core

layout (location = 0) in vec4 vVertex;
layout (location = 1) in vec4 vNormal;
layout (location = 2) in vec2 vTexture;
layout (location = 3) in uvec4 boneIDs;
layout (location = 4) in vec4 boneWeights;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

uniform mat4 bones[128];

void main(){
	mat4 boneTransform = bones[boneIDs[0]] * boneWeights[0];
	boneTransform += bones[boneIDs[1]] * boneWeights[1];
	boneTransform += bones[boneIDs[2]] * boneWeights[2];
	boneTransform += bones[boneIDs[3]] * boneWeights[3];
	
	gl_Position = lightSpaceMatrix * model * (boneTransform * vVertex);
}
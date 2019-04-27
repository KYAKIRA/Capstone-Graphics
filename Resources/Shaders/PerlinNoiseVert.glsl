#version 330 core

layout (location = 0) in vec4 vVertex;
layout (location = 1) in vec4 vNormal;
layout (location = 2) in vec2 vTexture;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform sampler3D noiseTexture;
uniform float offsetMult;
uniform float speedMult;
uniform float time;

out vec3 vertPos;
out vec3 vertNormal;
out vec2 texCoords;

void main(){
	texCoords = vTexture;
	
	vertNormal = (normalMatrix * vNormal.xyz);
	
	mat4 modelViewMatrix = viewMatrix * modelMatrix;
	
	vertPos = vec3((modelMatrix * vVertex));
	
	vec3 noiseVec = normalize(vec3(texture(noiseTexture,vNormal.xyz + time / speedMult)));
	vec3 offset = vNormal.xyz * dot(vertNormal,normalize(noiseVec)) /offsetMult;
	
	gl_Position = projectionMatrix * modelViewMatrix * (vVertex + (vec4(offset, 1.0)));
}
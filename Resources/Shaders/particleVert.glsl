#version 330 core

in vec4 vVertex;

out vec2 texCoords1;
out vec2 texCoords2;
out float blend;

uniform mat4 projection;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec2 texOffset1;
uniform vec2 texOffset2;
uniform vec2 texCoordInfo;

void main(){
	vec2 texCoords = vVertex.xy + vec2(0.5,0.5);
	texCoords.y = 1.0 - texCoords.y; //Flip the y
	texCoords /= texCoordInfo.x;
	
	texCoords1 = texCoords + texOffset1;
	texCoords2 = texCoords + texOffset2;
	blend = texCoordInfo.y;
	
	mat4 modelViewMatrix = viewMatrix * modelMatrix;
	gl_Position =  projection * modelViewMatrix * vVertex;
}
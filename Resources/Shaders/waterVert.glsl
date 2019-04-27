#version 330 core

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

uniform float time;
uniform float waterHeight;
uniform int waveAmount;
uniform float amplitude[8];
uniform float waveLength[8];
uniform float speed[8];
uniform vec3 direction[8];

const float pi = 3.141592653589793238;

uniform float fogDensity;
uniform float fogGradient;

float wave(int i, float x, float y){
	float waveFrequency = 2 * pi / waveLength[i];
	float wavePhase = speed[i] * waveFrequency;
	float theta = dot(direction[i], vec3(x, y, 0));

	return amplitude[i] * sin(theta * waveFrequency + time * wavePhase);
}

float waveHeight(float x, float y){
	float height = 0.0;

	for(int i = 0; i < waveAmount; ++i)
		height += wave(i, x, y);

	return height;
}

float dWavedx(int i, float x, float y){
	float frequency = 2 * pi / waveLength[i];
	float phase = speed[i] * frequency;
	float theta = dot(direction[i], vec3(x, y, 0));
	float a = amplitude[i] * direction[i].x * frequency;

	return a * cos(theta * frequency + time * phase);
}

float dWavedy(int i, float x, float y){
	float frequency = 2 * pi / waveLength[i];
	float phase = speed[i] * frequency;
	float theta = dot(direction[i], vec3(x, y, 0));
	float a = amplitude[i] * direction[i].y * frequency;
	
	return a * cos(theta * frequency + time * phase);
}

vec3 waveNormal(float x, float y){
	float dx = 0.0;
	float dy = 0.0;

	for(int i = 0; i < waveAmount; ++i){
		dx += dWavedx(i, x, y);
		dy += dWavedy(i, x, y);
	}

	vec3 norm = vec3(-dx, -dy, 1.0);
	return normalize(norm);
}

void main(){
	vec4 pos = vVertex;

	pos.y = waterHeight + waveHeight(pos.x, pos.z);

	texCoords = vTexture;
	//Changed the old lighting to handle the pos x and z through the wave normal function
	vertNormal = normalize(normalMatrix * vNormal.y * waveNormal(pos.x,pos.z)); //Rotate the normal to the correct orientation

	mat4 modelViewMatrix = viewMatrix * modelMatrix;

	vertPos = vec3((modelMatrix * vVertex));

	gl_Position =  projectionMatrix * modelViewMatrix * pos;
	
	//Fog calculations
	vec4 positionRelativeToCam = viewMatrix * modelMatrix * vVertex;
	float distance = length(positionRelativeToCam.xyz);
	visibility = exp(-pow((distance * fogDensity), fogGradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
#version 330 core

#include "_shared.glsl"

in vec3 vertPos;
in vec3 vertNormal;
in vec2 texCoords;

uniform TextureMaterial material;
uniform vec3 viewPos;
uniform sampler3D noiseTexture;
uniform float time;
uniform float speedMult;
uniform bool textureIsMoving;

uniform vec4 baseAmbient;

uniform PointLight pointLights[8];
uniform DirectionalLight directionalLights[8];
uniform SpotLight spotLights[8];

uniform int numPointLights;
uniform int numDirLights;
uniform int numSpotLights;

out vec4 fragColor;

void main(){
	vec3 norm = normalize(vertNormal);
	vec3 viewDir = normalize(viewPos - vertPos);
	vec3 offSet= vec3(time,time,time);
	
	vec4 textured;
	if(textureIsMoving){
		textured = vec4(texture(material.diffuseMap, texCoords+(time/speedMult)));
	}else{
		textured = vec4(texture(material.diffuseMap, texCoords));
	}
	
	vec4 totalLight = baseAmbient;
	
	for(int i = 0; i < numPointLights; i++){
		totalLight += Lighting(pointLights[i], material, vertPos, viewDir, norm, texCoords); 
	}
	
	for(int j = 0; j < numDirLights; j++){
		totalLight += Lighting(directionalLights[j], material, vertPos, viewDir, norm, texCoords);
	}
	
	for(int l = 0; l < numSpotLights; l++){
		totalLight += Lighting(spotLights[l], material, vertPos, viewDir, norm, texCoords);
	}
	
	vec4 colorNew = vec4(0.7, 0.1, 0.0, 1.0);
	
	vec4 perlinColor = texture(noiseTexture,(vertPos * 1.25), 1.0);
	float intensityColor =	abs(perlinColor.x - 0.25) +
							abs(perlinColor.y - 0.125) +
							abs(perlinColor.z - 0.0625) +
							abs(perlinColor.w - 0.03125);
							
	fragColor = textured * totalLight;
}
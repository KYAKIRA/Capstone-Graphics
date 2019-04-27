#version 330 core

#include "_shared.glsl"

in vec3 vertPos;
in vec3 vertNormal;
in vec2 texCoords;
in float visibility;

uniform TextureMaterial material;
uniform vec3 viewPos;

uniform vec4 baseAmbient;

uniform PointLight pointLights[8];
uniform DirectionalLight directionalLights[8];
uniform SpotLight spotLights[8];

uniform int numPointLights;
uniform int numDirLights;
uniform int numSpotLights;

uniform vec4 fogColor;

out vec4 fragColor;

void main(){
	vec3 norm = normalize(vertNormal);
	vec3 viewDir = normalize(viewPos - vertPos);
	
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
	
	vec4 textured;
	textured = vec4(texture(material.diffuseMap, texCoords * material.textureScale));
	
	if(textured.a < 0.1){
		discard;
	}
	
	fragColor = mix(fogColor, totalLight * textured, visibility);
}
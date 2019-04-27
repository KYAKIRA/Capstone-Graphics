#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox1;
uniform samplerCube skybox2;
uniform float blendFactor; 

uniform vec4 fogColor; 
uniform vec2 limits;
uniform bool useFog;

void main() {    
	
	vec4 texture1 = texture(skybox1, TexCoords);
	vec4 texture2 = texture(skybox2, TexCoords); 
	vec4 finalColor = mix(texture1, texture2, blendFactor); 
	
	FragColor = finalColor;
	
	if(useFog == true){
		float factor = (TexCoords.y - limits.y) / (limits.x - limits.y);
		factor = clamp(factor, 0.0, 1.0); 
		FragColor = mix(fogColor, finalColor, factor); 
	}  
}
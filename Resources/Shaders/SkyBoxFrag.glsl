#version 330 core

in vec3 texCoords;
out vec4 fragColor;

uniform samplerCube skybox;

uniform vec4 fogColor; 
uniform vec2 limits;
uniform bool useFog;
void main() {    
    vec4 finalColor = texture(skybox, texCoords);

	fragColor = finalColor;
	
	if(useFog == true){
		float factor = (texCoords.y - limits.y) / (limits.x - limits.y);
		factor = clamp(factor, 0.0, 1.0); 
		fragColor = mix(fogColor, finalColor, factor); 
	}   
}
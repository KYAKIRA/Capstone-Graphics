#version 330 core

in vec2 texCoords1;
in vec2 texCoords2;
in float blend;

out vec4 FragColor;

uniform sampler2D particleTexture;

void main(){
	vec4 color1 = vec4(texture(particleTexture, texCoords1));
	vec4 color2 = vec4(texture(particleTexture, texCoords2));

	FragColor = mix(color1, color2, blend);
	
	//Sets to all white
	//FragColor = vec4(1.0);
}
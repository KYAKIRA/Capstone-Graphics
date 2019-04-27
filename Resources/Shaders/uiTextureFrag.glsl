#version 330 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D text;
uniform float transparency;

void main(){
	vec4 color = texture(text, texCoords);
	color.w = color.w * transparency;
	
	fragColor = color;
}
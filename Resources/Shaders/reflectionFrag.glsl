#version 330 core

in vec3 vertPos;
in vec3 vertNormal;
in float visiblility;

out vec4 FragColor;

uniform vec3 viewPos;
uniform samplerCube skybox;
uniform bool isReflective;
uniform float refractionIndex;

uniform vec4 fogColor;

void main(){
	vec3 R = vec3(0,0,0);
	vec3 I = normalize(vertPos - viewPos);
	
	if(isReflective){
		R = reflect(I, normalize(vertNormal));
	}else if(!isReflective){
		float ratio = 1.00 / refractionIndex;
		R = refract(I, normalize(vertNormal), ratio);
	}
	
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
	
	FragColor = mix(fogColor, FragColor, visiblility);
}
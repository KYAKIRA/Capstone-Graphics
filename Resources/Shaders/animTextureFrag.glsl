#version 330 core

struct Material{
	sampler2D diffuseMap;
	sampler2D specularMap;
	float shininess;
};
struct PointLight{
	vec3 position;
	
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
	
	float constant;
    float linear;
    float quadratic;
	//--------------------
	float lightIntensity;
	vec4 lightColor;	
};
struct DirectionalLight{
	vec3 direction;
	
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
	//--------------------
	float lightIntensity;
	vec4 lightColor;
};
struct SpotLight{
	vec3 position;
	vec3 direction;
	
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;	
	
	float constant;
    float linear;
    float quadratic;
	float cutOff;
	float outerCutOff;
	//--------------------
	float lightIntensity;
	vec4 lightColor;
};

in vec3 vertPos;
in vec3 vertNormal;
in vec2 texCoords;
in float visiblility;

uniform Material material;  
uniform vec3 viewPos;

//Base Light in the event no lighting is there
uniform vec4 BaseAmbient;
//Array of each Light type 
uniform PointLight pointLights[32];
uniform DirectionalLight directionalLights[32];
uniform SpotLight spotLights[32];
//Number of each light in the game
uniform int NumPntLight;
uniform int NumDirLight;
uniform int NumSPLight;

//Resize Texture Uniform
uniform float resizeTexture;
out vec4 fragColor;

uniform vec4 fogColor;

//Prototype Functions
vec4 CalculatePointLighting(PointLight light, vec3 fragPos, vec3 viewDir,vec3 normal);	//Point
vec4 CalculateDirectionalLighting(DirectionalLight light, vec3 viewDir, vec3 normal); 	//Directional	
vec4 CalculateSpotLighting(SpotLight light, vec3 fragPos, vec3 viewDir,vec3 normal);	//Spot

void main(){ 
	vec3 norm = normalize(vertNormal);
	vec3 viewDir = normalize(viewPos - vertPos);
  
	
	vec4 totalLight = BaseAmbient;
	//Cacualte Point Lighting
	for(int i = 0; i < NumPntLight; i++){
		totalLight += CalculatePointLighting(pointLights[i], vertPos, viewDir, norm); 
	}
	//Cacualte Directional Lighting
	for(int j = 0; j < NumDirLight; j++){
		totalLight += CalculateDirectionalLighting(directionalLights[j], viewDir, norm);
	} 
	//Cacualte Spot Lighting
	for(int l = 0; l < NumSPLight; l++){
		totalLight += CalculateSpotLighting(spotLights[l], vertPos, viewDir, norm);
	} 
	
	//End Result
	fragColor = mix(fogColor, totalLight, visiblility);
	
}

//Cacualte Point Lights
vec4 CalculatePointLighting(PointLight light, vec3 fragPos, vec3 viewDir, vec3 normal){
    vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	// attenuation
    float distance    = length(light.position - fragPos); 
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
	// combine results
    vec4 ambient  = light.ambient  *          vec4(texture(material.diffuseMap, texCoords*resizeTexture));
    vec4 diffuse  = light.diffuse  *  diff *  vec4(texture(material.diffuseMap, texCoords*resizeTexture));
    vec4 specular = light.specular *  spec  *  vec4(texture(material.specularMap, texCoords*resizeTexture));
    ambient  *= attenuation * light.lightIntensity;	 
    diffuse  *= attenuation * light.lightIntensity;	 
    specular *= attenuation * light.lightIntensity;	 
    return (ambient + diffuse + specular) * light.lightColor ;
}

//Cacualte Directional Lights
vec4 CalculateDirectionalLighting(DirectionalLight light, vec3 viewDir, vec3 normal){
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	// combine results
    vec4 ambient  = light.ambient  * 		 vec4(texture(material.diffuseMap, texCoords*resizeTexture));
    vec4 diffuse  = light.diffuse  * diff *  vec4(texture(material.diffuseMap, texCoords*resizeTexture));
    vec4 specular = light.specular * spec *  vec4(texture(material.specularMap, texCoords*resizeTexture));
	ambient  *= light.lightIntensity;	 
    diffuse  *= light.lightIntensity;	 
    specular *= light.lightIntensity;	
    return (ambient + diffuse + specular) * light.lightColor ; 
}

//Cacualte Spot Lights
vec4 CalculateSpotLighting(SpotLight light, vec3 fragPos, vec3 viewDir,vec3 normal){
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos); 
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec4 ambient  = light.ambient  *          vec4(texture(material.diffuseMap, texCoords*resizeTexture));
    vec4 diffuse  = light.diffuse  * diff *   vec4(texture(material.diffuseMap, texCoords*resizeTexture));
    vec4 specular = light.specular * spec *   vec4(texture(material.specularMap, texCoords*resizeTexture));
    ambient  *= attenuation * intensity * light.lightIntensity;
    diffuse  *= attenuation * intensity * light.lightIntensity;
    specular *= attenuation * intensity * light.lightIntensity;
    return (ambient + diffuse + specular) * light.lightColor ;
}
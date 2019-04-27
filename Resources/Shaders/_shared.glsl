struct ColorMaterial{
	vec4 color;
	float shininess;
	bool receivesShadows;
};

struct TextureMaterial{
	sampler2D diffuseMap;
	sampler2D specularMap;
	float shininess;
	bool receivesShadows;
	float textureScale;
};

struct PointLight{
	vec3 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float constant;
	float linear;
	float quadratic;
	float lightIntensity;
	vec4 lightColor;
};

struct DirectionalLight{
	vec3 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float lightIntensity;
	vec4 lightColor;
	mat4 lightSpaceMatrix;
	sampler2D shadowMap;
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
	float lightIntensity;
	vec4 lightColor;
	mat4 lightSpaceMatrix;
};

vec4 Lighting(PointLight light, ColorMaterial material, vec3 fragPos, vec3 viewDir, vec3 normal);
vec4 Lighting(DirectionalLight light, ColorMaterial material, vec3 vertPos, vec3 viewDir, vec3 normal);
vec4 Lighting(SpotLight light, ColorMaterial material, vec3 fragPos, vec3 viewDir,vec3 normal);

vec4 Lighting(PointLight light, TextureMaterial material, vec3 fragPos, vec3 viewDir, vec3 normal, vec2 texCoords);
vec4 Lighting(DirectionalLight light, TextureMaterial material, vec3 vertPos, vec3 viewDir, vec3 normal, vec2 texCoords);
vec4 Lighting(SpotLight light, TextureMaterial material, vec3 fragPos, vec3 viewDir,vec3 normal, vec2 texCoords);

float Shadows(DirectionalLight light, vec3 vertPos, vec3 normal, vec3 lightDir);

vec4 Lighting(PointLight light, ColorMaterial material, vec3 fragPos, vec3 viewDir, vec3 normal){
	vec3 lightDir = normalize(light.position - fragPos);
	
	float diff = max(dot(normal, lightDir), 0.0);
	
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	vec4 ambient = light.ambient * material.color;
	vec4 diffuse = light.diffuse * diff * material.color;
	vec4 specular = light.specular * spec * material.color;
	
	ambient *= attenuation * light.lightIntensity;
	diffuse *= attenuation * light.lightIntensity;
	specular *= attenuation * light.lightIntensity;
	return (ambient + diffuse + specular) * light.lightColor;
}

vec4 Lighting(DirectionalLight light, ColorMaterial material, vec3 vertPos, vec3 viewDir, vec3 normal){
	vec3 lightDir = normalize(-light.direction);
	
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

	vec4 ambient = light.ambient * material.color * light.lightIntensity;
	vec4 diffuse = light.diffuse * diff * material.color * light.lightIntensity;
	vec4 specular = light.specular * spec * material.color * light.lightIntensity;
	
	float shadow = 0.0;
	if(material.receivesShadows){
		shadow = Shadows(light, vertPos, normal, lightDir);
	}
	
	return (ambient + (1.0 - shadow) * (diffuse + specular)) * light.lightColor;
}

vec4 Lighting(SpotLight light, ColorMaterial material, vec3 fragPos, vec3 viewDir,vec3 normal){
	vec3 lightDir = normalize(light.position - fragPos);
	
	float diff = max(dot(normal, lightDir), 0.0);
	
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	float theta = dot(lightDir, normalize(-light.direction)); 
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	
	vec4 ambient = light.ambient * material.color;
	vec4 diffuse = light.diffuse * diff * material.color;
	vec4 specular = light.specular * spec * material.color;
	
	ambient *= attenuation * intensity * light.lightIntensity;
	diffuse *= attenuation * intensity * light.lightIntensity;
	specular *= attenuation * intensity * light.lightIntensity;
	
	return (ambient + diffuse + specular) * light.lightColor;
}

vec4 Lighting(PointLight light, TextureMaterial material, vec3 fragPos, vec3 viewDir, vec3 normal, vec2 texCoords){
	vec3 lightDir = normalize(light.position - fragPos);
	
	float diff = max(dot(normal, lightDir), 0.0);
	
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	vec4 ambient  = light.ambient * vec4(texture(material.diffuseMap, texCoords * material.textureScale));
	vec4 diffuse  = light.diffuse * diff * vec4(texture(material.diffuseMap, texCoords * material.textureScale));
	vec4 specular = light.specular * spec * vec4(texture(material.specularMap, texCoords * material.textureScale));
	ambient *= attenuation * light.lightIntensity;
	diffuse *= attenuation * light.lightIntensity;
	specular *= attenuation * light.lightIntensity;
	
	return (ambient + diffuse + specular) * light.lightColor;
}

vec4 Lighting(DirectionalLight light, TextureMaterial material, vec3 vertPos, vec3 viewDir, vec3 normal, vec2 texCoords){
	vec4 color = vec4(texture(material.diffuseMap, texCoords * material.textureScale));
	
	vec3 lightDir = normalize(-light.direction);
	
	float diff = max(dot(normal, lightDir), 0.0);
	
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	
	vec4 ambient  = light.ambient;
	vec4 diffuse  = light.diffuse * diff;
	vec4 specular = light.specular * spec;
	ambient  *= light.lightIntensity;
	diffuse  *= light.lightIntensity;
	specular *= light.lightIntensity;
	
	float shadow = 0.0;
	if(material.receivesShadows){
		shadow = Shadows(light, vertPos, normal, lightDir);
	}
	
	return (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
}

vec4 Lighting(SpotLight light, TextureMaterial material, vec3 fragPos, vec3 viewDir,vec3 normal, vec2 texCoords){
	vec3 lightDir = normalize(light.position - fragPos);
	
	float diff = max(dot(normal, lightDir), 0.0);
	
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	
	float distance = length(light.position - fragPos); 
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	float theta = dot(lightDir, normalize(-light.direction)); 
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	
	vec4 ambient = light.ambient * vec4(texture(material.diffuseMap, texCoords * material.textureScale));
	vec4 diffuse = light.diffuse  * diff * vec4(texture(material.diffuseMap, texCoords * material.textureScale));
	vec4 specular = light.specular * spec * vec4(texture(material.specularMap, texCoords * material.textureScale));
	ambient  *= attenuation * intensity * light.lightIntensity;
	diffuse  *= attenuation * intensity * light.lightIntensity;
	specular *= attenuation * intensity * light.lightIntensity;
	
	return (ambient + diffuse + specular) * light.lightColor;
}

float Shadows(DirectionalLight light, vec3 vertPos, vec3 normal, vec3 lightDir){
	vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(vertPos, 1.0);
	
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	
	//Stops shadows from rendering outside the shadow distance
	if(projCoords.z > 1.0){
		return 0.0;
	}
	
	float currentDepth = projCoords.z;
	float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(light.shadowMap, 0);
	for(int x = -1; x <= 1; ++x){
		for(int y = -1; y <= 1; ++y){
			float pcfDepth = texture(light.shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	return shadow;
}
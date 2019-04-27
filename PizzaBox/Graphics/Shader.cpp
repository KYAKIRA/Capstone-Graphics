#include "Shader.h"

#include <string>

#include <glew.h>

#include "Graphics/RenderEngine.h"
#include "Lighting/DirectionalLight.h"
#include "Lighting/PointLight.h"
#include "Lighting/SpotLight.h"
#include "LowLevel/Uniform.h"

#include "Core/FileSystem.h"
#include "Graphics/Color.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

int Shader::maxTextureUnits = 0;

//Vertex and Fragment Shader filenames, then the number of attributes (in pairs)
//The pair consists of an index and a name
Shader::Shader(const std::string& vertPath_, const std::string& fragPath_) : Resource(vertPath_), fragmentFileName(fragPath_), shader(0), uniforms(), currentBoundTextures(0){
	_ASSERT(FileSystem::FileExists(fileName));
	_ASSERT(FileSystem::FileExists(fragmentFileName));
}

Shader::Shader(const std::string& vertPath_, const std::string& fragPath_, int numAttributes_, ...) : Resource(vertPath_), fragmentFileName(fragPath_), shader(0), uniforms(), currentBoundTextures(0){
	_ASSERT(FileSystem::FileExists(fileName));
	_ASSERT(FileSystem::FileExists(fragmentFileName));
}

Shader::Shader(const std::string& vertPath_, const std::string& fragPath_, const std::vector<std::string>& args_) : Resource(vertPath_), fragmentFileName(fragPath_), shader(0), uniforms(), currentBoundTextures(0){
	_ASSERT(FileSystem::FileExists(fileName));
	_ASSERT(FileSystem::FileExists(fragmentFileName));
}

Shader::~Shader(){
}

bool Shader::Load(){
	std::string vString = FileSystem::ReadFileToString(fileName);
	std::string fString = FileSystem::ReadFileToString(fragmentFileName);

	//Add in code shared by all shaders if include directive is found
	std::string toReplace = "#include \"_shared.glsl\"";
	size_t pos = vString.find(toReplace);
	if(pos != std::string::npos){
		vString = vString.replace(pos, toReplace.size(), RenderEngine::GetSharedShaderCode());
	}

	pos = fString.find(toReplace);
	if(pos != std::string::npos){
		fString = fString.replace(pos, toReplace.size(), RenderEngine::GetSharedShaderCode());
	}

	if(vString.empty() || fString.empty()){
		Debug::LogError("Could not load shader files!", __FILE__, __LINE__);
		return false;
	}

	//Our shader code needs to be stored in a const char* to be passed to OpenGL
	const char* vsText = vString.c_str();
	const char* fsText = fString.c_str();

	//GL_VERTEX_SHADER and GL_FRAGMENT_SHADER are defined in glew.h
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Check for errors
	if(vertShader == 0 || fragShader == 0){
		Debug::LogError("Can't create a new shader!", __FILE__, __LINE__);
		return false;
	}

	glShaderSource(vertShader, 1, &vsText, 0);
	glShaderSource(fragShader, 1, &fsText, 0);

	glCompileShader(vertShader);
	//Check for errors
	GLint status;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE){
		Debug::LogError("VERT: " + GetShaderLog(vertShader));
		return false;
	}

	glCompileShader(fragShader);
	//Check for errors
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE){
		Debug::LogError("FRAG: " + GetShaderLog(fragShader));
		return false;
	}

	shader = glCreateProgram();
	glAttachShader(shader, fragShader);
	glAttachShader(shader, vertShader);

	//--------------------------------------------------------------------------------------------------------

	glLinkProgram(shader);
	/// Check for errors
	glGetProgramiv(shader, GL_LINK_STATUS, &status);
	if(status == GL_FALSE){
		Debug::LogError(GetShaderLog(shader));
		return false;
	}

	glDeleteShader(fragShader);
	glDeleteShader(vertShader);
	glDetachShader(shader, fragShader);
	glDetachShader(shader, vertShader);

	//--------------------------------------------------------------------------------------------------------

	int count;
	glGetProgramiv(shader, GL_ACTIVE_UNIFORMS, &count);
	Debug::Log("There are " + std::to_string(count) + " active uniforms on shader " + fileName, __FILE__, __LINE__);

	//--------------------------------------------------------------------------------------------------------

	GLint maxUniformLength = 0;
	glGetProgramiv(shader, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLength);

	GLchar* name = new GLchar[maxUniformLength];
	GLint size;
	GLenum type;
	GLsizei length;

	for(GLuint i = 0; i < static_cast<GLuint>(count); i++){
		glGetActiveUniform(shader, i, 1024, &length, &size, &type, name);

		if(size <= 1){
			uniforms[name] = new Uniform(this, name);
			continue;
		}

		//Handle arrays of uniforms
		std::string uniformName = std::string(name).substr(0, length - 3);
		for(int j = 0; j < size; j++){
			std::string newName = uniformName + "[" + std::to_string(j) + "]";
			uniforms[newName] = new Uniform(this, newName);
		}
	}

	//We allocated our character buffer with new[] so we have to delete it with delete[]
	delete[] name;

	if(maxTextureUnits == 0){
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	}

	return true;
}

void Shader::Unload(){
	for(auto& u : uniforms){
		delete u.second;
		u.second = nullptr;
	}

	uniforms.clear();

	if(shader != 0){
		glDeleteProgram(shader);
		shader = 0;
	}
}

std::string Shader::GetShaderLog(GLuint shader_){
	std::string errorLog;

	GLsizei errorLogSize = 0;
	glGetProgramiv(shader_, GL_INFO_LOG_LENGTH, &errorLogSize);
	errorLog.resize(errorLogSize);
	glGetProgramInfoLog(shader_, errorLogSize, &errorLogSize, &errorLog[0]);

	return errorLog;
}

unsigned int Shader::Program() const{
	return shader;
}

void Shader::Use(){
	glUseProgram(shader);
}

void Shader::Unbind(){
	int boundTextures = GL_TEXTURE0 + currentBoundTextures;
	for(int i = GL_TEXTURE0; i < boundTextures; i++){
		UnbindTexture(i);
	}

	glUseProgram(0);
}

GLuint Shader::GetUniformID(const std::string& uniformName_) const{
	_ASSERT(!uniformName_.empty());
	_ASSERT(glGetUniformLocation(Program(), uniformName_.c_str()) != -1);
	return glGetUniformLocation(Program(), uniformName_.c_str());
}

void Shader::BindInt(GLuint uniformID_, int value_){
	glUniform1iv(uniformID_, 1, &value_);
}

void Shader::BindInt(const Uniform& uniform_, int value_){
	BindInt(uniform_.id, value_);
}

void Shader::BindInt(const std::string& uniformName_, int value_){
	auto search = uniforms.find(uniformName_);
	if(search != uniforms.end()){
		BindInt(search->second->id, value_);
		return;
	}

	Debug::LogWarning("Uniform [" + uniformName_ + "] not found in uniform list!", __FILE__, __LINE__);
	BindInt(GetUniformID(uniformName_), value_);
}

void Shader::BindFloat(GLuint uniformID_, float value_){
	glUniform1fv(uniformID_, 1, &value_);
}

void Shader::BindFloat(const Uniform& uniform_, float value_){
	BindFloat(uniform_.id, value_);
}

void Shader::BindFloat(const std::string& uniformName_, float value_){
	auto search = uniforms.find(uniformName_);
	if(search != uniforms.end()){
		BindFloat(search->second->id, value_);
		return;
	}

	Debug::LogWarning("Uniform [" + uniformName_ + "] not found in uniform list!", __FILE__, __LINE__);
	BindFloat(GetUniformID(uniformName_), value_);
}

void Shader::BindVector2(GLuint uniformID_, const Vector2& value_){
	glUniform2fv(uniformID_, 1, value_);
}

void Shader::BindVector2(const Uniform& uniform_, const Vector2& value_){
	BindVector2(uniform_.id, value_);
}

void Shader::BindVector2(const std::string& uniformName_, const Vector2& value_){
	auto search = uniforms.find(uniformName_);
	if(search != uniforms.end()){
		BindVector2(search->second->id, value_);
		return;
	}

	Debug::LogWarning("Uniform [" + uniformName_ + "] not found in uniform list!", __FILE__, __LINE__);
	BindVector2(GetUniformID(uniformName_), value_);
}

void Shader::BindVector3(GLuint uniformID_, const Vector3& value_){
	glUniform3fv(uniformID_, 1, value_);
}

void Shader::BindVector3(const Uniform& uniform_, const Vector3& value_){
	BindVector3(uniform_.id, value_);
}

void Shader::BindVector3(const std::string& uniformName_, const Vector3& value_){
	auto search = uniforms.find(uniformName_);
	if(search != uniforms.end()){
		BindVector3(search->second->id, value_);
		return;
	}

	Debug::LogWarning("Uniform [" + uniformName_ + "] not found in uniform list!", __FILE__, __LINE__);
	BindVector3(GetUniformID(uniformName_), value_);
}

void Shader::BindColor(GLuint uniformID_, const Color& value_){
	glUniform4fv(uniformID_, 1, value_);
}

void Shader::BindColor(const Uniform& uniform_, const Color& value_){
	BindColor(uniform_.id, value_);
}

void Shader::BindColor(const std::string& uniformName_, const Color& value_){
	auto search = uniforms.find(uniformName_);
	if(search != uniforms.end()){
		BindColor(search->second->id, value_);
		return;
	}

	Debug::LogWarning("Uniform [" + uniformName_ + "] not found in uniform list!", __FILE__, __LINE__);
	BindColor(GetUniformID(uniformName_), value_);
}

void Shader::BindMatrix3(GLuint uniformID_, const Matrix3& value_){
	glUniformMatrix3fv(uniformID_, 1, GL_FALSE, value_);
}

void Shader::BindMatrix3(const Uniform& uniform_, const Matrix3& value_){
	BindMatrix3(uniform_.id, value_);
}

void Shader::BindMatrix3(const std::string& uniformName_, const Matrix3& value_){
	auto search = uniforms.find(uniformName_);
	if(search != uniforms.end()){
		BindMatrix3(search->second->id, value_);
		return;
	}

	Debug::LogWarning("Uniform [" + uniformName_ + "] not found in uniform list!", __FILE__, __LINE__);
	BindMatrix3(GetUniformID(uniformName_), value_);
}

void Shader::BindMatrix4(GLuint uniformID_, const Matrix4& value_){
	glUniformMatrix4fv(uniformID_, 1, GL_FALSE, value_);
}

void Shader::BindMatrix4(const Uniform& uniform_, const Matrix4& value_){
	BindMatrix4(uniform_.id, value_);
}

void Shader::BindMatrix4(const std::string& uniformName_, const Matrix4& value_){
	auto search = uniforms.find(uniformName_);
	if(search != uniforms.end()){
		BindMatrix4(search->second->id, value_);
		return;
	}

	Debug::LogWarning("Uniform [" + uniformName_ + "] not found in uniform list!", __FILE__, __LINE__);
	BindMatrix4(GetUniformID(uniformName_), value_);
}

void Shader::BindTexture(int textureNumber_, GLuint textureID_){
	glActiveTexture(textureNumber_);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID_);
	glDisable(GL_TEXTURE_2D);
	currentBoundTextures++;
}

void Shader::BindTexture(const Uniform& uniform_, GLuint textureID_){
	if(currentBoundTextures >= maxTextureUnits){
		Debug::LogError("Attempted to bind invalid texture number!", __FILE__, __LINE__);
		return;
	}

	glUniform1i(uniform_.id, currentBoundTextures);
	BindTexture(GL_TEXTURE0 + currentBoundTextures, textureID_);
}

void Shader::BindTexture(const std::string& uniformName_, GLuint textureID_){
	auto search = uniforms.find(uniformName_);
	if(search != uniforms.end()){
		BindTexture(*search->second, textureID_);
		return;
	}

	Debug::LogWarning("Uniform [" + uniformName_ + "] not found in uniform list!", __FILE__, __LINE__);
	BindTexture(GetUniformID(uniformName_), textureID_);
}

void Shader::BindTexture3D(int textureNumber_, GLuint textureID_){
	glActiveTexture(textureNumber_);
	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, textureID_);
	glDisable(GL_TEXTURE_3D);
	currentBoundTextures++;
}

void Shader::BindTexture3D(const Uniform& uniform_, GLuint textureID_){
	if(currentBoundTextures >= maxTextureUnits){
		Debug::LogError("Attempted to bind invalid texture number!", __FILE__, __LINE__);
		return;
	}

	glUniform1i(uniform_.id, currentBoundTextures);
	BindTexture3D(GL_TEXTURE0 + currentBoundTextures, textureID_);
}

void Shader::BindTexture3D(const std::string& uniformName_, GLuint textureID_){
	auto search = uniforms.find(uniformName_);
	if(search != uniforms.end()){
		BindTexture3D(*search->second, textureID_);
		return;
	}

	Debug::LogWarning("Uniform [" + uniformName_ + "] not found in uniform list!", __FILE__, __LINE__);
	BindTexture3D(GetUniformID(uniformName_), textureID_);
}

void Shader::BindCubeMap(int textureNumber_, GLuint textureID_){
	glActiveTexture(textureNumber_);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID_);
	glDisable(GL_TEXTURE_CUBE_MAP);
}

void Shader::UnbindTexture(int textureNumber_){
	glActiveTexture(textureNumber_);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	currentBoundTextures--;
}

void Shader::UnbindCubeMap(int textureNumber_){
	glActiveTexture(textureNumber_);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_TEXTURE_CUBE_MAP);
}

void Shader::BindDirectionalLight(unsigned int id_, const DirectionalLight* light_){
	BindVector3("directionalLights[" + std::to_string(id_) + "].direction", light_->GetGameObject()->GetTransform()->GetForward());
	BindColor("directionalLights[" + std::to_string(id_) + "].ambient", light_->GetAmbient());
	BindColor("directionalLights[" + std::to_string(id_) + "].diffuse", light_->GetDiffuse());
	BindColor("directionalLights[" + std::to_string(id_) + "].specular", light_->GetSpecular());
	BindFloat("directionalLights[" + std::to_string(id_) + "].lightIntensity", light_->GetIntensity());
	BindColor("directionalLights[" + std::to_string(id_) + "].lightColor", light_->GetColor());
	BindMatrix4("directionalLights[" + std::to_string(id_) + "].lightSpaceMatrix", light_->GetLightSpaceMatrix());
	BindTexture("directionalLights[" + std::to_string(id_) + "].shadowMap", light_->GetDepthMap());
}

void Shader::BindPointLight(unsigned int id_, const PointLight* light_){
	BindVector3("pointLights[" + std::to_string(id_) + "].position", light_->GetGameObject()->GlobalPosition());
	BindColor("pointLights[" + std::to_string(id_) + "].ambient", light_->GetAmbient());
	BindColor("pointLights[" + std::to_string(id_) + "].diffuse", light_->GetDiffuse());
	BindColor("pointLights[" + std::to_string(id_) + "].specular", light_->GetSpecular());
	BindFloat("pointLights[" + std::to_string(id_) + "].constant", light_->GetConstant());
	BindFloat("pointLights[" + std::to_string(id_) + "].linear", light_->GetLinear());
	BindFloat("pointLights[" + std::to_string(id_) + "].quadratic", light_->GetQuadratic());
	BindFloat("pointLights[" + std::to_string(id_) + "].lightIntensity", light_->GetIntensity());
	BindColor("pointLights[" + std::to_string(id_) + "].lightColor", light_->GetColor());
}

void Shader::BindSpotLight(unsigned int id_, const SpotLight* light_){
	BindVector3("spotLights[" + std::to_string(id_) + "].position", light_->GetGameObject()->GlobalPosition());
	BindVector3("spotLights[" + std::to_string(id_) + "].direction", light_->GetGameObject()->GetTransform()->GetForward());
	BindColor("spotLights[" + std::to_string(id_) + "].ambient", light_->GetAmbient());
	BindColor("spotLights[" + std::to_string(id_) + "].diffuse", light_->GetDiffuse());
	BindColor("spotLights[" + std::to_string(id_) + "].specular", light_->GetSpecular());
	BindFloat("spotLights[" + std::to_string(id_) + "].constant", light_->GetConstant());
	BindFloat("spotLights[" + std::to_string(id_) + "].linear", light_->GetLinear());
	BindFloat("spotLights[" + std::to_string(id_) + "].quadratic", light_->GetQuadratic());
	BindFloat("spotLights[" + std::to_string(id_) + "].cutOff", light_->GetCutOff());
	BindFloat("spotLights[" + std::to_string(id_) + "].outerCutOff", light_->GetOuterCutOff());
	BindFloat("spotLights[" + std::to_string(id_) + "].lightIntensity", light_->GetIntensity() * 1000.0f);
	BindColor("spotLights[" + std::to_string(id_) + "].lightColor", light_->GetColor());
	BindMatrix4("spotLights[" + std::to_string(id_) + "].lightSpaceMatrix", light_->GetLightSpaceMatrix());
	BindTexture("spotLights[" + std::to_string(id_) + "].shadowMap", light_->GetDepthMap());
}
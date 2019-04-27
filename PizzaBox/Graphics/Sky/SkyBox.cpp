#include "SkyBox.h"

#include "Core/Time.h"
#include "Graphics/RenderEngine.h"
#include "Math/Matrix.h"
#include "Resource/ResourceManager.h"

using namespace PizzaBox;

SkyBox::SkyBox(const std::string& skyboxFile_, float size_, float rotateSpeed_, const std::string& shaderName_) : Sky(shaderName_, size_, rotateSpeed_), fileName(skyboxFile_), file(nullptr){
}

SkyBox::~SkyBox(){
}

bool SkyBox::Initialize(){
	shader = ResourceManager::LoadResource<Shader>(shaderName);
	if(shader == nullptr){
		Debug::LogError("Could not load shader " + shaderName + "!", __FILE__, __LINE__);
		return false;
	}

	projectionMatrixID = new Uniform(shader, "projectionMatrix");
	viewMatrixID = new Uniform(shader, "viewMatrix");
	scaleMatrixID = new Uniform(shader, "scaleMatrix");
	fogColorID = new Uniform(shader, "fogColor");
	limitsID = new Uniform(shader, "limits");
	useFogID = new Uniform(shader, "useFog");

	file = ResourceManager::LoadResource<SkyBoxResource>(fileName);
	if(file == nullptr){
		Debug::LogError("Could not load SkyboxResource " + fileName + "!", __FILE__, __LINE__);
		return false;
	}   

	vao.Bind();
	vbo.Bind();

	vbo.SetBufferData(sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	vao.SetupVertexAttribute(0, 3, 3 * sizeof(float), (GLvoid*)(0));

	vbo.Unbind();
	vao.Unbind();
	 
	return true;
}

void SkyBox::Destroy(){ 
	if(fogColorID != nullptr){
		delete fogColorID;
		fogColorID = nullptr;
	}

	if(limitsID != nullptr){
		delete limitsID;
		limitsID = nullptr;
	}

	if(useFogID != nullptr){
		delete useFogID;
		useFogID = nullptr;
	}

	if(projectionMatrixID != nullptr){
		delete projectionMatrixID;
		projectionMatrixID = nullptr;
	}

	if(viewMatrixID != nullptr){
		delete viewMatrixID;
		viewMatrixID = nullptr;
	} 

	if(scaleMatrixID != nullptr){
		delete scaleMatrixID;
		scaleMatrixID = nullptr;
	}

	if(file != nullptr){
		ResourceManager::UnloadResource(fileName);
		file = nullptr;
	}

	if(shader != nullptr){
		ResourceManager::UnloadResource(shaderName);
		shader = nullptr;
	}
}

GLuint SkyBox::GetSkyboxTexture(){
	return file->TextureID();
}

void SkyBox::Render(Camera* camera_){ 
	glDepthFunc(GL_LEQUAL);
	shader->Use();
	   
	Matrix4 viewMatrix = Matrix4(Matrix3(camera_->GetViewMatrix()));
	//Handles rotation
	rotation += rotateSpeed * Time::DeltaTime();
	viewMatrix *= Matrix4::Rotate(rotation, Vector3(0.0f, 1.0f, 0.0f));

	//Handles Scale 
	if(camera_->GetRenderMode() == Camera::RenderMode::Orthographic && size > 20.0f){
		SetSize(20.0f);
	}else if(camera_->GetRenderMode() == Camera::RenderMode::Perspective && size < 1000.0f){
		SetSize(1000.0f);
	} 

	Matrix4 scaleMatrix = Matrix4::Identity() * Matrix4::Scale(Vector3(size, size, size));
	shader->BindMatrix4(scaleMatrixID->id, scaleMatrix);
	shader->BindMatrix4(projectionMatrixID->id, camera_->GetProjectionMatrix());
	shader->BindMatrix4(viewMatrixID->id, viewMatrix);
	
	shader->BindCubeMap(GL_TEXTURE0, file->TextureID());
	shader->BindColor(fogColorID->id, RenderEngine::GetFogColor());
	shader->BindInt(useFogID->id, useFog);
	shader->BindVector2(limitsID->id, limits);

	vao.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36); 
	vao.Unbind();
	shader->UnbindCubeMap(GL_TEXTURE0);

	glDepthFunc(GL_LESS);
}
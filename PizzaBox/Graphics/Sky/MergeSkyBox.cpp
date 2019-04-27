#include "MergeSkyBox.h"

#include "Core/Time.h"
#include "Graphics/RenderEngine.h"
#include "Resource/ResourceManager.h"

using namespace PizzaBox;

MergeSkyBox::MergeSkyBox(const std::string& skyboxFile_, const std::string& skyboxFile2_, float size_, float rotateSpeed_, const std::string& shaderName_) : Sky(shaderName_, size_, rotateSpeed_), fileName(skyboxFile_), fileName2(skyboxFile2_), file(nullptr), file2(nullptr), time(0.0f), blendFactorID(nullptr), skybox1ID(false), skybox2ID(false){
}
 
MergeSkyBox::~MergeSkyBox(){
}
 
bool MergeSkyBox::Initialize(){
	shader = ResourceManager::LoadResource<Shader>(shaderName);
	if(shader == nullptr){
		Debug::LogError("Could not load shader " + shaderName + "!", __FILE__, __LINE__);
		return false;
	}

	projectionMatrixID = new Uniform(shader, "projectionMatrix");
	viewMatrixID = new Uniform(shader, "viewMatrix");
	scaleMatrixID = new Uniform(shader, "scaleMatrix");
	blendFactorID = new Uniform(shader, "blendFactor");
	fogColorID = new Uniform(shader, "fogColor");
	limitsID = new Uniform(shader, "limits");
	useFogID = new Uniform(shader, "useFog");
	skybox1ID = new Uniform(shader, "skybox1");
	skybox2ID = new Uniform(shader, "skybox2");

	file = ResourceManager::LoadResource<SkyBoxResource>(fileName);
	if(file == nullptr){
		Debug::LogError("Could not load SkyboxResource " + fileName + "!", __FILE__, __LINE__);
		return false;
	}

	file2 = ResourceManager::LoadResource<SkyBoxResource>(fileName2);
	if(file2 == nullptr){
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

void MergeSkyBox::Destroy(){ 
	if(file != nullptr){
		ResourceManager::UnloadResource(fileName);
		file = nullptr;
	}

	if(file2 != nullptr){
		ResourceManager::UnloadResource(fileName2);
		file2 = nullptr;
	}

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

	if(shader != nullptr){
		ResourceManager::UnloadResource(shaderName);
		shader = nullptr;
	}
}

void MergeSkyBox::Render(Camera* camera){
	glDepthFunc(GL_LEQUAL);
	shader->Use();

	Matrix4 viewMatrix = Matrix4(Matrix3(camera->GetViewMatrix()));
	//handles rotation
	rotation += rotateSpeed * Time::DeltaTime();
	viewMatrix *= Matrix4::Rotate(rotation, Vector3(0.0f, 1.0f, 0.0f)); 

	//handles Scale 
	if(camera->GetRenderMode() == Camera::RenderMode::Orthographic && size > 20.0f){
		SetSize(20.0f);
	}else if(camera->GetRenderMode() == Camera::RenderMode::Perspective && size < 1000.0f){
		SetSize(1000.0f);
	}

	Matrix4 scaleMatrix = Matrix4::Identity() * Matrix4::Scale(Vector3(size, size, size));
	shader->BindMatrix4(scaleMatrixID->id, scaleMatrix);
	shader->BindMatrix4(projectionMatrixID->id, camera->GetProjectionMatrix());
	shader->BindMatrix4(viewMatrixID->id, viewMatrix);
	shader->BindColor(fogColorID->id, RenderEngine::GetFogColor());
	shader->BindInt(useFogID->id, useFog);
	shader->BindVector2(limitsID->id, limits);
	vao.Bind();

	BindTexture();

	glDrawArrays(GL_TRIANGLES, 0, 36);

	vao.Unbind();

	shader->UnbindCubeMap(GL_TEXTURE0);
	shader->UnbindCubeMap(GL_TEXTURE1);
	 
	glDepthFunc(GL_LESS);
}

void MergeSkyBox::BindTexture(){
	time += Time::DeltaTime(); 
	if(time > 40.0f){
		time = 0.0f;
	}

	int texture1, texture2;
	float blendFactor;
	if(time >= 0.0f && time < 10.0f){
		texture1 = file2->TextureID();
		texture2 = file2->TextureID();
		blendFactor = (time - 0.0f) / (10.0f - 0.0f);
	}else if (time >= 10.0f && time < 20.0f){
		texture1 = file2->TextureID();
		texture2 = file->TextureID();
		blendFactor = (time - 10.0f) / (20.0f - 10.0f);
	}else if (time >= 20.0f && time < 30.0f){
		texture1 = file->TextureID();
		texture2 = file->TextureID();
		blendFactor = (time - 20.0f) / (30.0f - 20.0f);
	}else{
		texture1 = file->TextureID();
		texture2 = file2->TextureID();
		blendFactor = (time - 30.0f) / (40.0f - 30.0f);
	} 
	shader->BindFloat(blendFactorID->id, blendFactor);
	shader->BindCubeMap(GL_TEXTURE0, texture1);
	shader->BindInt(skybox1ID->id, 0);
	shader->BindCubeMap(GL_TEXTURE1, texture2);
	shader->BindInt(skybox2ID->id, 1);
}

GLuint MergeSkyBox::GetSkyboxTexture(){
	return file->TextureID();
}
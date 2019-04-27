#include "PostProcessing.h"

#include <glew.h>

#include "Graphics/RenderEngine.h"
#include "Resource/ResourceManager.h"

using namespace PizzaBox;

VAO* PostProcessing::vao = nullptr;
Buffer* PostProcessing::vbo = nullptr;
Shader* PostProcessing::screenShader = nullptr; 
std::string PostProcessing::shaderName = "";

bool PostProcessing::Initialize(std::string shaderName_){	 
	shaderName = shaderName_;
	screenShader = ResourceManager::LoadResource<Shader>(shaderName);
	if(screenShader == nullptr){
		Debug::LogError("Screen shader could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	screenShader->Use();
	screenShader->BindInt("screenTexture", 0);

	vao = new VAO();
	vbo = new Buffer(GL_ARRAY_BUFFER);

	CreateQuad();

	return true;
}

void PostProcessing::Destroy(){
	if(screenShader != nullptr){
		ResourceManager::UnloadResource(shaderName);
		screenShader = nullptr;
	}

	if(vbo != nullptr){
		delete vbo;
		vbo = nullptr;
	}

	if(vao != nullptr){
		delete vao;
		vao = nullptr;
	}
} 

void PostProcessing::OnResize(){
}

void PostProcessing::DoPostProcessing(GLuint texture_){ 
	Start();

	//Regular screen
	screenShader->BindTexture("screenTexture", texture_);
	 
	End();
} 

void PostProcessing::Start(){
	//Fixes the view port for displaying
	glViewport(static_cast<GLint>(0), static_cast<GLint>(0), static_cast<GLsizei>(RenderEngine::ScreenSize().x), static_cast<GLsizei>(RenderEngine::ScreenSize().y));

	vao->Bind();
	glDisable(GL_DEPTH_TEST);
	screenShader->Use();
}

void PostProcessing::End(){
	glDrawArrays(GL_TRIANGLES, 0, 6); 
	glEnable(GL_DEPTH_TEST);
	vao->Unbind();
	screenShader->Unbind();
}

void PostProcessing::CreateQuad(){
	//vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	float quadVertices[] = {
		//positions      //texCoords
		-1.0f,  1.0f,	  0.0f, 1.0f,
		-1.0f, -1.0f,	  0.0f, 0.0f,
		1.0f, -1.0f,	  1.0f, 0.0f,
		-1.0f,  1.0f,     0.0f, 1.0f,
		1.0f, -1.0f,     1.0f, 0.0f,
		1.0f,  1.0f,     1.0f, 1.0f
	};

	vao->Bind();
	vbo->Bind();
	vbo->SetBufferData(sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	vao->SetupVertexAttribute(0, 2, 4 * sizeof(float), (GLvoid*)(0));
	vao->SetupVertexAttribute(1, 2, 4 * sizeof(float), (GLvoid*)(2 * sizeof(float)));
}

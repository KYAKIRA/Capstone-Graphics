#include "Blur.h"

#include "Core/GameManager.h"
#include "Graphics/RenderEngine.h"
#include "Resource/ResourceManager.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

Blur::Blur(std::string shaderName_) : shader(nullptr), fbo(nullptr), shaderName(shaderName_){
}

Blur::~Blur(){
}

bool Blur::Initialize(){ 
	shader = ResourceManager::LoadResource<Shader>(shaderName);
	if(shader == nullptr){
		Debug::LogError("Shader did not load");
		return false;
	}

	fbo = new BlurFBO(RenderEngine::ScreenSize());
	if(fbo->Initialize() == false){
		Debug::LogError("Blur FBO could not be initialized!", __FILE__, __LINE__);
		return false;
	}

	return true;
}

void Blur::Destroy(){
	if(fbo != nullptr){
		fbo->Destroy();
		delete fbo;
		fbo = nullptr;
	}

	if(shader != nullptr){
		ResourceManager::UnloadResource(shaderName);
		shader = nullptr;
	}
}

void Blur::Update(Mode mode_, GLuint texture_, int blurStrength_){ 
	shader->Use();

	if(mode_ == Mode::horizontal){
		shader->BindFloat("targetWidth", static_cast<float>(RenderEngine::ScreenSize().x) / static_cast<float>(blurStrength_));
	}else if(mode_ == Mode::vertical){
		shader->BindFloat("targetHeight", static_cast<float>(RenderEngine::ScreenSize().y) / static_cast<float>(blurStrength_));
	}

	shader->BindTexture("originalTexture", texture_);

	fbo->Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	fbo->Unbind();
}

GLuint Blur::GetTexture(){
	return fbo->GetColorTexture();
} 

void Blur::OnResize(){
	if(fbo != nullptr){
		fbo->Destroy();
		delete fbo;
		fbo = nullptr;
	}

	fbo = new BlurFBO(RenderEngine::ScreenSize());
	if(fbo->Initialize() == false){
		Debug::DisplayFatalErrorMessage("Initialization Error", "Horizontal Blur FBO could not be initialized!");
		GameManager::Stop();
	}
}
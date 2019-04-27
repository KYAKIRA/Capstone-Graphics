#include "ImageUI.h"

#include <rttr/registration.h>

#include "Math/Math.h"
#include "Math/Matrix.h"
#include "Resource/ResourceManager.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<ImageUI>("ImageUI")
		.constructor<std::string, std::string>()
		.constructor<std::string, std::string, const Rect&, bool>()
		.constructor<std::string, std::string, const Rect&, bool, Script*>()
		.method("Initialize", &ImageUI::Initialize)
		.method("Destroy", &ImageUI::Destroy)
		.method("Render", &ImageUI::Render)
		.method("SetTransparency", &ImageUI::SetTransparency);
}
#pragma warning( pop )

ImageUI::ImageUI(const std::string& name_, const std::string& textureName_, const Rect& rect_, bool clamp_, Script* script_) : UIElement(name_, rect_, script_), transparency(1.0f), textureName(textureName_), texture(nullptr), shader(nullptr), vao(), vbo(GL_ARRAY_BUFFER){
	projectionUniform = nullptr;
	transparencyUniform = nullptr;

	clampToScreen = clamp_;
	Clamp();
}

ImageUI::ImageUI(const std::string& name_, const std::string& textureName_, const UIEventFunc& function_, const Rect& rect_, bool clamp_) : UIElement(name_, rect_, function_), transparency(1.0f), textureName(textureName_), texture(nullptr), shader(nullptr), vao(), vbo(GL_ARRAY_BUFFER){
	projectionUniform = nullptr;
	transparencyUniform = nullptr;

	clampToScreen = clamp_;
	Clamp();
}

ImageUI::~ImageUI(){
}

bool ImageUI::Initialize(){
	shader = ResourceManager::LoadResource<Shader>("UITextureShader");
	if(shader == nullptr){
		Debug::LogError("Could not load texture shader!");
		return false;
	}

	texture = ResourceManager::LoadResource<Texture>(textureName);
	if(texture == nullptr){
		Debug::LogError("Texture " + textureName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	//Configure VAO/VBO for texture quads
	vao.Bind();
	vbo.Bind();

	vbo.SetBufferData(sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	vao.SetupVertexAttribute(0, 4, 4 * sizeof(GLfloat), 0);

	vbo.Unbind();
	vao.Unbind();

	projectionUniform = new Uniform(shader, "projection");
	transparencyUniform = new Uniform(shader, "transparency");

	return true;
}

void ImageUI::Destroy(){
	if(projectionUniform != nullptr){
		delete projectionUniform;
		projectionUniform = nullptr;
	}

	if(transparencyUniform != nullptr){
		delete transparencyUniform;
		transparencyUniform = nullptr;
	}

	if(texture != nullptr){
		ResourceManager::UnloadResource(textureName);
		texture = nullptr;
	}

	if(shader != nullptr){
		ResourceManager::UnloadResource("UITextureShader");
		shader = nullptr;
	}
}

void ImageUI::Render(){
	float x = rect.x;
	float y = rect.y;
	float w = rect.w / 2.0f;
	float h = rect.h / 2.0f;

	//Update VBO for each character
	Vector4 vertices[6] = {
		Vector4(x - w,	y + h,	0.0f, 0.0f),
		Vector4(x - w,	y - h,	0.0f, 1.0f),
		Vector4(x + w,	y - h,	1.0f, 1.0f),

		Vector4(x - w,	y + h,	0.0f, 0.0f),
		Vector4(x + w,	y - h,	1.0f, 1.0f),
		Vector4(x + w,	y + h,	1.0f, 0.0f)
	};

	vbo.Bind();
	vbo.SetBufferSubData(0, sizeof(vertices), vertices);
	vbo.Unbind();

	shader->Use();
	shader->BindMatrix4(projectionUniform->id, Matrix4::Orthographic(0.0f, static_cast<float>(1.0f), 0.0f, static_cast<float>(1.0f)));
	shader->BindTexture(GL_TEXTURE0, texture->TextureID());
	shader->BindFloat(transparencyUniform->id, transparency);
	vao.Bind();

	glDrawArrays(GL_TRIANGLES, 0, 6);

	shader->BindTexture(GL_TEXTURE0, 0);
	vao.Unbind();
}

void ImageUI::SetTransparency(float transparency_){
	transparency = Math::Clamp(0.0f, 1.0f, transparency_);
}
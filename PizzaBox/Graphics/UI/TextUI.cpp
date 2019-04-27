#include "TextUI.h"

#include <rttr/registration.h>

#include "Graphics/RenderEngine.h"
#include "Math/Math.h"
#include "Math/Matrix.h"
#include "Resource/ResourceManager.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<TextUI>("TextUI")
		.constructor<const std::string&, const Rect&, bool, const std::string&, const std::string&>()
		.constructor<const std::string&, const Rect&, bool, const std::string&, const std::string&, Script*>()
		.method("Initialize", &TextUI::Initialize)
		.method("Destroy", &TextUI::Destroy)
		.method("Render", &TextUI::Render)
		.method("GetText", &TextUI::GetText)
		.method("GetColor", &TextUI::GetColor)
		.method("SetText", &TextUI::SetText)
		.method("SetColor", &TextUI::SetColor);
}
#pragma warning( pop )

TextUI::TextUI(const std::string& name_, const Rect& rect_, bool clamp_, const std::string& text_, const std::string& fontName_, Script* script_) : UIElement(name_, rect_, script_), text(text_), fontName(fontName_), font(nullptr), textShader(nullptr), color(Color::White), vao(), vbo(GL_ARRAY_BUFFER){
	_ASSERT(!fontName.empty());
	textColorUniform = nullptr;
	projectionUniform = nullptr;
	
	clampToScreen = clamp_;
	Clamp();
}

TextUI::TextUI(const std::string& name_, const Rect& rect_, bool clamp_, const std::string& text_, const std::string& fontName_, const UIEventFunc& function_) : UIElement(name_, rect_, function_), text(text_), fontName(fontName_), font(nullptr), textShader(nullptr), color(Color::White), vao(), vbo(GL_ARRAY_BUFFER){
	_ASSERT(!fontName.empty());
	textColorUniform = nullptr;
	projectionUniform = nullptr;

	clampToScreen = clamp_;
	Clamp();
}

TextUI::~TextUI(){
#ifdef _DEBUG
	if(font != nullptr || textShader != nullptr || textColorUniform != nullptr || projectionUniform != nullptr){
		Debug::LogWarning("Memory leak detected in TextUI!", __FILE__, __LINE__);
		Destroy();
	}
#endif //_DEBUG
}

bool TextUI::Initialize(){
	textShader = ResourceManager::LoadResource<Shader>("TextShader");
	if(textShader == nullptr){
		Debug::LogError("Could not load text shader!");
		return false;
	}

	font = ResourceManager::LoadResource<Font>(fontName);
	if(font == nullptr){
		Debug::LogError("Font " + fontName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	//Configure VAO/VBO for texture quads
	vao.Bind();
	vbo.Bind();

	vbo.SetBufferData(sizeof(GLfloat) * 6 * 4, nullptr, GL_STATIC_DRAW);
	vao.SetupVertexAttribute(0, 4, 4 * sizeof(GLfloat), 0);

	vbo.Unbind();
	vao.Unbind();

	textColorUniform = new Uniform(textShader, "textColor");
	projectionUniform = new Uniform(textShader, "projection");

	return true;
}

void TextUI::Destroy(){
	if(textColorUniform != nullptr){
		delete textColorUniform;
		textColorUniform = nullptr;
	}

	if(projectionUniform != nullptr){
		delete projectionUniform;
		projectionUniform = nullptr;
	}

	if(font != nullptr){
		ResourceManager::UnloadResource(fontName);
		font = nullptr;
	}

	if(textShader != nullptr){
		ResourceManager::UnloadResource("TextShader");
		textShader = nullptr;
	}
}

void TextUI::Render(){
	//Activate corresponding render state
	textShader->Use();
	textShader->BindColor(textColorUniform->id, color);

	//This really isn't the best solution but it works for now
	textShader->BindMatrix4(projectionUniform->id, Matrix4::Orthographic(0.0f, 1.0f, 0.0f, 1.0f));

	vao.Bind();

	int totalWidthInPixels = 0;
	int biggestHeightInPixels = 0;
	for(char c : text){
		FontCharacter ch = font->Characters().at(c);
		totalWidthInPixels += ch.sizeX + ch.offsetX;

		if(ch.sizeY - ch.offsetY > biggestHeightInPixels){
			biggestHeightInPixels = ch.sizeY - ch.offsetY;
		}
	}

	float screenWidthPerPixel = rect.w / totalWidthInPixels;

	float x = rect.x - (rect.w / 2.0f);
	float y = rect.y + (rect.h / 2.0f);
	float width = screenWidthPerPixel;
	float height = rect.h / 100.0f;

	//Iterate through all characters in text
	for(char c : text){
		FontCharacter ch = font->Characters().at(c);

		GLfloat xpos = x + ch.offsetX * width;
		GLfloat ypos = y - (ch.sizeY - ch.offsetY) * height;

		GLfloat w = ch.sizeX * width;
		GLfloat h = ch.sizeY * height;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,		0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};

		//Don't bind the texture if it's a space, for some reason doing so causes OpenGL warnings
		if(c != 32){
			//Render glyph texture over quad
			textShader->BindTexture(GL_TEXTURE0, ch.textureID); 
		}

		//Update content of VBO memory
		vbo.Bind();
		vbo.SetBufferSubData(0, sizeof(vertices), vertices);
		vbo.Unbind();

		//Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.next >> 6) * width; //Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels)) 
	}

	vao.Unbind();

	glBindTexture(GL_TEXTURE_2D, 0);
}

std::string TextUI::GetText(){
	return text;
}

Color TextUI::GetColor(){
	return color;
}

void TextUI::SetText(const std::string& text_){
	text = text_;
}

void TextUI::SetColor(const Color& color_){
	color = color_;
}
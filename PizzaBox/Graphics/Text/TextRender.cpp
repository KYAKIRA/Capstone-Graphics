#include "TextRender.h"

#include "FontEngine.h"
#include "Graphics/RenderEngine.h"
#include "Math/Matrix.h"
#include "Resource/ResourceManager.h"

using namespace PizzaBox;

TextRender::TextRender(const std::string fontResource_, const std::string text_, float scale_, const Color& color_) : Component(), fontResource(fontResource_), text(text_), scale(scale_), color(color_), font(nullptr), textShader(nullptr), vao(), vbo(GL_ARRAY_BUFFER){
	_ASSERT(!fontResource.empty());
	textColorUniform = nullptr;
	projectionUniform = nullptr;
}

TextRender::~TextRender(){
	#ifdef _DEBUG
	if(font != nullptr || textShader != nullptr || textColorUniform != nullptr || projectionUniform != nullptr){
		Debug::LogWarning("Memory leak detected in TextRender!", __FILE__, __LINE__);
		Destroy();
	}
	#endif //_DEBUG
}

bool TextRender::Initialize(GameObject* go_){
	_ASSERT(go_ != nullptr);
	gameObject = go_;

	textShader = ResourceManager::LoadResource<Shader>("TextShader");
	if(textShader == nullptr){
		Debug::LogError("Could not load shader!");
		return false;
	}

	font = ResourceManager::LoadResource<Font>(fontResource);
	if(font == nullptr){
		Debug::LogError("Could not load font!");
		return false;
	}

	// Configure VAO/VBO for texture quads 
	vao.Bind();
	vbo.Bind(); 

	vbo.SetBufferData(sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	vao.SetupVertexAttribute(0, 4, 4 * sizeof(GLfloat), (GLvoid*)(0));

	vbo.Unbind();
	vao.Bind();

	textColorUniform = new Uniform(textShader, "textColor");
	projectionUniform = new Uniform(textShader, "projection");

	RenderEngine::RegisterTextRender(this);
	return true;
}

void TextRender::Destroy(){
	RenderEngine::UnregisterTextRender(this);

	if(textColorUniform != nullptr){
		delete textColorUniform;
		textColorUniform = nullptr;
	}

	if(projectionUniform != nullptr){
		delete projectionUniform;
		projectionUniform = nullptr;
	}

	if(font != nullptr){
		ResourceManager::UnloadResource(fontResource);
		font = nullptr;
	}

	if(textShader != nullptr){
		ResourceManager::UnloadResource("TextShader");
		textShader = nullptr;
	}
}

std::string TextRender::GetText() const{
	return text;
}

float TextRender::GetScale() const{
	return scale;
}

Color TextRender::GetColor() const{
	return color;
}

void TextRender::SetText(const std::string& text_){
	text = text_;
}

void TextRender::SetScale(float scale_){
	scale = scale_;
}

void TextRender::SetColor(const Color& color_){
	color = color_;
}

void TextRender::Render(Camera* camera_){
	float x = gameObject->GetTransform()->GetPosition().x;
	float y = gameObject->GetTransform()->GetPosition().y;

	//Activate corresponding render state	
	textShader->Use();
	textShader->BindColor(*textColorUniform, color);
	//This really isn't the best solution but it works for now 
	ScreenCoordinate ss = RenderEngine::ScreenSize();
	textShader->BindMatrix4(*projectionUniform, Matrix4::Orthographic(0.0f, static_cast<float>(ss.x), 0.0f, static_cast<float>(ss.y)));
	glActiveTexture(GL_TEXTURE0);
	vao.Bind();

	//Iterate through all characters in text
	for(char c : text){
		FontCharacter ch = font->Characters().at(c);

		GLfloat xpos = x + ch.offsetX * scale;
		GLfloat ypos = y - (ch.sizeY - ch.offsetY) * scale;

		GLfloat w = ch.sizeX * scale;
		GLfloat h = ch.sizeY * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },            
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }           
		};

		//Don't bind the texture if it's a space, for some reason doing so causes OpenGL warnings
		if(c != 32){
			//Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.textureID);
		}
		// Update content of VBO memory 
		vbo.Bind();
		vbo.SetBufferSubData(0, 6 * 4 * sizeof(GLfloat), vertices);  
		vbo.Unbind();

		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.next >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}  
	vao.Unbind();
	glBindTexture(GL_TEXTURE_2D, 0);
}
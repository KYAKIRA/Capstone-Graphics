#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include "Font.h"
#include "Graphics/Camera.h"
#include "Graphics/Color.h"
#include "Graphics/Shader.h"
#include "Graphics/LowLevel/VAO.h"
#include "Graphics/LowLevel/Buffer.h"
#include "Object/Component.h"

namespace PizzaBox{
	class TextRender : public Component{
	public:
		TextRender(const std::string fontResource_, const std::string text_ = "TextRender", float scale_ = 1.0f, const Color& color_ = Color::White);
		~TextRender() override;

		bool Initialize(GameObject* go_) override;
		void Destroy() override;

		std::string GetText() const;
		float GetScale() const;
		Color GetColor() const;

		void SetText(const std::string& text_);
		void SetScale(float scale_);
		void SetColor(const Color& color_);

		void Render(Camera* camera_);

	private:
		std::string fontResource;
		std::string text;
		float scale;
		Color color;
		Font* font;
		Shader* textShader;
		VAO vao;
		Buffer vbo;
		Uniform* textColorUniform;
		Uniform* projectionUniform;
	};
}

#endif //!TEXT_RENDER_H
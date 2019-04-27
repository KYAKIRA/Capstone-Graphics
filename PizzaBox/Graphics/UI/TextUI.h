#ifndef TEXT_UI_H
#define TEXT_UI_H

#include "UIElement.h"
#include "Graphics/Color.h"
#include "Graphics/Shader.h"
#include "Graphics/LowLevel/Buffer.h"
#include "Graphics/LowLevel/VAO.h"
#include "Graphics/Text/Font.h"

namespace PizzaBox{
	class TextUI : public UIElement{
	public:
		TextUI(const std::string& name_, const Rect& rect_, bool clamp_, const std::string& text_, const std::string& fontName_, Script* script_ = nullptr);
		TextUI(const std::string& name_, const Rect& rect_, bool clamp_, const std::string& text_, const std::string& fontName_, const UIEventFunc& function_);
		virtual ~TextUI() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;
		
		virtual void Render() override;

		std::string GetText();
		Color GetColor();

		void SetText(const std::string& text_);
		void SetColor(const Color& color_);

	private:
		std::string text, fontName;
		Font* font;
		Shader* textShader;
		Color color;
		VAO vao;
		Buffer vbo;
		Uniform* textColorUniform, *projectionUniform;
	};
}

#endif //!TEXT_UI_H
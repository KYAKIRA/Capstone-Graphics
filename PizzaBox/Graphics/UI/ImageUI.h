#ifndef IMAGE_UI_H
#define IMAGE_UI_H

#include "UIElement.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/LowLevel/Buffer.h"
#include "Graphics/LowLevel/VAO.h"

namespace PizzaBox{
	class ImageUI : public UIElement{
	public:
		ImageUI(const std::string& name_, const std::string& textureName_, const Rect& rect_ = Rect(), bool clamp_ = true, Script* script_ = nullptr);
		ImageUI(const std::string& name_, const std::string& textureName_, const UIEventFunc& function_, const Rect& rect_ = Rect(), bool clamp_ = true);
		virtual ~ImageUI() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;

		virtual void Render() override;

		void SetTransparency(float transparency_);
	
	private:
		float transparency;
		std::string textureName;
		Texture* texture;
		Shader* shader;
		VAO vao;
		Buffer vbo;
		Uniform* projectionUniform, *transparencyUniform;
	};
}

#endif //!IMAGE_UI_H
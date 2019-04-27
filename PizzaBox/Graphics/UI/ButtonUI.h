#ifndef BUTTON_UI_H
#define BUTTON_UI_H

#include "ImageUI.h"
#include "TextUI.h"
#include "UIElement.h"

namespace PizzaBox{
	class ButtonUI : public UIElement{
	public:
		ButtonUI(const std::string& name_, const Rect& rect_ = Rect(), bool clamp_ = true, const std::string& image_ = "DefaultButtonTexture", Script* script_ = nullptr);
		ButtonUI(const std::string& name_, const UIEventFunc& function_, const Rect& rect_ = Rect(), bool clamp = true, const std::string& image_ = "DefaultButtonTexture");
		~ButtonUI();

		virtual bool Initialize() override;
		virtual void Destroy() override;

		virtual void Render() override;

		virtual void OnSelect() override;
		virtual void OnDeselect() override;

		inline void SetTransparency(float transparency_){
			transparency = transparency_;

			if(imageUI != nullptr){
				imageUI->SetTransparency(transparency_);
			}

			if(textUI != nullptr){
				Color c = textUI->GetColor();
				textUI->SetColor(Color(c.r, c.g, c.b, transparency));
			}
		}

		void AddText(const std::string& words_);

	private:
		float transparency;
		std::string imageName;
		std::string fontName;
		std::string text;
		TextUI* textUI;
		ImageUI* imageUI;
	};
}

#endif //!BUTTON_UI_H
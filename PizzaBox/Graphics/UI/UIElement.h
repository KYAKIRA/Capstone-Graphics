#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <string>
#include <functional>

#include "Math/Rect.h"
#include "Script/Script.h"

typedef std::function<void(PizzaBox::UIEvent)> UIEventFunc;

namespace PizzaBox{
	class UIElement{
	public:
		UIElement(std::string name_, const Rect& rect_ = Rect(), Script* obj_ = nullptr) : name(name_), rect(rect_), isEnabled(true), script(obj_), function([](PizzaBox::UIEvent){}), clampToScreen(true), isSelectable(false), receivesBackInput(false){}
		UIElement(const std::string& name_, const Rect& rect_, const UIEventFunc& function_) : name(name_), rect(rect_), isEnabled(true), script(nullptr), function(function_), clampToScreen(true), isSelectable(false), receivesBackInput(false){}
		virtual ~UIElement(){}

		std::string name;
		Rect rect;

		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;

		virtual void Render() = 0;

		virtual void OnClick(){
			if(script != nullptr){
				script->OnUI(UIEvent::OnClick);
			}else if(function != nullptr){
				function(UIEvent::OnClick);
			}
		}

		virtual void OnRelease(){
			if(script != nullptr){
				script->OnUI(UIEvent::OnRelease);
			}else if(function != nullptr){
				function(UIEvent::OnRelease);
			}
		}

		virtual void OnSelect(){}
		virtual void OnDeselect(){}

		inline void SetEnable(bool isEnabled_){ isEnabled = isEnabled_; };
		inline bool GetEnable() const{ return isEnabled; };

		inline std::string GetName(){ return name; }
		inline Rect GetRect(){ return rect; }
		inline Script* GetScript(){ return script; };
		inline UIEventFunc GetFunction(){ return function; }
		inline bool IsSelectable(){ return isSelectable; }
		inline bool ReceivesBackInput(){ return receivesBackInput; }

		inline void SetName(const std::string& name_){ name = name_; }
		inline void SetRect(const Rect& rect_){ rect = rect_; }
		inline void SetScript(Script* script_){ script = script_; };
		inline void SetFunction(const UIEventFunc& function_){ function = function_; }
		inline void SetIsSelectable(bool isSelectable_){ isSelectable = isSelectable_; }
		inline void SetReceivesBackInput(bool receivesBackInput_){ receivesBackInput = receivesBackInput_; }

	protected:
		Script* script;
		std::function<void(PizzaBox::UIEvent)> function;
		bool clampToScreen;

		void Clamp();

	private:
		bool isEnabled;
		bool isSelectable;
		bool receivesBackInput;
	};
}

#endif //!UI_ELEMENT_H
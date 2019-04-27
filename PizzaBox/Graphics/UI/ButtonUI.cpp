#include "ButtonUI.h"

#include <rttr/registration.h>

#include "Graphics/RenderEngine.h"
#include "Math/Math.h"
#include "Resource/ResourceManager.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<ButtonUI>("ButtonUI")
		.constructor<const std::string&>()
		.constructor<const std::string&, const Rect&, bool>()
		.constructor<const std::string&, const Rect&, bool, const std::string&>()
		.constructor<const std::string&, const Rect&, bool, const std::string&, Script*>()
		.constructor<const std::string&, const UIEventFunc&>()
		.constructor<const std::string&, const UIEventFunc&, const Rect&>()
		.constructor<const std::string&, const UIEventFunc&, const Rect&, bool>()
		.constructor<const std::string&, const UIEventFunc&, const Rect&, bool, const std::string&>()
		.method("Initialize", &ButtonUI::Initialize)
		.method("Destroy", &ButtonUI::Destroy)
		.method("Render", &ButtonUI::Render)
		.method("OnSelect", &ButtonUI::OnSelect)
		.method("OnDeselect", &ButtonUI::OnDeselect)
		.method("SetTransparency", &ButtonUI::SetTransparency)
		.method("AddText", &ButtonUI::AddText);
}
#pragma warning( pop )

ButtonUI::ButtonUI(const std::string& name_, const Rect& rect_, bool clamp_, const std::string& imageName_, Script* script_) : UIElement(name_, rect_, script_), transparency(1.0f), imageName(imageName_), fontName("ArialFont"), text(""), textUI(nullptr), imageUI(nullptr){
	SetTransparency(transparency);
	SetIsSelectable(true);
	
	clampToScreen = clamp_;
	Clamp();
}

ButtonUI::ButtonUI(const std::string& name_, const UIEventFunc& function_, const Rect& rect_, bool clamp_, const std::string& imageName_) : UIElement(name_, rect_, function_), transparency(1.0f), imageName(imageName_), fontName("ArialFont"), text(""), textUI(nullptr), imageUI(nullptr){
	SetTransparency(transparency);
	SetIsSelectable(true);

	clampToScreen = clamp_;
	Clamp();
}

ButtonUI::~ButtonUI(){
}

bool ButtonUI::Initialize(){
	imageUI = new ImageUI(name + "Image", imageName, rect, clampToScreen);
	if(imageUI->Initialize() == false){
		Debug::LogError("Could not initialize ImageUI!", __FILE__, __LINE__);
		return false;
	}

	//Hardcoded fudge values - TODO
	Rect textRect = rect;
	textRect.w /= 2.0f;
	textRect.h /= 1.5f;
	textRect.x -= textRect.w / 16.0f;
	textRect.y -= textRect.h / 1.5f;

	textUI = new TextUI("ButtonText", textRect, clampToScreen, text, fontName);
	if(textUI->Initialize() == false){
		Debug::LogError("Could not initialize TextUI!", __FILE__, __LINE__);
		return false;
	}

	SetTransparency(transparency);

	return true;
}

void ButtonUI::Destroy(){
	if(imageUI != nullptr){
		imageUI->Destroy();
		delete imageUI;
		imageUI = nullptr;
	}

	if(textUI != nullptr){
		textUI->Destroy();
		delete textUI;
		textUI = nullptr;
	}
}

void ButtonUI::Render(){
	if(imageUI != nullptr){
		imageUI->Render();
	}

	if(textUI != nullptr){
		textUI->Render();
	}
}

void ButtonUI::OnSelect(){
	if(imageUI != nullptr){
		imageUI->SetTransparency(0.5f);
	}
}

void ButtonUI::OnDeselect(){
	if(imageUI != nullptr){
		imageUI->SetTransparency(1.0f);
	}
}

void ButtonUI::AddText(const std::string& words_){
	text = words_;
	if(textUI != nullptr){
		textUI->SetText(words_);
	}
}
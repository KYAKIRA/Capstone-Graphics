#include "UIElement.h"

#include <rttr/registration.h>

#include "Math/Math.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<UIElement>("UIElement")
		.property("name", &UIElement::name)
		.property("rect", &UIElement::rect)
		.method("Initialize", &UIElement::Initialize)
		.method("Destroy", &UIElement::Destroy)
		.method("Render", &UIElement::Render)
		.method("OnClick", &UIElement::OnClick)
		.method("OnRelease", &UIElement::OnRelease)
		.method("OnSelect", &UIElement::OnSelect)
		.method("OnDeselect", &UIElement::OnDeselect)
		.method("GetName", &UIElement::GetName)
		.method("GetRect", &UIElement::GetRect)
		.method("GetEnable", &UIElement::GetEnable)
		.method("GetScript", &UIElement::GetScript)
		.method("GetFunction", &UIElement::GetFunction)
		.method("IsSelectable", &UIElement::IsSelectable)
		.method("ReceivesBackInput", &UIElement::ReceivesBackInput)
		.method("SetName", &UIElement::SetName)
		.method("SetRect", &UIElement::SetRect)
		.method("SetEnable", &UIElement::SetEnable)
		.method("SetScript", &UIElement::SetScript)
		.method("SetFunction", &UIElement::SetFunction)
		.method("SetIsSelectable", &UIElement::SetIsSelectable)
		.method("SetReceivesBackInput", &UIElement::SetReceivesBackInput);
}
#pragma warning( pop )

void UIElement::Clamp(){
	if(!clampToScreen){
		return;
	}

	//TODO - Fix this
	/*if(rect.x - rect.w / 2.0f < 0.0f){
		rect.x += Math::Abs(rect.x - rect.w / 2.0f);
	}

	if(rect.x + rect.w / 2.0f > 1.0f){
		rect.x -= Math::Abs(rect.x + rect.w / 2.0f);
	}

	if(rect.y - rect.h / 2.0f < 0.0f){
		rect.x += Math::Abs(rect.y - rect.h / 2.0f);
	}

	if(rect.y + rect.h / 2.0f > 1.0f){
		rect.x -= Math::Abs(rect.y + rect.h / 2.0f);
	}*/
}
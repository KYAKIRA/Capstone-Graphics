#include "UISet.h"

#include <rttr/registration.h>

#include "Graphics/UI/UIManager.h"
#include "Math/Math.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<UISet>("UISet")
		.constructor<const std::string&>()
		.property("name", &UISet::name)
		.property("elements", &UISet::elements)
		.method("Initialize", &UISet::Initialize)
		.method("Destroy", &UISet::Destroy)
		.method("AddElement", &UISet::AddElement)
		.method("Render", &UISet::Render)
		.method("HasSelectableElements", &UISet::HasSelectableElements)
		.method("SelectElement", &UISet::SelectElement)
		.method("SetSelectedElement", &UISet::SetSelectedElement)
		.method("ClickSelectedElement", &UISet::ClickSelectedElement)
		.method("ReleaseSelectedElement", &UISet::ReleaseSelectedElement)
		.method("ClickBackElement", &UISet::ClickBackElement)
		.method("ReleaseBackElement", &UISet::ReleaseBackElement);
}
#pragma warning( pop )

UISet::UISet(const std::string& name_) : name(name_), elements(), currentSelected(nullptr){
}

UISet::~UISet(){
	for(UIElement* e : elements){
		e->Destroy();
		delete e;
		e = nullptr;
	}

	elements.clear();
	elements.shrink_to_fit();
}

bool UISet::Initialize(){
	for(UIElement* e : elements){
		if(e->Initialize() == false){
			Debug::LogError("Could not initialize all elements of UISet!", __FILE__, __LINE__);
			return false;
		}
	}

	return true;
}

void UISet::Destroy(){
	for(UIElement* e : elements){
		e->Destroy();
	}

	currentSelected = nullptr;
}

void UISet::AddElement(UIElement* element_){
	_ASSERT(element_ != nullptr);
	elements.push_back(element_);
}

void UISet::Render(){
	for(UIElement* e : elements){
		if(e->GetEnable()){
			e->Render();
		}
	}
}

bool UISet::HasSelectableElements(){
	for(UIElement* e : elements){
		if(e->GetEnable() && e->IsSelectable()){
			return true;
		}
	}

	return false;
}

void UISet::SelectElement(SelectorDirection direction_){
	if(!HasSelectableElements()){
		return;
	}

	if(direction_ == SelectorDirection::None){
		SetSelectedElement(nullptr);
		return;
	}

	if(currentSelected == nullptr){
		for(UIElement* e : elements){
			if(!e->GetEnable() || !e->IsSelectable()){
				continue;
			}

			SetSelectedElement(e);
			break;
		}

		return;
	}

	switch(direction_){
		case SelectorDirection::None:
			SetSelectedElement(nullptr);
			break;
		case SelectorDirection::Up:
			SetSelectedElement(GetNextUp());
			break;
		case SelectorDirection::Down:
			SetSelectedElement(GetNextDown());
			break;
		case SelectorDirection::Left:
			SetSelectedElement(GetNextLeft());
			break;
		case SelectorDirection::Right:
			SetSelectedElement(GetNextRight());
			break;
		default:
			break;
	}
}

void UISet::SetSelectedElement(UIElement* element_){
	if(!HasSelectableElements()){
		return;
	}

	if(currentSelected != nullptr){
		currentSelected->OnDeselect();
		currentSelected = nullptr;
	}

	if(element_ == nullptr || !element_->GetEnable() || !element_->IsSelectable()){
		return;
	}

	_ASSERT(std::find(elements.begin(), elements.end(), element_) != elements.end());
	element_->OnSelect();

	currentSelected = element_;
}

void UISet::ClickSelectedElement(){
	if(currentSelected != nullptr && currentSelected->GetEnable()){
		currentSelected->OnClick();
	}
}

void UISet::ReleaseSelectedElement(){
	if(currentSelected != nullptr && currentSelected->GetEnable()){
		currentSelected->OnRelease();
	}
}

void UISet::ClickBackElement(){
	for(UIElement* e : elements){
		if(!e->GetEnable()){
			continue;
		}

		if(e->ReceivesBackInput()){
			e->OnClick();
		}
	}
}

void UISet::ReleaseBackElement(){
	for(UIElement* e : elements){
		if(!e->GetEnable()){
			continue;
		}

		if(e->ReceivesBackInput()){
			e->OnRelease();
		}
	}
}

UIElement* UISet::GetNextUp() const{
	UIElement* next = nullptr;
	for(UIElement* e : elements){
		if(!e->GetEnable() || !e->IsSelectable()){
			continue;
		}

		if(currentSelected == nullptr){
			return e;
		}

		if(next == nullptr){
			if(e->rect.y > currentSelected->rect.y){
				next = e;
			}
			continue;
		}

		if(e->rect.y > currentSelected->rect.y && e->rect.y < next->rect.y){
			next = e;
			continue;
		}

		if(e->rect.y == next->rect.y){
			auto currentDiff = Math::Abs(next->rect.x - currentSelected->rect.x);
			auto newDiff = Math::Abs(e->rect.x - currentSelected->rect.x);

			if(newDiff < currentDiff){
				next = e;
			}
		}
	}

	if(next == nullptr){
		Rect curRect = currentSelected->rect;
		curRect.y = 0.0f;

		for(UIElement* e : elements){
			if(!e->GetEnable() || !e->IsSelectable()){
				continue;
			}

			if(next == nullptr){
				if(e->rect.y > curRect.y){
					next = e;
				}
				continue;
			}

			if(e->rect.y > curRect.y && e->rect.y < next->rect.y){
				next = e;
				continue;
			}

			if(e->rect.y == next->rect.y){
				auto currentDiff = Math::Abs(next->rect.x - curRect.x);
				auto newDiff = Math::Abs(e->rect.x - curRect.x);

				if(newDiff < currentDiff){
					next = e;
				}
			}
		}
	}

	return next;
}

UIElement* UISet::GetNextDown() const{
	UIElement* next = nullptr;
	for(UIElement* e : elements){
		if(!e->GetEnable() || !e->IsSelectable()){
			continue;
		}

		if(currentSelected == nullptr){
			return e;
		}

		if(next == nullptr){
			if(e->rect.y < currentSelected->rect.y){
				next = e;
			}
			continue;
		}

		if(e->rect.y < currentSelected->rect.y && e->rect.y > next->rect.y){
			next = e;
			continue;
		}

		if(e->rect.y == next->rect.y){
			auto currentDiff = Math::Abs(next->rect.x - currentSelected->rect.x);
			auto newDiff = Math::Abs(e->rect.x - currentSelected->rect.x);

			if(newDiff < currentDiff){
				next = e;
			}
		}
	}

	if(next == nullptr){
		Rect curRect = currentSelected->rect;
		curRect.y = 1.0f;

		for(UIElement* e : elements){
			if(!e->GetEnable() || !e->IsSelectable()){
				continue;
			}

			if(next == nullptr){
				if(e->rect.y < curRect.y){
					next = e;
				}
				continue;
			}

			if(e->rect.y < curRect.y && e->rect.y > next->rect.y){
				next = e;
				continue;
			}

			if(e->rect.y == next->rect.y){
				auto currentDiff = Math::Abs(next->rect.x - curRect.x);
				auto newDiff = Math::Abs(e->rect.x - curRect.x);

				if(newDiff < currentDiff){
					next = e;
				}
			}
		}
	}

	return next;
}

UIElement* UISet::GetNextLeft() const{
	std::vector<UIElement*> leftElements;

	UIElement* next = nullptr;
	for(UIElement* e : elements){
		if(!e->GetEnable() || !e->IsSelectable()){
			continue;
		}

		if(currentSelected == nullptr){
			return e;
		}

		if(next == nullptr){
			if(e->rect.x < currentSelected->rect.x){
				next = e;
			}
			continue;
		}

		if(e->rect.x < currentSelected->rect.x && e->rect.x > next->rect.x){
			next = e;
			continue;
		}

		if(e->rect.x == next->rect.x){
			auto currentDiff = Math::Abs(next->rect.y - currentSelected->rect.y);
			auto newDiff = Math::Abs(e->rect.y - currentSelected->rect.y);

			if(newDiff < currentDiff){
				next = e;
			}
		}
	}

	if(next == nullptr){
		Rect curRect = currentSelected->rect;
		curRect.x = 1.0f;

		for(UIElement* e : elements){
			if(!e->GetEnable() || !e->IsSelectable()){
				continue;
			}

			if(next == nullptr){
				if(e->rect.x < curRect.x){
					next = e;
				}
				continue;
			}

			if(e->rect.x < curRect.x && e->rect.x > next->rect.x){
				next = e;
				continue;
			}

			if(e->rect.x == next->rect.x){
				auto currentDiff = Math::Abs(next->rect.y - curRect.y);
				auto newDiff = Math::Abs(e->rect.y - curRect.y);

				if(newDiff < currentDiff){
					next = e;
				}
			}
		}
	}

	return next;
}

UIElement* UISet::GetNextRight() const{
	UIElement* next = nullptr;
	for(UIElement* e : elements){
		if(!e->GetEnable() || !e->IsSelectable()){
			continue;
		}

		if(currentSelected == nullptr){
			return e;
		}

		if(next == nullptr){
			if(e->rect.x > currentSelected->rect.x){
				next = e;
			}
			continue;
		}

		if(e->rect.x > currentSelected->rect.x && e->rect.x < next->rect.x){
			next = e;
			continue;
		}

		if(e->rect.x == next->rect.x){
			auto currentDiff = Math::Abs(next->rect.y - currentSelected->rect.y);
			auto newDiff = Math::Abs(e->rect.y - currentSelected->rect.y);

			if(newDiff < currentDiff){
				next = e;
			}
		}
	}

	if(next == nullptr){
		Rect curRect = currentSelected->rect;
		curRect.x = 0.0f;

		for(UIElement* e : elements){
			if(!e->GetEnable() || !e->IsSelectable()){
				continue;
			}

			if(next == nullptr){
				if(e->rect.x > curRect.x){
					next = e;
				}
				continue;
			}

			if(e->rect.x > curRect.x && e->rect.x < next->rect.x){
				next = e;
				continue;
			}

			if(e->rect.x == next->rect.x){
				auto currentDiff = Math::Abs(next->rect.y - curRect.y);
				auto newDiff = Math::Abs(e->rect.y - curRect.y);

				if(newDiff < currentDiff){
					next = e;
				}
			}
		}
	}

	return next;
}
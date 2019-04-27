#include "UIManager.h"

#include <algorithm>

#include <rttr/registration.h>

#include "ImageUI.h"
#include "Core/GameManager.h"
#include "Tools/Debug.h"
#include "Input/InputManager.h"
#include "Graphics/RenderEngine.h"

using namespace PizzaBox;

std::vector<UISet*> UIManager::sets = std::vector<UISet*>();
std::vector<UISet*> UIManager::activeSets = std::vector<UISet*>();
bool UIManager::changedUISetsThisFrame = false;
SelectorInput UIManager::lastInput = SelectorInput::None;
SelectorDirection UIManager::lastDirection = SelectorDirection::None;

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::enumeration<SelectorInput>("SelectorInput")(
		rttr::value("None", SelectorInput::None),
		rttr::value("Click", SelectorInput::Click),
		rttr::value("Release", SelectorInput::Release),
		rttr::value("BackClick", SelectorInput::BackClick),
		rttr::value("BackClick", SelectorInput::BackRelease)
	);

	rttr::registration::enumeration<SelectorDirection>("SelectorDirection")(
		rttr::value("None", SelectorDirection::None),
		rttr::value("Up", SelectorDirection::Up),
		rttr::value("Down", SelectorDirection::Down),
		rttr::value("Left", SelectorDirection::Left),
		rttr::value("Right", SelectorDirection::Right)
	);

	rttr::registration::class_<UIManager>("UIManager")
		.method("Initialize", &UIManager::Initialize)
		.method("Destroy", &UIManager::Destroy)
		.method("Update", &UIManager::Update)
		.method("Render", &UIManager::Render)
		.method("AddToSet", &UIManager::AddSet)
		.method("EnableSet", &UIManager::EnableSet)
		.method("DisableSet", &UIManager::DisableSet)
		.method("DisableAllSets", &UIManager::DisableAllSets)
		.method("ToggleSet", &UIManager::ToggleSet)
		.method("IsSetActive", &UIManager::IsSetActive)
		.method("MoveSetUp", &UIManager::MoveSetUp)
		.method("MoveSetDown", &UIManager::MoveSetDown)
		.method("MoveSetToTop", &UIManager::MoveSetToTop)
		.method("MoveSetToBottom", &UIManager::MoveSetToBottom);
}
#pragma warning( pop )

bool UIManager::Initialize(){
	//No sets should be added before the UI Manager is ready to go
	_ASSERT(sets.size() == 0);

	Axis* verticalUIAxis = new Axis();
	verticalUIAxis->SetKeyInput(SDLK_UP, SDLK_DOWN);
	verticalUIAxis->SetGamepadPad(0, PizzaBox::Axis::AxisType::Y);
	InputManager::AddAxis("__UI_VERTICAL_1__", verticalUIAxis);

	Axis* verticalUIAxis2 = new Axis();
	verticalUIAxis2->SetGamepadAxis(0, 1, 0.5f);
	InputManager::AddAxis("__UI_VERTICAL_2__", verticalUIAxis2);

	Axis* horizontalUIAxis = new Axis();
	horizontalUIAxis->SetKeyInput(SDLK_RIGHT, SDLK_LEFT);
	horizontalUIAxis->SetGamepadAxis(0, 0);
	horizontalUIAxis->SetGamepadPad(0, PizzaBox::Axis::AxisType::X);
	InputManager::AddAxis("__UI_HORIZONTAL__", horizontalUIAxis);

	Button* button1 = new Button();
	button1->SetKeyInput(SDLK_SPACE);
	button1->SetGamepadInput(0, 0);
	InputManager::AddButton("__UI_BUTTON_1__", button1);

	Button* button2 = new Button();
	button2->SetKeyInput(SDLK_RETURN);
	InputManager::AddButton("__UI_BUTTON_2__", button2);

	Button* backButton = new Button();
	backButton->SetKeyInput(SDLK_ESCAPE);
	backButton->SetGamepadInput(0, 1);
	InputManager::AddButton("__UI_BACK_BUTTON__", backButton);

	UISet* set = new UISet("__SELECTED_BACKGROUND__");
	ImageUI* image = new ImageUI("Background", "ButtonWrapperImage", Rect());
	image->SetIsSelectable(false);
	set->AddElement(image);
	sets.push_back(set);
	
	return true;
}

void UIManager::Destroy(){
	for(UISet* s : sets){
		_ASSERT(s != nullptr);
		s->Destroy();
		delete s;
		s = nullptr;
	}

	sets.clear();
	sets.shrink_to_fit();

	activeSets.clear();
	activeSets.shrink_to_fit();
}

void UIManager::Update(){
	if(changedUISetsThisFrame){
		changedUISetsThisFrame = false;
		return; //Don't allow any UI input if we *just* changed sets, prevents this from erroneously responding to user inputs
	}

	ScreenCoordinate mousePos = PizzaBox::InputManager::GetMousePosition();
	MouseDelta mouseDelta = PizzaBox::InputManager::GetMouseDelta();

	//Convert mouse position from pixel space into UI space
	float mX = static_cast<float>(mousePos.x) / static_cast<float>(RenderEngine::ScreenSize().x);
	float mY = static_cast<float>(mousePos.y) / static_cast<float>(RenderEngine::ScreenSize().y);

	bool mouseDown = false;
	bool mouseUp = false;
	SelectorInput input = GetInput();
	SelectorDirection direction = GetInputDirection();

	//Stops us from registering a single input as a double-tap
	if(input == lastInput){
		input = SelectorInput::None;
	} else{
		lastInput = input;
	}

	//Stops us from registering a single input as a double-tap
	if(direction == lastDirection){
		direction = SelectorDirection::None;
	} else{
		lastDirection = direction;
	}

	if(PizzaBox::InputManager::GetMouseDown(SDL_BUTTON_LEFT)){
		mouseDown = true;
	}

	if(PizzaBox::InputManager::GetMouseUp(SDL_BUTTON_LEFT)){
		mouseUp = true;
	}

	std::vector<UISet*> currentActiveSets = std::vector<UISet*>(activeSets);
	bool hasSelectableElements = false;
	for(UISet* set : activeSets){
		if(set->HasSelectableElements()){
			hasSelectableElements = true;
			break;
		}
	}

	if(!hasSelectableElements){
		RenderEngine::ShowCursor(false);
	} else if(mouseDelta.x != 0 || mouseDelta.y != 0){
		RenderEngine::ShowCursor(true);
		input = SelectorInput::None;
		direction = SelectorDirection::None;

		for(UISet* set : activeSets){
			if(IsSetActive(set->name)){
				set->SetSelectedElement(nullptr);
			}
		}

	} else if(input != SelectorInput::None || direction != SelectorDirection::None){
		RenderEngine::ShowCursor(false);
	}

	for(UISet* set : currentActiveSets){
		if(!IsSetActive(set->name)){
			continue; //This set is no longer active, skip it
		}

		if(direction != SelectorDirection::None && set->HasSelectableElements()){
			set->SelectElement(direction);
		}

		if(set->HasSelectableElements()){
			switch(input){
				case SelectorInput::Click:
					set->ClickSelectedElement();
					break;
				case SelectorInput::Release:
					set->ReleaseSelectedElement();
					break;
				case SelectorInput::BackClick:
					set->ClickBackElement();
					break;
				case SelectorInput::BackRelease:
					set->ReleaseBackElement();
					break;
			}
		}

		for(UIElement* e : set->elements){
			//Skip this element if it's not active
			if(e->GetEnable() == false){
				continue;
			}

			if(!RenderEngine::IsShowingCursor()){
				break;
			}

			if(e->rect.Intersects(mX, mY)){
				if(direction == SelectorDirection::None){
					set->SetSelectedElement(e);
				}

				if(mouseDown){
					e->OnClick();
				}

				if(mouseUp){
					e->OnRelease();
				}
			}
		}

		bool useBackground = false;
		std::vector<UISet*> currentActiveSets = std::vector<UISet*>(activeSets);
		for(UISet* set : currentActiveSets){
			if(set->SelectedElement() != nullptr){
				Rect r = set->SelectedElement()->GetRect();
				r.w *= 1.05f;
				r.h *= 1.05f;

				EnableSet("__SELECTED_BACKGROUND__");
				MoveSetToBottom("__SELECTED_BACKGROUND__");
				GetElementFromSet<ImageUI>("__SELECTED_BACKGROUND__", "Background")->SetRect(r);
				useBackground = true;
				break;
			}
		}

		if(!useBackground){
			DisableSet("__SELECTED_BACKGROUND__");
		}
	}
}

void UIManager::Render(){
	for(UISet* s : activeSets){
		_ASSERT(s != nullptr);
		s->Render();
	}
}

void UIManager::AddSet(UISet* set_){
	//Must be a valid pointer to a UISet
	_ASSERT(set_ != nullptr);

	for(UISet* s : sets){
		//This will fire if a set with this name already exists
		_ASSERT(set_->name != s->name);
	}
		
	sets.push_back(set_);
}

void UIManager::EnableSet(const std::string& name_){
	for(UISet* s : activeSets){
		if(s->name == name_){
			return; //This set is already enabled
		}
	}

	for(UISet* s : sets){
		if(s->name == name_){
			//This is the one we're looking for
			if(s->Initialize() == false){
				Debug::DisplayFatalErrorMessage("UI Error", "Could not initialize UI Set " + s->name + "!");
				GameManager::Stop();
			}
			activeSets.push_back(s);

			changedUISetsThisFrame = true;
			return;
		}
	}

	Debug::LogWarning("No UISet named " + name_ + " found!", __FILE__, __LINE__);
}

void UIManager::DisableSet(const std::string& name_){
	for(UISet* s : activeSets){
		if(s->name == name_){
			s->Destroy();

			changedUISetsThisFrame = true;
			break;
		}
	}

	//Remove any sets that match the name passed in here
	activeSets.erase( std::remove_if(activeSets.begin(), activeSets.end(), [name_](UISet* s){ return s->name == name_; }), activeSets.end());
}

void UIManager::DisableAllSets(){
	for(UISet* s : activeSets){
		s->Destroy();
	}

	activeSets.clear();
}

void UIManager::ToggleSet(const std::string& name_){
	for(UISet* s : activeSets){
		if(s->name == name_){
			DisableSet(name_);
			return;
		}
	}

	for(UISet* s : sets){
		if(s->name == name_){
			EnableSet(name_);
			return;
		}
	}

	Debug::LogWarning("Attempted to toggle nonexistant UI Set " + name_ + "!", __FILE__, __LINE__);
}

bool UIManager::IsSetActive(const std::string& setName_){
	for(const auto& set : activeSets){
		if(set->name == setName_){
			return true;
		}
	}

	return false;
}

void UIManager::MoveSetUp(const std::string& setName_){
	UISet* setToMove = nullptr;
	for(UISet* s : activeSets){
		if(s->name == setName_){
			setToMove = s;
			break;
		}
	}

	if(setToMove == nullptr){
		Debug::LogWarning("No UISet named " + setName_ + " found!", __FILE__, __LINE__);
		return;
	}

	if(setToMove == activeSets.back()){
		return;
	}

	auto it = std::find(activeSets.begin(), activeSets.end(), setToMove);
	std::iter_swap(it, it + 1);
}

void UIManager::MoveSetDown(const std::string& setName_){
	UISet* setToMove = nullptr;
	for(UISet* s : activeSets){
		if(s->name == setName_){
			setToMove = s;
			break;
		}
	}

	if(setToMove == nullptr){
		Debug::LogWarning("No UISet named " + setName_ + " found!", __FILE__, __LINE__);
		return;
	}

	if(setToMove == activeSets.front()){
		return;
	}

	auto it = std::find(activeSets.begin(), activeSets.end(), setToMove);
	std::iter_swap(it, it - 1);
}

void UIManager::MoveSetToTop(const std::string& setName_){
	UISet* setToMove = nullptr;
	for(UISet* s : activeSets){
		if(s->name == setName_){
			setToMove = s;
			break;
		}
	}

	if(setToMove == nullptr){
		Debug::LogWarning("No UISet named " + setName_ + " found!", __FILE__, __LINE__);
		return;
	}

	activeSets.erase(std::remove(activeSets.begin(), activeSets.end(), setToMove), activeSets.end());
	activeSets.push_back(setToMove); //Back of the list is the "top" of the UI rendering stack
}

void UIManager::MoveSetToBottom(const std::string& setName_){
	UISet* setToMove = nullptr;
	for(UISet* s : activeSets){
		if(s->name == setName_){
			setToMove = s;
			break;
		}
	}

	if(setToMove == nullptr){
		Debug::LogWarning("No UISet named " + setName_ + " found!", __FILE__, __LINE__);
		return;
	}

	activeSets.erase(std::remove(activeSets.begin(), activeSets.end(), setToMove), activeSets.end());
	activeSets.insert(activeSets.begin(), setToMove); //Front of the list is the "bottom" of the UI rendering stack
}

SelectorInput UIManager::GetInput(){
	if(PizzaBox::InputManager::GetButtonDown("__UI_BUTTON_1__") || PizzaBox::InputManager::GetButtonDown("__UI_BUTTON_2__")){
		return SelectorInput::Click;
	}else if(PizzaBox::InputManager::GetButtonUp("__UI_BUTTON_1__") || PizzaBox::InputManager::GetButtonUp("__UI_BUTTON_2__")){
		return SelectorInput::Release;
	}

	if(PizzaBox::InputManager::GetButtonDown("__UI_BACK_BUTTON__")){
		return SelectorInput::BackClick;
	}else if(PizzaBox::InputManager::GetButtonUp("__UI_BACK_BUTTON__")){
		return SelectorInput::BackRelease;
	}

	return SelectorInput::None;
}

SelectorDirection UIManager::GetInputDirection(){
	float vertical1 = PizzaBox::InputManager::GetAxis("__UI_VERTICAL_1__");
	float vertical2 = PizzaBox::InputManager::GetAxis("__UI_VERTICAL_2__");
	float horizontal = PizzaBox::InputManager::GetAxis("__UI_HORIZONTAL__");

	if(vertical1 > 0 || vertical2 < 0){
		return SelectorDirection::Up;
	}else if(vertical1 < 0 || vertical2 > 0){
		return SelectorDirection::Down;
	}

	if(horizontal > 0){
		return SelectorDirection::Right;
	}else if(horizontal < 0){
		return SelectorDirection::Left;
	}

	return SelectorDirection::None;
}
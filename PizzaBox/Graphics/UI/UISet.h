#ifndef UI_SET_H
#define UI_SET_H

#include <string>
#include <vector>

#include "UIElement.h"

namespace PizzaBox{
	//Forward declaration
	enum class SelectorDirection;

	class UISet{
	public:
		UISet(const std::string& name_);
		~UISet();

		std::string name;
		std::vector<UIElement*> elements;

		bool Initialize();
		void Destroy();

		void AddElement(UIElement* element_);
		
		void Render();

		inline bool HasSelectableElements();
		void SelectElement(SelectorDirection direction_);
		void SetSelectedElement(UIElement* element_);
		void ClickSelectedElement();
		void ReleaseSelectedElement();
		void ClickBackElement();
		void ReleaseBackElement();

		inline UIElement* SelectedElement(){ return currentSelected; }

	private:
		UIElement* currentSelected;

		UIElement* GetNextUp() const;
		UIElement* GetNextDown() const;
		UIElement* GetNextLeft() const;
		UIElement* GetNextRight() const;
	};
}

#endif //!UI_SET_H
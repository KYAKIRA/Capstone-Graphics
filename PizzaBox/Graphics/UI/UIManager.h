#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <vector>

#include "UISet.h"

namespace PizzaBox{
	enum class SelectorInput{
		None,
		Click,
		Release,
		BackClick,
		BackRelease
	};

	enum class SelectorDirection{
		None,
		Up,
		Down,
		Left,
		Right
	};

	class UIManager{
	public:
		static bool Initialize();
		static void Destroy();

		static void Update();
		static void Render();

		static void AddSet(UISet* set_);
		static void EnableSet(const std::string& setName_);
		static void DisableSet(const std::string& setName_);
		static void DisableAllSets();
		static void ToggleSet(const std::string& setName_);
		static bool IsSetActive(const std::string& setName_);

		template <class T>
		static T* GetElementFromSet(const std::string& setName_, const std::string& elementName_){
			for(const auto& set : activeSets){
				if(set->name == setName_){
					for(const auto& element : set->elements){
						T* e = dynamic_cast<T*>(element);
						if(e != nullptr && e->name == elementName_){
							return e;
						}
					}
				}
			}

			return nullptr;
		}

		static void MoveSetUp(const std::string& setName_);
		static void MoveSetDown(const std::string& setName_);
		static void MoveSetToTop(const std::string& setName_);
		static void MoveSetToBottom(const std::string& setName_);
		
	private:
		static std::vector<UISet*> sets;
		static std::vector<UISet*> activeSets;

		static SelectorInput lastInput;
		static SelectorDirection lastDirection;
		static bool changedUISetsThisFrame;

		static SelectorInput GetInput();
		static SelectorDirection GetInputDirection();
	};
}

#endif //!UI_MANAGER_H
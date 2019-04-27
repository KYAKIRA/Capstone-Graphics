#ifndef CHARACTER_CONTROLLER_H
#define CHARACTER_CONTROLLER_H

#include <Script/Script.h>

namespace GamePackage{
	class CharacterController : public PizzaBox::Script{
	public:
		CharacterController();
		void OnStart() override;
		void Update(const float deltaTime_) override;
		void OnDestroy() override;

	private:
		bool toggle;
	};
}

#endif //!CHARACTER_CONTROLLER_H
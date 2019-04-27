#ifndef SOLAR_SYSTEM_H
#define SOLAR_SYSTEM_H

#include <Script/Script.h>
#include <vector>

namespace GamePackage {
	class SolarSystem : public PizzaBox::Script {
	public:
		std::vector<PizzaBox::GameObject*> planets;
	private:
		void OnStart() override;
		void Update(const float deltaTime_) override;
		void OnDestroy() override;

		void ApplyGravity(const float deltaTime_);
	};
}

#endif //!SOLAR_SYSTEM_H
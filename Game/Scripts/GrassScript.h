#ifndef GRASS_SCRIPT_H
#define GRASS_SCRIPT_H

#include <Graphics/Effects/GrassSimulation.h>
#include <Script/Script.h>

namespace GamePackage{
	class GrassScript : public PizzaBox::Script{
	public:
		GrassScript(PizzaBox::GrassSimulation* simulation_);
		virtual ~GrassScript() override;

		PizzaBox::GrassSimulation* simulation;

		virtual void OnStart() override;
		virtual void Update(const float deltaTime_) override;
		virtual void OnDestroy() override;

		void SetGrassSimulation(PizzaBox::GrassSimulation* simulation_);
	};
}

#endif //!GRASS_SCRIPT_H
#ifndef SOLAR_SCENE_H
#define SOLAR_SCENE_H

#include <Core/Scene.h>
 
namespace GamePackage{
	class SolarScene : public PizzaBox::Scene{
	public:
		SolarScene();
		~SolarScene();

		bool Initialize();
		void Destroy();
	};
}

#endif //!SOLAR_SCENE_H
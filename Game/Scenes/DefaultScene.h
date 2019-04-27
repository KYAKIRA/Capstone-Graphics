#ifndef DEFAULT_SCENE_H
#define DEFAULT_SCENE_H

#include <Core/Scene.h>

namespace GamePackage{
	class DefaultScene : public PizzaBox::Scene{
	public:
		DefaultScene();
		virtual ~DefaultScene() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;
	};
}

#endif //!DEFAULT_SCENE_H
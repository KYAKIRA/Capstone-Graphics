#ifndef LOGO_SCENE_H
#define LOGO_SCENE_H

#include <Core/Scene.h>

namespace GamePackage{
	class LogoScene : public PizzaBox::Scene{
	public:
		LogoScene();
		virtual ~LogoScene() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;
	};
}

#endif //!LOGO_SCENE_H
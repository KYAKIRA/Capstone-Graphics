#ifndef LOGO_MANAGER_H
#define LOGO_MANAGER_H

#include <Script/Script.h>
#include <Graphics/UI/ImageUI.h>

namespace GamePackage{
	class LogoManager : public PizzaBox::Script{
	public:
		LogoManager(PizzaBox::ImageUI* image_, float endTime_ = 20.0f, float waitTime_ = 2.0f);

		virtual void OnStart() override;
		virtual void Update(const float deltaTime_) override;
		virtual void OnDestroy() override;

	private:
		PizzaBox::ImageUI* image;
		float currentTime, endTime, waitTime, transparency;
	};
}

#endif //!LOGO_MANAGER_H
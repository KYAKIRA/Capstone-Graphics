#ifndef SHADOW_FBO_H
#define SHADOW_FBO_H

#include "BaseFBO.h"

namespace PizzaBox{
	class ShadowFBO : public BaseFBO{
	public:
		ShadowFBO(const ScreenCoordinate& screenSize_);
		virtual ~ShadowFBO() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;

		inline GLuint GetDepthMap() { return depthMap; }

	protected:
		GLuint depthMap;
	}; 
}

#endif //!SHADOW_FBO_H
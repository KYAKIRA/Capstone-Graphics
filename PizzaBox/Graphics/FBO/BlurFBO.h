#ifndef BLUR_FBO_H
#define BLUR_FBO_H

#include "BaseFBO.h"

namespace PizzaBox{
	class BlurFBO : public BaseFBO{
	public:
		BlurFBO(const ScreenCoordinate& screenSize_);
		virtual ~BlurFBO() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;

		GLuint GetColorTexture(){
			return colorTexture;
		}

		GLuint GetDepthBuffer(){
			return depthBuffer;
		}

	protected:
		GLuint colorTexture;
		GLuint depthBuffer;
	};
}

#endif //!BLUR_FBO_H
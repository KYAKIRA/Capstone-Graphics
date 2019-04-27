#ifndef MULTISAMPLE_FBO_H
#define MULTISAMPLE_FBO_H

#include "BaseFBO.h"

namespace PizzaBox{
	class MultisampleFBO : public BaseFBO{
	public:
		MultisampleFBO(const ScreenCoordinate& screenSize_);
		virtual ~MultisampleFBO() override;

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

#endif //!MULTISAMPLE_FBO_H
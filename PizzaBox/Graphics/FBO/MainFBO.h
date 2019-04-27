#ifndef MAIN_FBO_H
#define MAIN_FBO_H

#include "BaseFBO.h"

namespace PizzaBox{
	class MainFBO : public BaseFBO{
	public:
		MainFBO(const ScreenCoordinate& screenSize_);
		virtual ~MainFBO() override;

		virtual bool Initialize() override;
		virtual void Destroy() override;

		GLuint GetColorTexture(){
			return colorTexture;
		}

	protected:
		GLuint colorTexture;
	};
}

#endif //!MAIN_FBO_H
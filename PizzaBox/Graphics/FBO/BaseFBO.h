#ifndef BASE_FBO_H
#define BASE_FBO_H

#include <glew.h>

#include "../../Core/ScreenCoordinate.h"
#include "../../Tools/Debug.h"

namespace PizzaBox{
	class BaseFBO{
	public:
		BaseFBO(const ScreenCoordinate& screenSize_) : screenSize(screenSize_), frameBuffer(0){}
		virtual ~BaseFBO(){}

		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;

		void Bind(){
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		}

		void Unbind(){
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void ResolveToFBO(BaseFBO* mul){
			//Here mul should be the framebuffer that the texture is attach to that is not multisampled
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mul->frameBuffer);
			//glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer); 
			glBlitFramebuffer(0, 0, screenSize.x, screenSize.y, 0, 0, screenSize.x, screenSize.y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			Unbind();
		}

		void ResolveToScreen(){	
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);	//this would be the same as unbinding the buffer
			glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer); 
			glBlitFramebuffer(0, 0, screenSize.x, screenSize.y, 0, 0, screenSize.x, screenSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST); 
		}

		GLuint GetFrameBuffer(){
			return frameBuffer;
		}

	protected:
		ScreenCoordinate screenSize;
		GLuint frameBuffer;

		bool CheckFBOStatus(){
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			switch(status){
				case GL_FRAMEBUFFER_COMPLETE:
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					Debug::LogError("Framebuffer incomplete: Attachment is NOT complete", __FILE__, __LINE__);
					return false;
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					Debug::LogError("Framebuffer incomplete: No image is attached to FBO", __FILE__, __LINE__);
					return false;
				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
					Debug::LogError("Framebuffer incomplete: Draw buffer", __FILE__, __LINE__);
					return false; 
				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
					Debug::LogError("Framebuffer incomplete: Read buffer", __FILE__, __LINE__);
					return false; 
				case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
					Debug::LogError("Framebuffer incomplete: Multisample", __FILE__, __LINE__);
					return false;
				case GL_FRAMEBUFFER_UNSUPPORTED:
					Debug::LogError("Framebuffer incomplete: Unsupported by FBO implementation", __FILE__, __LINE__);
					return false;
				case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
					Debug::LogError("Framebuffer incomplete: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS", __FILE__, __LINE__);
					return false;
				case GL_FRAMEBUFFER_UNDEFINED:
					Debug::LogError("Framebuffer incomplete: GL_FRAMEBUFFER_UNDEFINED", __FILE__, __LINE__);
					return false;
				case GL_INVALID_ENUM:
					Debug::LogError("Framebuffer incomplete: GL_INVALID_ENUM", __FILE__, __LINE__);
					return false;
				default:
					Debug::LogError("Framebuffer incomplete: Unknown error", __FILE__, __LINE__);
					return false;
			} 
			return true;
		}
	};
}

#endif //!BASE_FBO_H
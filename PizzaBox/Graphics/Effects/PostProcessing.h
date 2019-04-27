#ifndef POST_PROCESSING_H
#define POST_PROCESSING_H
 
#include "../Shader.h"
#include "../LowLevel/VAO.h"
#include "../LowLevel/Buffer.h"

namespace PizzaBox {
	class PostProcessing {
	public:
		static bool Initialize(std::string shaderName_);
		static void Destroy();

		static void DoPostProcessing(GLuint texture_); 

		static void OnResize();

	private:
		static Shader* screenShader;
		static std::string shaderName;
		static VAO* vao;
		static Buffer* vbo;

		static void Start();
		static void End(); 
		static void CreateQuad();

		//Delete unwanted compiler generated constructors, destructors and assignment operators
		PostProcessing() = delete;
		PostProcessing(const PostProcessing&) = delete;
		PostProcessing(PostProcessing&&) = delete;
		PostProcessing& operator=(const PostProcessing&) = delete;
		PostProcessing& operator=(PostProcessing&&) = delete;
		~PostProcessing() = delete; 
	}; 
}
#endif //!POST_PROCESSING_H
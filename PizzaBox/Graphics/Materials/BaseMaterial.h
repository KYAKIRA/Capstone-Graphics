#ifndef BASE_MATERIAL_H
#define BASE_MATERIAL_H

#include <string>

#include "Graphics/Shader.h"
#include "Tools/Debug.h"

namespace PizzaBox{
	class BaseMaterial{
	public:
		BaseMaterial(std::string shaderName_) : shaderName(shaderName_), shader(nullptr){}
		virtual ~BaseMaterial(){
			#ifdef _DEBUG
			if(shader != nullptr){
				Debug::LogError("Memory Leak Detected in BaseMaterial!");
			}
			#endif //_DEBUG
		}

		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;

		Shader* GetShader() const{ return shader; }
		std::string GetShaderName() const{ return shaderName; }

	protected:
		std::string shaderName;
		Shader* shader;

		virtual void SetupUniforms(){}
		virtual void CleanupUniforms(){}
	};
}

#endif //!BASE_MATERIAL_H
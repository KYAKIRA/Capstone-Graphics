#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <unordered_map>
#include <string>
#include <vector>

#include "LowLevel/Uniform.h"
#include "Resource/Resource.h"

//Forward Declaration
typedef unsigned int GLuint;

namespace PizzaBox{
	//Forward Declarations
	struct Vector2;
	struct Vector3;
	struct Color;
	class Matrix3;
	class Matrix4;
	class DirectionalLight;
	class PointLight;
	class SpotLight;

	class Shader : public Resource{
	public:
		Shader(const std::string& vertPath_, const std::string& fragPath_);
		Shader(const std::string& vertPath_, const std::string& fragPath_, int numAttributes_, ...);
		Shader(const std::string& vertPath_, const std::string& fragPath_, const std::vector<std::string>& args_);
		~Shader();

		bool Load() override;
		void Unload() override;

		unsigned int Program() const;
		void Use();
		void Unbind();

		GLuint GetUniformID(const std::string& value_) const;

		void BindInt(GLuint uniformID_, int value_);
		void BindInt(const Uniform& uniform_, int value_);
		void BindInt(const std::string& uniformName_, int value_);

		void BindFloat(GLuint uniformID_, float value_);
		void BindFloat(const Uniform& uniform_, float value_);
		void BindFloat(const std::string& uniformName_, float value_);

		void BindVector2(GLuint uniformID_, const Vector2& value_);
		void BindVector2(const Uniform& uniform_, const Vector2& value_);
		void BindVector2(const std::string& uniformName_, const Vector2& value_);

		void BindVector3(GLuint uniformID_, const Vector3& value_);
		void BindVector3(const Uniform& uniform_, const Vector3& value_);
		void BindVector3(const std::string& uniformName_, const Vector3& value_);

		void BindColor(GLuint uniformID_, const Color& value_);
		void BindColor(const Uniform& uniform_, const Color& value_);
		void BindColor(const std::string& uniformName_, const Color& value_);

		void BindMatrix3(GLuint uniformID_, const Matrix3& value_);
		void BindMatrix3(const Uniform& uniform_, const Matrix3& value_);
		void BindMatrix3(const std::string& uniformName_, const Matrix3& value_);

		void BindMatrix4(GLuint uniformID_, const Matrix4& value_);
		void BindMatrix4(const Uniform& uniform_, const Matrix4& value_);
		void BindMatrix4(const std::string& uniformName_, const Matrix4& value_);

		void BindTexture(int textureNumber_, GLuint textureID_);
		void BindTexture(const Uniform& uniform_, GLuint textureID_);
		void BindTexture(const std::string& uniformName_, GLuint textureID_);

		void BindTexture3D(int textureNumber_, GLuint textureID_);
		void BindTexture3D(const Uniform& uniform_, GLuint textureID_);
		void BindTexture3D(const std::string& uniformName_, GLuint textureID_);

		void BindCubeMap(int textureNumber_, GLuint textureID_);
		void UnbindTexture(int textureNumber_);
		void UnbindCubeMap(int textureNumber_);

		void BindDirectionalLight(unsigned int id_, const DirectionalLight* light_);
		void BindPointLight(unsigned int id_, const PointLight* light_);
		void BindSpotLight(unsigned int id_, const SpotLight* light_);

		static std::string GetShaderLog(GLuint shader_);

	private:
		std::string fragmentFileName;
		GLuint shader;

		std::unordered_map<std::string, Uniform*> uniforms;
		int currentBoundTextures;

		static int maxTextureUnits;

		Shader(const Shader&) = delete;
		Shader(Shader&&) = delete;
		Shader& operator = (const Shader&) = delete;
		Shader& operator = (Shader&&) = delete;
	};
}

#endif //!SHADER_H
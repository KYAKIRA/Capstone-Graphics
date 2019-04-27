#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include <glew.h>

#include "Graphics/Color.h"
#include "Math/Vector.h"
#include "Object/Component.h"
#include "Object/GameObject.h"

namespace PizzaBox{
	class LightSource : public Component{
	public:
		LightSource(float intensity_, const Color& lightColor_) : Component(), intensity(intensity_), lightColor(lightColor_), ambient(Color(0.3f, 0.3f, 0.3f)), diffuse(Color(0.8f, 0.8f, 0.8f)), specular(Color(1.0f, 1.0f, 1.0f)), castsShadows(true), lightViewMatrix(Matrix4::Identity()), lightSpaceMatrix(Matrix4::Identity()), depthMap(0){}
		virtual ~LightSource(){}

		virtual bool Initialize(GameObject* go_) = 0;
		virtual void Destroy() = 0;

		inline Color GetAmbient() const{ return ambient; }
		inline Color GetDiffuse() const{ return diffuse; }
		inline Color GetSpecular() const{ return specular; }
		inline float GetIntensity() const{ return intensity; }
		inline Color GetColor() const{ return lightColor; }
		inline bool CastsShadows() const{ return castsShadows; }
		inline Matrix4 GetLightViewMatrix() const{ return lightViewMatrix; }
		inline Matrix4 GetLightSpaceMatrix() const{ return lightSpaceMatrix; }
		inline GLuint GetDepthMap() const{ return depthMap; }

		inline void SetAmbient(const Color& c_){ ambient = c_; }
		inline void SetDiffuse(const Color& c_){ diffuse = c_; }
		inline void SetSpecular(const Color& c_){ specular = c_; }
		inline void SetIntensity(float i_){ intensity = i_; }
		inline void SetColor(const Color& c_){ lightColor = c_; }
		inline void SetCastsShadows(bool casts_){ castsShadows = casts_; }
		inline void SetLightViewMatrix(const Matrix4& m_){ lightViewMatrix = m_; }
		inline void SetLightSpaceMatrix(const Matrix4& m_){ lightSpaceMatrix = m_; }
		inline void SetDepthMap(GLuint depthMap_){ depthMap = depthMap_; }

	protected:  
		Color ambient, diffuse, specular; 
		float intensity;
		Color lightColor;
		bool castsShadows;
		Matrix4 lightViewMatrix;
		Matrix4 lightSpaceMatrix;
		GLuint depthMap;
	}; 
}

#endif //!LIGHT_SOURCE_H
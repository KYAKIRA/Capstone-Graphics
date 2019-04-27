#ifndef RAY_MANAGER_H
#define RAY_MANAGER_H

#include <map>
#include <vector>

#include "Graphics/Camera.h"
#include "Graphics/Color.h"
#include "Graphics/Shader.h"
#include "Graphics/LowLevel/Buffer.h"
#include "Graphics/LowLevel/Uniform.h"
#include "Graphics/LowLevel/VAO.h"
#include "Tools/Debug.h"

namespace PizzaBox{
	struct RayCastRender{
		Vector3 vertices[2];
		Color color;

		RayCastRender(Vector3 start_, Vector3 end_, Color color_) : color(color_){
			vertices[0] = start_;
			vertices[1] = end_;
		}
	};

	class RayManager{
	public:
		RayManager();
		~RayManager();

		bool Initialize();
		void Destroy();

		void Render(Camera* cam_);
		void AddRay(RayCastRender ray_, RayMode mode_);
		void ClearPermanent();

	private:
		Uniform *projectionMatrixID, *viewMatrixID, *colorID;
		Shader* rayShader;
		std::string rayShaderName;
		VAO vao;
		Buffer vbo;

		std::map<RayMode, std::vector<RayCastRender>> rays;
	};
}
#endif //!RAY_MANAGER_H
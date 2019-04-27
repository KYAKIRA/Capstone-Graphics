#ifndef GRASS_SIMULATION_H
#define GRASS_SIMULATION_H

#include "Graphics/Materials/MeshMaterial.h"

namespace PizzaBox{
	class GrassSimulation :public Component{
	public:
		GrassSimulation(const std::string& diffMap_ = "GrassTexture", const std::string& specMap_ = "", const std::string& normalMap_ = "", int grassAmount_ = 1, float shiny_ = 32.0f, const Vector3& sway_ = Vector3(0.5f, 0.3f, 0.1f), float frequency_ = 0.025f);
		virtual ~GrassSimulation() override;

		virtual bool Initialize(class GameObject* go_) override;
		virtual void Destroy() override;

		void GenerateField();

	private:
		Vector3 sway;
		float frequency;
		float shininess;
		int grassAmount;
		std::string diffuseMapName;
		std::string specularMapName;
		std::string normalMapName;
		GameObject* grassField;
	};
}

#endif //!GRASS_SIMULATION_H
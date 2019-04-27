#ifndef ANIM_MESH_RENDER_H
#define ANIM_MESH_RENDER_H

#include <string>

#include "Animator.h"
#include "AnimModel.h"
#include "Graphics/Camera.h"
#include "Graphics/Color.h"
#include "Graphics/Lighting/DirectionalLight.h"
#include "Graphics/Lighting/PointLight.h"
#include "Graphics/Lighting/SpotLight.h"
#include "Graphics/Materials/MeshMaterial.h"
#include "Object/Component.h"

namespace PizzaBox{
	class AnimMeshRender : public Component{
	public:
		AnimMeshRender(const std::string& modelName_, const std::string& textureName_, Animator* animator_ = nullptr);
		AnimMeshRender(const std::string& modelName_, const Color& color_, Animator* animator_ = nullptr);
		AnimMeshRender(const std::string& modelName_, const std::vector<MeshMaterial*>& materials_, Animator* animator_ = nullptr);
		AnimMeshRender(const std::string& modelName_, Animator* animator_ = nullptr);
		virtual ~AnimMeshRender() override;

		virtual bool Initialize(GameObject* go_) override;
		virtual void Destroy() override;

		void Update(float deltaTime_);
		void Render(Camera* camera_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector<SpotLight*>& spotLights_) const;

		inline AnimModel* GetAnimModel() const{ return model; }
		inline Animator* GetAnimator() const{ return animator; }
		inline bool CastsShadows() const{ return castsShadows; }

		void SetMaterial(MeshMaterial* material_, size_t index_ = 0);
		void SetAnimator(Animator* animator_);
		inline void SetCastsShadows(bool casts_){ castsShadows = casts_; }

		void BindSkeletonInstance(Shader* shader_) const;

	private:
		std::string modelName;
		AnimModel* model;
		std::vector<MeshMaterial*> materials;
		Animator* animator;
		bool castsShadows;
	};
}

#endif //!ANIM_MESH_RENDER_H
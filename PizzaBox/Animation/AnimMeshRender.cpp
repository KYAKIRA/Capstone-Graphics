#include "AnimMeshRender.h"

#include "Core/GameManager.h"
#include "Graphics/RenderEngine.h"
#include "Graphics/Effects/Shadows.h"
#include "Graphics/Materials/ColorMaterial.h"
#include "Graphics/Materials/TexturedMaterial.h"
#include "Graphics/Models/ModelLoader.h"
#include "Resource/ResourceManager.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

AnimMeshRender::AnimMeshRender(const std::string& modelName_, const std::string& textureName_, Animator* animator_) : Component(), modelName(modelName_), model(nullptr), materials(), animator(animator_), castsShadows(true){
	_ASSERT(!modelName.empty());
	_ASSERT(!textureName_.empty());

	if(animator != nullptr){
		materials.push_back(new TexturedMaterial(textureName_, true));
	}else{
		materials.push_back(new TexturedMaterial(textureName_));
	}
}

AnimMeshRender::AnimMeshRender(const std::string& modelName_, const Color& color_, Animator* animator_) : Component(), modelName(modelName_), model(nullptr), materials(), animator(animator_), castsShadows(true){
	_ASSERT(!modelName_.empty());

	if(animator != nullptr){
		materials.push_back(new ColorMaterial(color_, true));
	}else{
		materials.push_back(new ColorMaterial(color_));
	}
}

AnimMeshRender::AnimMeshRender(const std::string& modelName_, const std::vector<MeshMaterial*>& materials_, Animator* animator_) : Component(), modelName(modelName_), model(nullptr), materials(materials_), animator(animator_), castsShadows(true){
	_ASSERT(!modelName_.empty());
	_ASSERT(!materials_.empty());
}

AnimMeshRender::AnimMeshRender(const std::string& modelName_, Animator* animator_) : Component(), modelName(modelName_), model(nullptr), materials(), animator(animator_), castsShadows(true){
	_ASSERT(!modelName_.empty());
}

AnimMeshRender::~AnimMeshRender(){
	for(auto& m : materials){
		if(m != nullptr){
			m->Destroy();
			delete m;
			m = nullptr;
		}
	}

	materials.clear();
	materials.shrink_to_fit();

	#ifdef _DEBUG
	if(model != nullptr || animator != nullptr){
		Debug::LogWarning("Memory leak detected in AnimMeshRender!", __FILE__, __LINE__);
		Destroy();
	}
	#endif //_DEBUG
}

bool AnimMeshRender::Initialize(GameObject* go_){
	//Makes sure the GameObject is a valid pointer
	_ASSERT(go_ != nullptr);

	gameObject = go_;

	model = ResourceManager::LoadResource<AnimModel>(modelName);
	if(model == nullptr || model->meshList.size() == 0){
		Debug::LogError("Model was not loaded correctly", __FILE__, __LINE__);
		return false;
	}

	if(materials.empty()){
		materials = ModelLoader::LoadMaterials(model->GetFileName(), true);
		if(materials.empty()){
			Debug::LogError("Materials could not be loaded from model file!", __FILE__, __LINE__);
			return false;
		}
	}

	for(auto& m : materials){
		if(m->Initialize() == false){
			Debug::LogError("Material could not be initialized!", __FILE__, __LINE__);
			return false;
		}
	}

	if(animator != nullptr && animator->Initialize(model) == false){
		Debug::LogError("Animator could not be initialized!", __FILE__, __LINE__);
		return false;
	}

	RenderEngine::RegisterAnimMeshRender(this);

	return true;
}

void AnimMeshRender::Destroy(){
	RenderEngine::UnregisterAnimMeshRender(this);

	if(animator != nullptr){
		animator->Destroy();
		delete animator;
		animator = nullptr;
	}

	for(auto& m : materials){
		if(m != nullptr){
			m->Destroy();
		}
	}

	if(model != nullptr){
		ResourceManager::UnloadResource(modelName);
		model = nullptr;
	}

	gameObject = nullptr;
}

void AnimMeshRender::Update(float deltaTime_){
	if(animator != nullptr){
		animator->Update(deltaTime_);
	}
}

void AnimMeshRender::Render(Camera* camera_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector<SpotLight*>& spotLights_) const{
	//Make sure we're getting a valid Camera pointer
	_ASSERT(camera_ != nullptr);
	//Make sure we have a model to render
	_ASSERT(model != nullptr);
	//Make sure we have a material
	_ASSERT(!materials.empty());
	_ASSERT(materials[0] != nullptr);

	Matrix4 modelMatrix = gameObject->GetTransform()->GetTransformation();

	for(unsigned int i = 0; i < model->meshList.size(); i++){
		int mi = i;

		if(i < materials.size() && materials[i] != nullptr){
			mi = i;
		}else{
			mi = 0;
		}

		materials[mi]->Update();
		materials[mi]->Render(camera_, modelMatrix, dirLights_, pointLights_, spotLights_);

		BindSkeletonInstance(materials[mi]->GetShader());

		model->meshList[i]->Render();

		materials[mi]->GetShader()->Unbind();
	}
}

void AnimMeshRender::SetMaterial(MeshMaterial* material_, size_t index_){
	//Must be a valid AnimMaterial pointer
	_ASSERT(material_ != nullptr);

	while(index_ >= materials.size()){
		materials.push_back(nullptr);
	}

	if(materials[index_] != nullptr){
		materials[index_]->Destroy();
		delete materials[index_];
		materials[index_] = nullptr;
	}

	materials[index_] = material_;
	if(materials[index_]->Initialize() == false){
		Debug::DisplayFatalErrorMessage("Material Initialization Error", "New material could not be initialized!");
		GameManager::Stop();
	}
}

void AnimMeshRender::SetAnimator(Animator* animator_){
	if(animator != nullptr){
		animator->Destroy();
		delete animator;
		animator = nullptr;
	}

	if(animator_ == nullptr){
		return;
	}

	animator = animator_;
	if(animator->Initialize(model) == false){
		Debug::DisplayFatalErrorMessage("Material Initialization Error", "New material could not be initialized!");
		GameManager::Stop();
	}
}

void AnimMeshRender::BindSkeletonInstance(Shader* shader_) const{
	_ASSERT(shader_ != nullptr);

	if(animator != nullptr){
		for(unsigned int i = 0; i < model->skeleton->GetJointCount(); i++){
			shader_->BindMatrix4("bones[" + std::to_string(i) + "]", animator->GetJointTransform(i));
		}
	}
}
#include "MeshRender.h"

#include "Core/GameManager.h"
#include "Graphics/Color.h"
#include "Graphics/Materials/ColorMaterial.h"
#include "Graphics/Materials/TexturedMaterial.h"
#include "Graphics/Models/ModelLoader.h"
#include "Graphics/RenderEngine.h"
#include "Math/Matrix.h"
#include "Resource/ResourceManager.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

MeshRender::MeshRender(const std::string& modelName_) : Component(), modelName(modelName_), model(nullptr), materials(), castsShadows(true){
	_ASSERT(!modelName_.empty());
}

MeshRender::MeshRender(const std::string& modelName_, MeshMaterial* material_) : Component(), modelName(modelName_), model(nullptr), materials(), castsShadows(true){
	_ASSERT(!modelName_.empty());
	_ASSERT(material_ != nullptr);
	materials.push_back(material_);
}

MeshRender::MeshRender(const std::string& modelName_, const std::string& textureName_) : Component(), modelName(modelName_), model(nullptr), castsShadows(true){
	_ASSERT(!textureName_.empty());
	materials.push_back(new TexturedMaterial(textureName_));
}

MeshRender::MeshRender(const std::string& modelName_, const std::vector<MeshMaterial*>& materials_) : Component(), modelName(modelName_), model(nullptr), materials(materials_), castsShadows(true){
	_ASSERT(!modelName_.empty());
	_ASSERT(!materials_.empty());
}

MeshRender::MeshRender(const std::string& modelName_, const Color& color_) : Component(), modelName(modelName_), model(nullptr), materials(), castsShadows(true){
	_ASSERT(!modelName_.empty());
	materials.push_back(new ColorMaterial(color_));
}

MeshRender::~MeshRender(){
	for(auto& m : materials){
		if(m != nullptr){
			m->Destroy();
			delete m;
			m = nullptr;
		}
	}

	materials.clear();
	materials.shrink_to_fit();
}

bool MeshRender::Initialize(GameObject* go_){
	//Makes sure the GameObject is a valid pointer
	_ASSERT(go_ != nullptr);

	gameObject = go_;

	model = ResourceManager::LoadResource<Model>(modelName);
	if(model == nullptr){
		Debug::LogError("Model was not loaded correctly", __FILE__, __LINE__);
		return false;
	}

	if(materials.empty()){
		materials = ModelLoader::LoadMaterials(model->GetFileName(), false);
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

	RenderEngine::RegisterMeshRender(this);
	return true;
}

void MeshRender::Destroy(){
	RenderEngine::UnregisterMeshRender(this);

	if(model != nullptr){
		ResourceManager::UnloadResource(modelName);
		model = nullptr;
	}

	for(auto& m : materials){
		if(m != nullptr){
			m->Destroy();
		}
	}

	gameObject = nullptr;
}

void MeshRender::Render(Camera* camera_, const std::vector<DirectionalLight*>& dirLights_, const std::vector<PointLight*>& pointLights_, const std::vector<SpotLight*>& spotLights_){
	//Make sure we're getting a valid Camera pointer
	_ASSERT(camera_ != nullptr);
	//Make sure we have a model to render
	_ASSERT(model != nullptr);
	//Make sure we have a material
	_ASSERT(!materials.empty());
	_ASSERT(materials.front() != nullptr);

	Matrix4 modelMatrix = gameObject->GetTransform()->GetTransformation();

	for(unsigned int i = 0; i < model->meshList.size(); i++){
		MeshMaterial* materialToUse = nullptr;
		if(i < materials.size() && materials[i] != nullptr){
			materialToUse = materials[i];
		}else{
			materialToUse = materials.front();
		}

		materialToUse->Update();
		materialToUse->Render(camera_, modelMatrix, dirLights_, pointLights_, spotLights_);
		model->meshList[i]->Render();

		materialToUse->GetShader()->Unbind();
	}
}

void MeshRender::SetMaterial(MeshMaterial* material_, size_t index_){
	//Must be a valid MeshMaterial pointer
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
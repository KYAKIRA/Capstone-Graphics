#include "AnimModel.h"

#include "Graphics/Models/ModelLoader.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

AnimModel::AnimModel(const std::string& filePath_) : Resource(filePath_), skeleton(nullptr), globalInverse(Matrix4::Identity()){
}

AnimModel::~AnimModel(){
	#ifdef _DEBUG
	if(!meshList.empty()){
		Debug::LogWarning("Memory Leak Detected in AnimModel!", __FILE__, __LINE__);
		Unload();
	}
	#endif //_DEBUG
}

bool AnimModel::Load(){
	if(ModelLoader::LoadAnimModel(fileName, *this) == false){
		Debug::LogError(fileName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	meshList.shrink_to_fit();

	return true;
}

void AnimModel::Unload(){
	for(AnimMesh* mesh : meshList){
		delete mesh;
		mesh = nullptr;
	}

	meshList.clear();
	meshList.shrink_to_fit();
}
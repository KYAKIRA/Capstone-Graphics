#include "Model.h"

#include "ModelLoader.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

Model::Model(const std::string filePath_) : Resource(filePath_){
}

Model::~Model(){
}

bool Model::Load(){
	meshList = ModelLoader::LoadSimpleModel(fileName);
	if(meshList.empty()){
		Debug::LogError(fileName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	meshList.shrink_to_fit();

	return true;
}

void Model::Unload(){
	for(Mesh* mesh : meshList){
		delete mesh;
		mesh = nullptr;
	}

	meshList.clear();
	meshList.shrink_to_fit();
}
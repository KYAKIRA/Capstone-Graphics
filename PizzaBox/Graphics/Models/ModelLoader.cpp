#include "ModelLoader.h"

#include "Graphics/Materials/ColorMaterial.h"
#include "Tools/Debug.h"

using namespace PizzaBox;

std::vector<Mesh*> ModelLoader::LoadSimpleModel(std::string filePath_){
	std::vector<Mesh*> meshList = std::vector<Mesh*>();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath_, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		Debug::LogError("AssImp could not load model! AssImp Error: " + std::string(importer.GetErrorString()));
		//Return the empty mesh list so that we know an error has occured
		return meshList;
	}
	
	//Recursively process every node of the AssImp scene
	ProcessSimpleNode(scene->mRootNode, scene, meshList);
	return meshList;
}

bool ModelLoader::LoadAnimModel(std::string filePath_, AnimModel& model_){
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath_, aiProcess_ValidateDataStructure | aiProcess_FindInvalidData | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_LimitBoneWeights);
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		Debug::LogError("AssImp could not load model! AssImp Error: " + std::string(importer.GetErrorString()));
		return false;
	}

	Skeleton* skeleton = MakeSkeleton(scene);
	SkinningData skinningData = LoadSkinningData(skeleton, scene);
	ProcessAnimNode(scene, model_.meshList, skeleton, skinningData);

	model_.skeleton = skeleton;
	model_.globalInverse = Matrix4(scene->mRootNode->mTransformation).Inverse();

	return true;
}

bool ModelLoader::LoadAnimClips(const std::string& filePath_, AnimClip& clip_, unsigned int clipID_){
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath_, aiProcess_ValidateDataStructure | aiProcess_FindInvalidData | aiProcess_ImproveCacheLocality);
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		Debug::LogError("AssImp could not load model! AssImp Error: " + std::string(importer.GetErrorString()));
		return false;
	}

	if(scene->HasAnimations() == false || scene->mNumAnimations <= clipID_){
		Debug::LogError("This file has no (or not enough) animations!", __FILE__, __LINE__);
		return false;
	}

	auto anim = scene->mAnimations[clipID_];

	clip_.SetLength(static_cast<float>(anim->mDuration / anim->mTicksPerSecond));

	unsigned int totalKeyFrames = 0;

	for(unsigned int i = 0; i < anim->mNumChannels; i++){
		for(unsigned int j = 0; j < anim->mChannels[i]->mNumPositionKeys; j++){
			clip_.AddPosKey(anim->mChannels[i]->mNodeName.C_Str(), PosKeyFrame(static_cast<float>(anim->mChannels[i]->mPositionKeys[j].mTime / anim->mTicksPerSecond),
				Vector3(anim->mChannels[i]->mPositionKeys[j].mValue)));
			totalKeyFrames++;
		}

		for(unsigned int j = 0; j < anim->mChannels[i]->mNumRotationKeys; j++){
			clip_.AddRotKey(anim->mChannels[i]->mNodeName.C_Str(), RotKeyFrame(static_cast<float>(anim->mChannels[i]->mRotationKeys[j].mTime / anim->mTicksPerSecond),
				Quaternion(anim->mChannels[i]->mRotationKeys[j].mValue)));
			totalKeyFrames++;
		}

		for(unsigned int j = 0; j < anim->mChannels[i]->mNumScalingKeys; j++){
			clip_.AddScaleKey(anim->mChannels[i]->mNodeName.C_Str(), ScaleKeyFrame(static_cast<float>(anim->mChannels[i]->mScalingKeys[j].mTime / anim->mTicksPerSecond),
				Vector3(anim->mChannels[i]->mScalingKeys[j].mValue)));
			totalKeyFrames++;
		}
	}

	Debug::Log("Total Key Frames for this animation: " + std::to_string(totalKeyFrames), __FILE__, __LINE__);

	return true;
}

std::vector<MeshMaterial*> ModelLoader::LoadMaterials(const std::string& filePath_, bool animated_){
	std::vector<MeshMaterial*> materialList = std::vector<MeshMaterial*>();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath_, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		Debug::LogError("AssImp could not load model! AssImp Error: " + std::string(importer.GetErrorString()));
		//Return the empty mesh list so that we know an error has occured
		return materialList;
	}

	for(unsigned int i = 0; i < scene->mNumMaterials; i++){
		auto curMat = scene->mMaterials[i];
		if(curMat->GetTextureCount(aiTextureType_DIFFUSE) > 0){
			Debug::LogError("Cannot process materials with textures!");
			return materialList;
		}

		aiColor4D color;
		float shiny;
		curMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		curMat->Get(AI_MATKEY_SHININESS, shiny);

		Color c = Color(color.r, color.g, color.b, color.a);
		materialList.push_back(new PizzaBox::ColorMaterial(c, animated_, shiny));
	}

	return materialList;
}

void ModelLoader::ProcessSimpleNode(const aiNode* node, const aiScene* scene, std::vector<Mesh*>& meshList_){
	//Reserve the appropriate capacity for the number of meshes on this node
	meshList_.reserve(meshList_.size() + node->mNumMeshes);

	for(unsigned int i = 0; i < node->mNumMeshes; i++){
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		
		std::vector<Vertex> verts;
		std::vector<unsigned int> indices;

		verts.reserve(mesh->mNumVertices);
		indices.reserve(mesh->mNumFaces * 3);

		for(unsigned int j = 0; j < mesh->mNumVertices; j++){
			verts.push_back(Vertex(
				Vector3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z),
				Vector3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z),
				Vector2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y)
			));
		}

		for(unsigned int j = 0; j < mesh->mNumFaces; j++){
			for(unsigned int k = 0; k < mesh->mFaces[j].mNumIndices; k++){
				indices.push_back(mesh->mFaces[j].mIndices[k]);
			}
		}

		meshList_.push_back(new Mesh(verts, indices));
	}
	
	for(unsigned int i = 0; i < node->mNumChildren; i++){
		ProcessSimpleNode(node->mChildren[i], scene, meshList_);
	}
}

void ModelLoader::ProcessAnimNode(const aiScene* scene_, std::vector<AnimMesh*>& meshList_, Skeleton* skeleton_, const SkinningData& skinningData_){
	//Reserve the appropriate capacity for the number of meshes on this node
	meshList_.reserve(meshList_.size() + scene_->mNumMeshes);

	for(unsigned int i = 0; i < scene_->mNumMeshes; i++){
		aiMesh* mesh = scene_->mMeshes[i];

		std::vector<AnimVertex> verts;
		std::vector<unsigned int> indices;

		verts.reserve(mesh->mNumVertices);
		indices.reserve(mesh->mNumFaces * 3);

		for(unsigned int j = 0; j < mesh->mNumVertices; j++){
			verts.push_back(AnimVertex(
				Vector3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z),
				Vector3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z),
				Vector2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y)
			));
		}

		for(unsigned int j = 0; j < mesh->mNumFaces; j++){
			for(unsigned int k = 0; k < mesh->mFaces[j].mNumIndices; k++){
				indices.push_back(mesh->mFaces[j].mIndices[k]);
			}
		}

		meshList_.push_back(new AnimMesh(verts, indices, skeleton_, skinningData_, i));
	}
}

Skeleton* ModelLoader::MakeSkeleton(const aiScene* scene_){
	std::vector<Joint> joints;

	for(unsigned int i = 0; i < scene_->mNumMeshes; i++){
		for(unsigned int j = 0; j < scene_->mMeshes[i]->mNumBones; j++){
			const auto& bone = scene_->mMeshes[i]->mBones[j];

			if(HasJoint(joints, bone->mName.C_Str())){
				continue;
			}

			Joint joint;
			joint.name = bone->mName.C_Str();
			joint.inverseBindPose = Matrix4(bone->mOffsetMatrix);

			joints.push_back(joint);
		}
	}

	return BuildSkeleton(joints, scene_);
}

SkinningData ModelLoader::LoadSkinningData(Skeleton* skeleton_, const aiScene* scene_){
	SkinningData data = SkinningData();
	data.meshData.reserve(scene_->mNumMeshes);

	for(unsigned int i = 0; i < scene_->mNumMeshes; i++){
		data.meshData.push_back(MeshData());

		for(unsigned int j = 0; j < scene_->mMeshes[i]->mNumBones; j++){
			const auto& bone = scene_->mMeshes[i]->mBones[j];
			data.meshData[i].vertexWeights[bone->mName.C_Str()].reserve(bone->mNumWeights);

			for(unsigned int k = 0; k < bone->mNumWeights; k++){
				data.meshData[i].vertexWeights[bone->mName.C_Str()].push_back(VertexWeight(
					bone->mWeights[k].mVertexId,
					bone->mWeights[k].mWeight
				));
			}
		}
	}

	return data;
}

bool ModelLoader::HasJoint(const std::vector<Joint>& joints, std::string name_){
	for(const Joint& j : joints){
		if(j.name == name_){
			return true;
		}
	}

	return false;
}

Joint& ModelLoader::GetJoint(std::vector<Joint>& joints, std::string name_){
	for(Joint& j : joints){
		if(j.name == name_){
			return j;
		}
	}

	throw std::exception("Invalid joint access!");
}

Skeleton* ModelLoader::BuildSkeleton(std::vector<Joint>& joints, const aiScene* scene){
	aiNode* rootBoneNode = nullptr;
	Joint rootJoint;

	for(Joint& j : joints){
		aiNode* node = scene->mRootNode->FindNode(j.name.c_str());
		if(node != nullptr && (node->mParent == nullptr || HasJoint(joints, node->mParent->mName.C_Str()) == false)){
			j.parentID = -1; //-1 denotes that this is the root node
			rootBoneNode = node;
			rootJoint = j;
			break;
		}
	}

	Skeleton* skeleton = new Skeleton();
	skeleton->AddJoint(rootJoint);

	BuildSkeletonChildren(joints, skeleton, rootBoneNode, 0);

	return skeleton;
}

void ModelLoader::BuildSkeletonChildren(std::vector<Joint>& joints, Skeleton* skeleton, const aiNode* node, unsigned int parentID_){
	for(unsigned int i = 0; i < node->mNumChildren; i++){
		for(Joint& j : joints){
			if(node->mChildren[i]->mName.C_Str() == j.name){
				j.parentID = parentID_;
				//j.transform = Matrix4(node->mTransformation);

				skeleton->AddJoint(j);

				BuildSkeletonChildren(joints, skeleton, node->mChildren[i], skeleton->GetJointCount() - 1);
			}
		}
	}
}
#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Animation/AnimModel.h"
#include "Animation/AnimClip.h"
#include "Graphics/Materials/MeshMaterial.h"

namespace PizzaBox{
	class ModelLoader{
	public:
		static std::vector<Mesh*> LoadSimpleModel(std::string filePath_);
		static bool LoadAnimModel(std::string filePath_, AnimModel& model_);
		static bool LoadAnimClips(const std::string& filePath_, AnimClip& clip_, unsigned int clipID_ = 0);

		static std::vector<MeshMaterial*> LoadMaterials(const std::string& filePath_, bool animated_);

	private:
		static void ProcessSimpleNode(const aiNode* node, const aiScene* scene, std::vector<Mesh*>& meshList_);
		static void ProcessAnimNode(const aiScene* scene_, std::vector<AnimMesh*>& meshList_, Skeleton* skeleton_, const SkinningData& data_);
		static Skeleton* MakeSkeleton(const aiScene* scene_);
		static SkinningData LoadSkinningData(Skeleton* skeleton_, const aiScene* scene_);
		static bool HasJoint(const std::vector<Joint>& joints, std::string name_);
		static Joint& GetJoint(std::vector<Joint>& joints, std::string name_);
		static Skeleton* BuildSkeleton(std::vector<Joint>& joints, const aiScene* scene);
		static void BuildSkeletonChildren(std::vector<Joint>& joints, Skeleton* skeleton, const aiNode* node, unsigned int parentID_);
	};
}

#endif //!MODEL_LOADER_H
#ifndef SKINNING_DATA_H
#define SKINNING_DATA_H

#include <string>
#include <map>
#include <vector>

namespace PizzaBox{
	struct VertexWeight{
		VertexWeight(size_t id_, float weight_) : id(id_), weight(weight_){
		}

		size_t id;
		float weight;
	};

	struct MeshData{
		std::map<std::string, std::vector<VertexWeight>> vertexWeights;
	};
	
	struct SkinningData{
		std::vector<MeshData> meshData;

		inline size_t GetMeshes() const{ return meshData.size(); }

		inline size_t GetJoints(size_t mesh_) const{
			_ASSERT(HasDataForMesh(mesh_));
			return meshData[mesh_].vertexWeights.size();
		}

		inline size_t GetVertices(size_t mesh_, const std::string& joint_) const{
			_ASSERT(HasDataForJoint(mesh_, joint_));
			return meshData[mesh_].vertexWeights.at(joint_).size();
		}

		inline const std::vector<VertexWeight>& GetDataForJoint(size_t mesh_, const std::string& joint_) const{
			_ASSERT(HasDataForJoint(mesh_, joint_));
			return meshData[mesh_].vertexWeights.at(joint_);
		}

		inline std::vector<VertexWeight>& GetDataForJoint(size_t mesh_, const std::string& joint_){
			_ASSERT(HasDataForJoint(mesh_, joint_));
			return meshData[mesh_].vertexWeights.at(joint_);
		}

		inline bool HasDataForMesh(size_t mesh_) const{
			return (meshData.size() > mesh_);
		}

		inline bool HasDataForJoint(size_t mesh_, const std::string& joint_) const{
			_ASSERT(HasDataForMesh(mesh_));
			return (meshData[mesh_].vertexWeights.find(joint_) != meshData[mesh_].vertexWeights.end());
		}
	};
}

#endif //!SKINNING_DATA_H
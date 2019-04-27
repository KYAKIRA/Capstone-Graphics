#ifndef ANIM_MESH_H
#define ANIM_MESH_H

#include <vector>

#include "AnimVertex.h"
#include "Skeleton.h"
#include "SkinningData.h"
#include "Graphics/LowLevel/VAO.h"
#include "Graphics/LowLevel/Buffer.h"

namespace PizzaBox{
	class AnimMesh{
	public:
		AnimMesh(const std::vector<AnimVertex>& verts_, const std::vector<unsigned int>& indices_, const Skeleton* skeleton_, const SkinningData& skinningData_, size_t meshIndex_);
		~AnimMesh();

		std::vector<AnimVertex> vertices;
		std::vector<unsigned int> indices;

		void Render() const;

	private:
		VAO vao;
		Buffer vbo;
		Buffer ebo;

		void SetVertexWeights(const Skeleton* skeleton_, const SkinningData& skinningData_, size_t meshIndex_);
		void GenerateBuffers();
	};
}

#endif //!ANIM_MESH_H
#include "AnimMesh.h"

#include "Tools/Debug.h"

using namespace PizzaBox;

AnimMesh::AnimMesh(const std::vector<AnimVertex>& verts_, const std::vector<unsigned int>& indices_, const Skeleton* skeleton_, const SkinningData& skinningData_, size_t meshIndex_) : vertices(verts_), indices(indices_), vao(), vbo(GL_ARRAY_BUFFER), ebo(GL_ELEMENT_ARRAY_BUFFER){
	SetVertexWeights(skeleton_, skinningData_, meshIndex_);
	GenerateBuffers();
}

AnimMesh::~AnimMesh(){
}

void AnimMesh::Render() const{
	//Bind the VAO that you want to use for drawing
	vao.Bind();
	ebo.Bind();

	//Render the array stored in the bound VAO by the indices in the EBO
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

	//Clear the vertex array for future use
	vao.Unbind();
	ebo.Unbind();
}

void AnimMesh::SetVertexWeights(const Skeleton* skeleton_, const SkinningData& skinningData_, size_t meshIndex_){
	std::map<size_t, std::vector<VertexWeight>> allPairsByVertex;
	for(size_t i = 0; i < vertices.size(); i++){
		allPairsByVertex[i] = std::vector<VertexWeight>();
		allPairsByVertex[i].reserve(4);
	}

	for(unsigned int i = 0; i < skeleton_->GetJointCount(); i++){
		std::string jointName = skeleton_->GetJoint(i).name;
		if(skinningData_.HasDataForJoint(meshIndex_, jointName)){
			auto dataForThis = skinningData_.GetDataForJoint(meshIndex_, jointName);
			for(unsigned int j = 0; j < dataForThis.size(); j++){
				allPairsByVertex[dataForThis[j].id].push_back(VertexWeight(i, dataForThis[j].weight));
			}
		}
	}

	for(size_t i = 0; i < vertices.size(); i++){
		for(size_t j = 0; j < allPairsByVertex[i].size() && j < AnimVertex::maxJointWeights; j++){
			vertices[i].jointIDs[j] = static_cast<unsigned int>(allPairsByVertex[i][j].id);
			vertices[i].jointWeights[j] = allPairsByVertex[i][j].weight;
		}
	}
}

void AnimMesh::GenerateBuffers(){
	vao.Bind();
	vbo.Bind();
	vbo.SetBufferData(vertices.size() * sizeof(AnimVertex), &vertices[0], GL_STATIC_DRAW);

	ebo.Bind();
	ebo.SetBufferData(indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	vao.SetupVertexAttribute(0, 3, sizeof(AnimVertex), (GLvoid*)(offsetof(AnimVertex, AnimVertex::position)));
	vao.SetupVertexAttribute(1, 3, sizeof(AnimVertex), (GLvoid*)(offsetof(AnimVertex, AnimVertex::normal)));
	vao.SetupVertexAttribute(2, 2, sizeof(AnimVertex), (GLvoid*)(offsetof(AnimVertex, AnimVertex::texCoords)));
	vao.SetupVertexIntAttribute(3, 4, sizeof(AnimVertex), (GLvoid*)(offsetof(AnimVertex, AnimVertex::jointIDs)));
	vao.SetupVertexAttribute(4, 4, sizeof(AnimVertex), (GLvoid*)(offsetof(AnimVertex, AnimVertex::jointWeights)));

	vbo.Unbind();
	vao.Unbind();
	ebo.Unbind();
}
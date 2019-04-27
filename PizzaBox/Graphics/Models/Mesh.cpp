#include "Mesh.h"

using namespace PizzaBox;

Mesh::Mesh(const std::vector<Vertex>& verts_, const std::vector<unsigned int>& indices_) : vertices(verts_), indices(indices_), vao(), vbo(GL_ARRAY_BUFFER), ebo(GL_ELEMENT_ARRAY_BUFFER){
	vertices.shrink_to_fit();
	indices.shrink_to_fit();

	GenerateBuffers();
}

Mesh::Mesh(const std::vector<Vector3>& vertList_, const std::vector<Vector3>& norm_, const std::vector<Vector2>& uvcoords_, const std::vector<unsigned int>& indices_) : vertices(std::vector<Vertex>()), indices(indices_), vao(), vbo(GL_ARRAY_BUFFER), ebo(GL_ELEMENT_ARRAY_BUFFER){
	vertices.reserve(vertList_.size());
	for(unsigned int i = 0; i < vertList_.size(); i++){
		vertices.push_back(Vertex(vertList_[i], norm_[i], uvcoords_[i]));
	}

	vertices.shrink_to_fit();
	indices.shrink_to_fit();

	GenerateBuffers();
}

Mesh::~Mesh(){
}

void Mesh::GenerateBuffers(){
	vao.Bind();
	vbo.Bind();
	vbo.SetBufferData(vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	ebo.Bind();
	ebo.SetBufferData(indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	vao.SetupVertexAttribute(0, 3, sizeof(Vertex), (GLvoid*)(0));
	vao.SetupVertexAttribute(1, 3, sizeof(Vertex), (GLvoid*)(sizeof(Vector3)));
	vao.SetupVertexAttribute(2, 2, sizeof(Vertex), ((GLvoid*)(sizeof(Vector3) + sizeof(Vector3))));

	vbo.Unbind();
	vao.Unbind();
	ebo.Unbind();
}

void Mesh::Render() const{
	//Bind the VAO that you want to use for drawing
	vao.Bind();
	ebo.Bind();

	//Render the array stored in the bound VAO by the indices in the EBO
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	
	//Clear the vertex array for future use
	vao.Unbind();
	ebo.Unbind();
}

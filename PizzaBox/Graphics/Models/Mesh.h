#ifndef MESH_H
#define MESH_H

#include <vector>

#include <glew.h>

#include "Graphics/Vertex.h"
#include "Graphics/LowLevel/VAO.h"
#include "Graphics/LowLevel/Buffer.h"
#include "Math/Vector.h"

namespace PizzaBox{
	class Mesh{
	public:
		Mesh(const std::vector<Vertex>& verts_, const std::vector<unsigned int>& indices_);
		Mesh(const std::vector<Vector3>& vertList_, const std::vector<Vector3>& norm_, const std::vector<Vector2>& uvcoords_, const std::vector<unsigned int>& indices_);
		~Mesh();

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		void Render() const;

	private:
		VAO vao;
		Buffer vbo;
		Buffer ebo;

		//GLuint vao, vbo, ebo; //VAO: Vertex Array Object / VBO: Vertex Buffer Object / EBO: Element Buffer Object
		void GenerateBuffers();
	};
}

#endif //!MESH_H
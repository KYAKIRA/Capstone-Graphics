#ifndef VAO_H
#define VAO_H

#include <glew.h>

namespace PizzaBox{
	class VAO{
	public:
		VAO();
		~VAO();

		GLuint id;

		void Bind() const;
		void Unbind() const;

		void SetupVertexAttribute(unsigned int id_, unsigned int num_, const unsigned int stride_, const GLvoid* offset_);
		void SetupVertexIntAttribute(unsigned int id_, unsigned int num_, unsigned int stride_, const GLvoid* offset_);
	};
}

#endif //!VAO_H
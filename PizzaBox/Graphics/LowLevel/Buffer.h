#ifndef BUFFER_H
#define BUFFER_H

#include <glew.h>

namespace PizzaBox{
	class Buffer{
	public:
		Buffer(GLenum type_);
		~Buffer();

		GLuint id;
		GLenum bufferType;

		void Bind() const;
		void Unbind() const;

		void SetBufferData(size_t size_, const void* data_, GLenum usage_);
		void SetBufferSubData(GLintptr offset_, GLsizeiptr size_, const GLvoid* data_);
	};
}

#endif //!BUFFER_H
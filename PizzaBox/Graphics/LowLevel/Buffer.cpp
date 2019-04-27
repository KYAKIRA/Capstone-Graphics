#include "Buffer.h"

using namespace PizzaBox;

Buffer::Buffer(GLenum type_) : bufferType(type_){
	glGenBuffers(1, &id);
}

Buffer::~Buffer(){
	glDeleteBuffers(1, &id);
}

void Buffer::Bind() const{
	glBindBuffer(bufferType, id);
}

void Buffer::Unbind() const{
	glBindBuffer(bufferType, 0);
}

void Buffer::SetBufferData(size_t size_, const void* data_, GLenum usage_){
	glBufferData(bufferType, size_, data_, usage_);
}

void Buffer::SetBufferSubData(GLintptr offset_, GLsizeiptr size_, const GLvoid* data_){
	glBufferSubData(bufferType, offset_, size_, data_);
}
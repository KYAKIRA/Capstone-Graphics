#include "VAO.h"

#include "Graphics/Vertex.h"

using namespace PizzaBox;

VAO::VAO() : id(0){
	glGenVertexArrays(1, &id);
}

VAO::~VAO(){
	glDeleteVertexArrays(1, &id);
}

void VAO::Bind() const{
	glBindVertexArray(id);
}

void VAO::Unbind() const{
	glBindVertexArray(0);
}

void VAO::SetupVertexAttribute(unsigned int id_, unsigned int num_, const unsigned int stride_, const GLvoid* offset_){
	glVertexAttribPointer(id_, num_, GL_FLOAT, GL_FALSE, stride_, offset_);
	glEnableVertexAttribArray(id_);
}

void VAO::SetupVertexIntAttribute(unsigned int id_, unsigned int num_, const unsigned int stride_, const GLvoid* offset_){
	glVertexAttribIPointer(id_, num_, GL_INT, stride_, offset_);
	glEnableVertexAttribArray(id_);
}
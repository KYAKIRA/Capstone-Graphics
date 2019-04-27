#include "Uniform.h"

#include "Graphics/Shader.h"

using namespace PizzaBox;

Uniform::Uniform(Shader* shader_, const std::string& name_){
	id = shader_->GetUniformID(name_);
}
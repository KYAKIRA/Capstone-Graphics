#ifndef UNIFORM_H
#define UNIFORM_H

#include <string>

#include <glew.h>

namespace PizzaBox{
	//Forward declaration
	class Shader;

	class Uniform{
	public:
		Uniform(Shader* shader_, const std::string& name_);

		GLuint id;
	};
}

#endif //!UNIFORM_H
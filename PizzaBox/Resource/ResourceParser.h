#ifndef RESOURCE_PARSER_H
#define RESOURCE_PARSER_H

#include <string>

namespace PizzaBox{
	class ResourceParser{
	public:
		static bool ParseEngineResources();
		static bool ParseGameResources();

	private:
		static class Shader* ParseShader(std::string entry); 
		 
		//Delete unwanted compiler generated constructors, destructors and assignment operators
		ResourceParser() = delete;
		ResourceParser(const ResourceParser&) = delete;
		ResourceParser(ResourceParser&&) = delete;
		ResourceParser& operator=(const ResourceParser&) = delete;
		ResourceParser& operator=(ResourceParser&&) = delete;
		~ResourceParser() = delete; 
	};
}

#endif //!RESOURCE_PARSER_H
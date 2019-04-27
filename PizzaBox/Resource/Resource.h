#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>

namespace PizzaBox{
	class Resource{
	public:
		Resource(const std::string& file_) : fileName(file_){}
		virtual ~Resource(){}

		virtual bool Load() = 0;
		virtual void Unload() = 0;

		inline std::string GetFileName(){ return fileName; }

	protected:
		std::string fileName;
	};
}

#endif //!RESOURCE_H
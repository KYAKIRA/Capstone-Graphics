#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Resource/Resource.h"

namespace PizzaBox{
	class Model : public Resource {
	public:
		explicit Model(const std::string filePath_);
		~Model();

		std::vector<Mesh*> meshList;

		bool Load() override;
		void Unload() override;
	};
}

#endif //!MODEL_H
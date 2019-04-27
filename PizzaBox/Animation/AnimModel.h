#ifndef ANIM_MODEL_H
#define ANIM_MODEL_H

#include <string>
#include <vector>

#include "AnimMesh.h"
#include "Skeleton.h"
#include "Resource/Resource.h"

namespace PizzaBox{
	class AnimModel : public Resource{
	public:
		explicit AnimModel(const std::string& filePath_);
		virtual ~AnimModel() override;

		std::vector<AnimMesh*> meshList;
		Skeleton* skeleton;
		Matrix4 globalInverse;

		virtual bool Load() override;
		virtual void Unload() override;
	};
}

#endif //!ANIM_MODEL_H
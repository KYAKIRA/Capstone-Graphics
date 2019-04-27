#ifndef ANIM_VERTEX_H
#define ANIM_VERTEX_H

#include "Math/Vector.h"

namespace PizzaBox{
	struct AnimVertex{
		AnimVertex(const Vector3& position_, const Vector3& normal_, const Vector2& texCoords_) : position(position_), normal(normal_), texCoords(texCoords_), jointIDs(), jointWeights(){
		}

		static constexpr unsigned int maxJointWeights = 4;

		Vector3 position;
		Vector3 normal;
		Vector2 texCoords;
		unsigned int jointIDs[maxJointWeights];
		float jointWeights[maxJointWeights];
	};
}

#endif //!ANIM_VERTEX_H
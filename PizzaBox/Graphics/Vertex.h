#ifndef VERTEX_H
#define VERTEX_H

#include "../Math/Vector.h"

namespace PizzaBox{
	struct Vertex{
		Vertex(Vector3 position_, Vector3 normal_, Vector2 uvCoords_) : position(position_), normal(normal_), uvCoords(uvCoords_){
		}

		Vector3 position;
		Vector3 normal;
		Vector2 uvCoords;
	};
}

#endif //!VERTEX_H
#ifndef COLLIDER_TYPES_H
#define COLLIDER_TYPES_H

#include <string>
#include <vector>

#include <reactphysics3d.h>

#include "Math/Vector.h"

namespace PizzaBox{
	struct BaseCollider{
		enum class Shape{
			Box,
			Sphere,
			Capsule,
			Convex,
			Concave
		};

		BaseCollider(Shape shape_) : shape(shape_){}
		virtual ~BaseCollider(){}

		Shape shape;
	};

	struct BoxCollider : public BaseCollider{
		BoxCollider(const Vector3& scale_) : BaseCollider(Shape::Box), scale(scale_){}

		Vector3 scale;
	};

	struct SphereCollider : public BaseCollider{
		SphereCollider(float radius_) : BaseCollider(Shape::Sphere), radius(radius_){}

		float radius;
	};

	struct CapsuleCollider : public BaseCollider{
		CapsuleCollider(float radius_, float height_) : BaseCollider(Shape::Capsule), radius(radius_), height(height_){}

		float radius;
		float height;
	};

	struct ConvexCollider : public BaseCollider{
		ConvexCollider(const std::string& modelName_);
		~ConvexCollider();

		rp3d::ConvexMeshShape* shape;
	};

	struct ConcaveCollider : public BaseCollider{
		ConcaveCollider(const std::string& modelName_);
		~ConcaveCollider();

		std::vector<float> vertices;
		std::vector<int> indices;
		rp3d::TriangleVertexArray* triangleArray;
		rp3d::TriangleMesh* mesh;
		rp3d::ConcaveShape* shape;
	};
}

#endif //!COLLIDER_TYPES_H
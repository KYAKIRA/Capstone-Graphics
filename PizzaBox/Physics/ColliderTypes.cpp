#include "ColliderTypes.h"

#include "Graphics/Models/Model.h"
#include "Resource/ResourceManager.h"

using namespace PizzaBox;

ConvexCollider::ConvexCollider(const std::string& modelName_) : BaseCollider(Shape::Convex), shape(nullptr){
	_ASSERT(!modelName_.empty());
	Model* model = ResourceManager::LoadResource<Model>(modelName_);
	if(model == nullptr){
		throw std::exception("Model load failed!");
	}

	std::vector<float> vertices;
	std::vector<int> indices;

	for(const auto& m : model->meshList){
		for(const auto& v : m->vertices){
			vertices.push_back(v.position.x);
			vertices.push_back(v.position.y);
			vertices.push_back(v.position.z);
		}

		for(const auto& i : m->indices){
			indices.push_back(i);
		}
	}

	const auto numFaces = indices.size() / 3;

	auto polygonFaces = new rp3d::PolygonVertexArray::PolygonFace[numFaces];
	auto face = polygonFaces;
	for(unsigned int f = 0; f < numFaces; f++){
		face->indexBase = f * 3;
		face->nbVertices = 3;
	}

	auto polygonVertexArray = new rp3d::PolygonVertexArray(static_cast<rp3d::uint>(vertices.size()), vertices.data(), static_cast<int>(3 * sizeof(float)),
		indices.data(), static_cast<int>(sizeof(int)), static_cast<rp3d::uint>(numFaces), polygonFaces,
		rp3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
		rp3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

	auto polyhedronMesh = new rp3d::PolyhedronMesh(polygonVertexArray);
	shape = new rp3d::ConvexMeshShape(polyhedronMesh);

	ResourceManager::UnloadResource(modelName_);
}

ConvexCollider::~ConvexCollider(){
	if(shape != nullptr){
		delete shape;
		shape = nullptr;
	}
}

ConcaveCollider::ConcaveCollider(const std::string& modelName_) : BaseCollider(Shape::Concave), vertices(), indices(), triangleArray(nullptr), mesh(nullptr), shape(nullptr){
	_ASSERT(!modelName_.empty());
	Model* model = ResourceManager::LoadResource<Model>(modelName_);
	if(model == nullptr){
		throw std::exception("Model load failed!");
	}

	for(const auto& m : model->meshList){
		for(const auto& v : m->vertices){
			vertices.push_back(v.position.x);
			vertices.push_back(v.position.y);
			vertices.push_back(v.position.z);
		}

		for(const auto& i : m->indices){
			indices.push_back(i);
		}
	}

	const auto numFaces = indices.size() / 3;

	triangleArray =
		new rp3d::TriangleVertexArray(static_cast<rp3d::uint>(vertices.size()), vertices.data(), static_cast<int>(3 * sizeof(float)),
			static_cast<rp3d::uint>(numFaces), indices.data(), static_cast<rp3d::uint>(3 * sizeof(int)),
			rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
			rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

	mesh = new rp3d::TriangleMesh();
	// Add the triangle vertex array to the triangle mesh
	mesh->addSubpart(triangleArray);
	// Create the concave mesh shape
	shape = new rp3d::ConcaveMeshShape(mesh);

	ResourceManager::UnloadResource(modelName_);
}

ConcaveCollider::~ConcaveCollider(){
	if(shape != nullptr){
		//delete shape;
		shape = nullptr;
	}

	if(mesh != nullptr){
		delete mesh;
		mesh = nullptr;
	}

	if(triangleArray != nullptr){
		delete triangleArray;
		triangleArray = nullptr;
	}
}
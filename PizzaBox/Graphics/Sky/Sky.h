#ifndef SKY_H
#define SKY_H

#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include "Graphics/LowLevel/Uniform.h"
#include "Graphics/LowLevel/VAO.h"
#include "Graphics/LowLevel/Buffer.h"

namespace PizzaBox{
	class Sky{
	public:
		Sky(std::string shaderName_, float size_, float rotateSpeed_) : shaderName(shaderName_), shader(nullptr), size(size_), rotation(0.0f), rotateSpeed(rotateSpeed_), 
			projectionMatrixID(nullptr), viewMatrixID(nullptr), scaleMatrixID(nullptr), fogColorID(nullptr), limitsID(nullptr), useFogID(nullptr), vao(), vbo(GL_ARRAY_BUFFER), useFog(false), limits(Vector2()){}
		virtual ~Sky(){}

		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;

		virtual void Render(Camera* camera_) = 0;

		inline void EnableRotation(bool rotates_, float speed_ = 1.0f){
			if(rotates_ == false){
				rotateSpeed = 0.0f;
			}else{
				rotateSpeed = speed_;
			}
		}

		inline void SetSize(float size_) { size = size_; }
		inline void SetUseFog(bool useFog_) { useFog = useFog_; }
		inline void SetLimits(float upper, float lower) { limits = Vector2(upper, lower); }

		virtual GLuint GetSkyboxTexture() = 0;

	protected:
		std::string shaderName;
		Shader* shader;
		float size; 
		float rotation, rotateSpeed;
		bool useFog;
		Vector2 limits;

		VAO vao;
		Buffer vbo;
		Uniform* fogColorID;
		Uniform* limitsID;
		Uniform* useFogID;
		Uniform* projectionMatrixID;
		Uniform* viewMatrixID;
		Uniform* scaleMatrixID;

		float skyboxVertices[108] = {
			// Positions          
			-1.0f,  1.0f, -1.0f,					//Vertex 1
			-1.0f, -1.0f, -1.0f,					//Vertex 2
			1.0f, -1.0f, -1.0f,						//Vertex 3
			1.0f, -1.0f, -1.0f,						//Vertex 3
			1.0f,  1.0f, -1.0f,						//Vertex 4
			-1.0f,  1.0f, -1.0f,					//Vertex 1

			-1.0f, -1.0f,  1.0f,					//Vertex 5
			-1.0f, -1.0f, -1.0f,					//Vertex 2
			-1.0f,  1.0f, -1.0f,					//Vertex 1
			-1.0f,  1.0f, -1.0f,					//Vertex 1
			-1.0f,  1.0f,  1.0f,					//Vertex 6
			-1.0f, -1.0f,  1.0f,					//Vertex 5

			1.0f, -1.0f, -1.0f,						//Vertex 3
			1.0f, -1.0f,  1.0f,						//Vertex 7
			1.0f,  1.0f,  1.0f,						//Vertex 8
			1.0f,  1.0f,  1.0f,						//Vertex 8
			1.0f,  1.0f, -1.0f,						//Vertex 4
			1.0f, -1.0f, -1.0f,						//Vertex 3

			-1.0f, -1.0f,  1.0f,					//Vertex 5
			-1.0f,  1.0f,  1.0f,					//Vertex 6
			1.0f,  1.0f,  1.0f,						//Vertex 8
			1.0f,  1.0f,  1.0f,						//Vertex 8
			1.0f, -1.0f,  1.0f,						//Vertex 7
			-1.0f, -1.0f,  1.0f,					//Vertex 5

			-1.0f,  1.0f, -1.0f,					//Vertex 1
			1.0f,  1.0f, -1.0f,						//Vertex 4
			1.0f,  1.0f,  1.0f,						//Vertex 8
			1.0f,  1.0f,  1.0f,						//Vertex 8
			-1.0f,  1.0f,  1.0f,					//Vertex 6
			-1.0f,  1.0f, -1.0f,					//Vertex 1

			-1.0f, -1.0f, -1.0f,					//Vertex 2
			-1.0f, -1.0f,  1.0f,					//Vertex 5
			1.0f, -1.0f, -1.0f,						//Vertex 3
			1.0f, -1.0f, -1.0f,						//Vertex 3
			-1.0f, -1.0f,  1.0f,					//Vertex 5
			1.0f, -1.0f,  1.0f					    //Vertex 7
		};
	};
}

#endif //!SKY_H
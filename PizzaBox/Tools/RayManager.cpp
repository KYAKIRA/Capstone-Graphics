#include "RayManager.h"

#include "Resource/ResourceManager.h"

using namespace PizzaBox;

RayManager::RayManager() : vao(), vbo(GL_ARRAY_BUFFER), rayShaderName("RayShader"), rays(){
	projectionMatrixID = nullptr;
	viewMatrixID = nullptr;
	colorID = nullptr;
	rayShader = nullptr;
}

RayManager::~RayManager(){
	#ifdef _DEBUG
	if(projectionMatrixID != nullptr || viewMatrixID != nullptr || colorID != nullptr || rayShader != nullptr || rays.size() > 0){
		Debug::LogWarning("Memory leak detected in RayManager!", __FILE__, __LINE__);
		Destroy();
	}
	#endif // _DEBUG
}

bool RayManager::Initialize(){
	rayShader = ResourceManager::LoadResource<Shader>(rayShaderName);
	if(rayShader == nullptr){
		Debug::LogError(rayShaderName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	projectionMatrixID = new Uniform(rayShader, "projectionMatrix");
	viewMatrixID = new Uniform(rayShader, "viewMatrix");
	colorID = new Uniform(rayShader, "color");
	
	rays.insert(std::make_pair<RayMode, std::vector<RayCastRender>>(RayMode::Permanent, std::vector<RayCastRender>()));
	rays.insert(std::make_pair<RayMode, std::vector<RayCastRender>>(RayMode::Repeat, std::vector<RayCastRender>()));

	return true;
}

void RayManager::Destroy(){
	if(projectionMatrixID != nullptr){
		delete projectionMatrixID;
		projectionMatrixID = nullptr;
	}

	if(viewMatrixID != nullptr){
		delete viewMatrixID;
		viewMatrixID = nullptr;
	}

	if(colorID != nullptr){
		delete colorID;
		colorID = nullptr;
	}

	if(rayShader != nullptr){
		ResourceManager::UnloadResource(rayShaderName);
		rayShader = nullptr;
	}

	rays.clear();
}

void RayManager::Render(Camera* cam_){
	for(auto it = rays.begin(); it != rays.end(); it++){
		for(RayCastRender ray : it->second){
			rayShader->Use();
			rayShader->BindMatrix4(projectionMatrixID->id, cam_->GetProjectionMatrix());
			rayShader->BindMatrix4(viewMatrixID->id, cam_->GetViewMatrix());
			rayShader->BindColor(colorID->id, ray.color);

			vao.Bind();
			vbo.Bind();

			vbo.SetBufferData(2 * sizeof(Vector3), &ray.vertices[0], GL_STREAM_DRAW);
			vao.SetupVertexAttribute(0, 3, sizeof(Vector3), (GLvoid*)(0));

			vbo.Unbind();
			vao.Unbind();

			vao.Bind();
			glDrawArrays(GL_LINES, 0, 2);
			vao.Unbind();
		}
	}

	rays[RayMode::Repeat].clear();
}

void RayManager::AddRay(RayCastRender ray_, RayMode mode_){
	if(mode_ == RayMode::Repeat){
		rays[RayMode::Repeat].push_back(ray_);
	}else if(mode_ == RayMode::Permanent){
		rays[RayMode::Permanent].push_back(ray_);
	}
}

void RayManager::ClearPermanent(){
	rays[RayMode::Permanent].clear();
	rays[RayMode::Permanent].shrink_to_fit();
}
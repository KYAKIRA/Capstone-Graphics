#include "Shadows.h"

#include "Math/Math.h"
#include "Resource/ResourceManager.h"

using namespace PizzaBox;

Shadows::Shadows(const std::string& depthShaderName_, unsigned int resolution_) : shadowResolution(resolution_), box(nullptr), animDepthShader(nullptr), depthShader(nullptr), animDepthShaderName("AnimDepthShader"), depthShaderName(depthShaderName_){
} 

Shadows::~Shadows(){
}

bool Shadows::Initialize(){
	depthShader = ResourceManager::LoadResource<Shader>(depthShaderName);
	if(depthShader == nullptr){
		Debug::LogError("Could not load " + depthShaderName + "!", __FILE__, __LINE__);
		return false;
	}

	animDepthShader = ResourceManager::LoadResource<Shader>(animDepthShaderName);
	if(animDepthShader == nullptr){
		Debug::LogError("Could not load " + animDepthShaderName + "!", __FILE__, __LINE__);
		return false;
	}

	box = new ShadowBox(100.0f);
	
	return true;
}

void Shadows::Destroy(){
	if(depthShader != nullptr){
		ResourceManager::UnloadResource(depthShaderName);
		depthShader = nullptr;
	}

	if(animDepthShader != nullptr){
		ResourceManager::UnloadResource(animDepthShaderName);
		animDepthShader = nullptr;
	}
}

void Shadows::Render(const std::vector<Camera*>& cams_, const std::vector<MeshRender*>& mrs_, const std::vector<AnimMeshRender*>& amrs_, const std::vector<DirectionalLight*>& dirs_, const std::vector<SpotLight*>& spots_){
	if(dirs_.size() <= 0){
		return;
	}

	ReserveFBOs(cams_.size() * (dirs_.size() + spots_.size())); //Make sure we have enough FBOs for every light

	glEnable(GL_DEPTH_TEST);

	auto fboIter = depthFBOs.begin();
	ShadowFBO* depthFBO = *fboIter;

	for(Camera* cam : cams_){
		for(DirectionalLight* dir : dirs_){
			depthFBO = *fboIter;

			//Render Scene for depth map
			depthFBO->Bind();
			glClear(GL_DEPTH_BUFFER_BIT);

			if(!dir->CastsShadows()){
				dir->SetLightViewMatrix(Matrix4::Identity());
				dir->SetLightSpaceMatrix(Matrix4::Identity());
				dir->SetDepthMap(depthFBO->GetDepthMap());

				depthFBO->Unbind();
				fboIter++;
				continue;
			}

			box->SetLightViewMatrix(dir->GetLightViewMatrix());
			box->UpdateBox(cam);

			//Update projection
			Matrix4 lightProjection = Matrix4::Identity();
			lightProjection[0] = 2.0f / box->GetWidth();
			lightProjection[5] = 2.0f / box->GetHeight();
			lightProjection[10] = -2.0f / box->GetLength();
			lightProjection[15] = 1.0f;

			//Update Light View
			Matrix4 lightView = Matrix4::Identity();
			lightView *= dir->GetGameObject()->GlobalRotationQuat().ToMatrix4().Inverse();
			lightView *= Matrix4::Translate(box->GetCenter()).Inverse();

			Matrix4 lightSpaceMatrix = lightProjection * lightView;

			glViewport(0, 0, shadowResolution, shadowResolution);
			depthShader->Use();
			for(MeshRender* mr : mrs_){
				if(!mr->CastsShadows()){
					continue;
				}

				depthShader->BindMatrix4("lightSpaceMatrix", lightSpaceMatrix);
				for(Mesh* mesh : mr->GetModel()->meshList){
					depthShader->BindMatrix4("model", mr->GetGameObject()->GetTransform()->GetTransformation());
					mesh->Render();
				}
			}

			animDepthShader->Use();
			for(AnimMeshRender* ar : amrs_){
				if(!ar->CastsShadows()){
					continue;
				}

				if(ar->GetAnimator() == nullptr){
					depthShader->Use();
					depthShader->BindMatrix4("lightSpaceMatrix", lightSpaceMatrix);
					for(AnimMesh* mesh : ar->GetAnimModel()->meshList){
						depthShader->BindMatrix4("model", ar->GetGameObject()->GetTransform()->GetTransformation());
						mesh->Render();
					}

					animDepthShader->Use();
				}else{
					animDepthShader->BindMatrix4("lightSpaceMatrix", lightSpaceMatrix);
					ar->BindSkeletonInstance(animDepthShader);

					for(AnimMesh* mesh : ar->GetAnimModel()->meshList){
						animDepthShader->BindMatrix4("model", ar->GetGameObject()->GetTransform()->GetTransformation());
						mesh->Render();
					}
				}
			}

			dir->SetLightViewMatrix(lightView);
			dir->SetLightSpaceMatrix(lightSpaceMatrix);
			dir->SetDepthMap(depthFBO->GetDepthMap());

			depthFBO->Unbind();
			fboIter++;
		}

		for(SpotLight* spot : spots_){
			depthFBO = *fboIter;

			//Render Scene for depth map
			depthFBO->Bind();
			glClear(GL_DEPTH_BUFFER_BIT);

			if(!spot->CastsShadows()){
				spot->SetLightViewMatrix(Matrix4::Identity());
				spot->SetLightSpaceMatrix(Matrix4::Identity());
				spot->SetDepthMap(depthFBO->GetDepthMap());

				depthFBO->Unbind();
				fboIter++;
				continue;
			}

			//Update projection
			Matrix4 lightProjection = Matrix4::Perspective(90.0f, 1.0f, 1.0f, 100.0f);

			//Update Light View
			Matrix4 lightView = Matrix4::Identity();
			lightView *= spot->GetGameObject()->GlobalRotationQuat().ToMatrix4().Inverse();
			lightView *= Matrix4::Translate(spot->GetGameObject()->GlobalPosition()).Inverse();

			Matrix4 lightSpaceMatrix = lightProjection * lightView;

			glViewport(0, 0, shadowResolution, shadowResolution);
			depthShader->Use();
			for(MeshRender* mr : mrs_){
				if(!mr->CastsShadows()){
					continue;
				}

				depthShader->BindMatrix4("lightSpaceMatrix", lightSpaceMatrix);
				for(Mesh* mesh : mr->GetModel()->meshList){
					depthShader->BindMatrix4("model", mr->GetGameObject()->GetTransform()->GetTransformation());
					mesh->Render();
				}
			}

			animDepthShader->Use();
			for(AnimMeshRender* ar : amrs_){
				if(!ar->CastsShadows()){
					continue;
				}

				animDepthShader->BindMatrix4("lightSpaceMatrix", lightSpaceMatrix);
				ar->BindSkeletonInstance(animDepthShader);

				for(AnimMesh* mesh : ar->GetAnimModel()->meshList){
					animDepthShader->BindMatrix4("model", ar->GetGameObject()->GetTransform()->GetTransformation());
					mesh->Render();
				}
			}

			spot->SetLightViewMatrix(lightView);
			spot->SetLightSpaceMatrix(lightSpaceMatrix);
			spot->SetDepthMap(depthFBO->GetDepthMap());

			depthFBO->Unbind();
			fboIter++;
		}
	}
}

void Shadows::ReserveFBOs(size_t numFBOs_){
	if(numFBOs_ <= depthFBOs.size()){
		return; //We already have enough
	}

	while(depthFBOs.size() < numFBOs_){
		ShadowFBO* df = new ShadowFBO(ScreenCoordinate(shadowResolution, shadowResolution));
		if(df->Initialize() == false){
			Debug::LogError("Shadow Fbo could not be initialized!", __FILE__, __LINE__);
			throw std::exception("Shadow FBO could not be initialized!");
		}

		depthFBOs.push_back(df);
	}
}
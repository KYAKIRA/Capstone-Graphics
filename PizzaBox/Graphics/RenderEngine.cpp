#include "RenderEngine.h"

#include <algorithm>

#include <rttr/registration.h>

#include "Camera.h"
#include "Effects/PostProcessing.h"
#include "Models/MeshRender.h"
#include "Sky/SkyBox.h"
#include "Text/TextRender.h"
#include "UI/UIManager.h"
#include "Core/Config.h"
#include "Core/GameManager.h"
#include "Core/SceneManager.h"
#include "Resource/ResourceManager.h"
#include "Core/FileSystem.h"

using namespace PizzaBox;

//Initialize static variables here
Window* RenderEngine::window = nullptr;
MultisampleFBO* RenderEngine::multisampleFBO = nullptr;
MainFBO* RenderEngine::postProcessFBO = nullptr;
Color RenderEngine::baseAmbient = Color(0.1f, 0.1f, 0.1f);
Color RenderEngine::fogColor = Color(0.5444f, 0.62f, 0.69f);
float RenderEngine::fogDensity = 0.0f;
float RenderEngine::fogGradient = 5.0f;
float RenderEngine::waterFogDensity = 0.0f;
float RenderEngine::waterFogGradient = 5.0f;
Shadows* RenderEngine::shadowHandler = nullptr;
bool RenderEngine::isShowingCursor = false;
std::vector<Camera*> RenderEngine::cams;
std::vector<MeshRender*> RenderEngine::mrs;
std::vector<TextRender*> RenderEngine::trs;
std::vector<DirectionalLight*> RenderEngine::dirs;
std::vector<PointLight*> RenderEngine::points;
std::vector<SpotLight*> RenderEngine::spots;
std::vector<ParticleSystem*> RenderEngine::pSystems;
std::vector<AnimMeshRender*> RenderEngine::animMeshRenders;
std::string RenderEngine::sharedShaderName = "Resources/Shaders/_shared.glsl";
std::string RenderEngine::sharedShaderCode = "";

//Suppress meaningless and unavoidable warning
#pragma warning( push )
#pragma warning( disable : 26444 )
RTTR_REGISTRATION{
	rttr::registration::class_<RenderEngine>("RenderEngine")
		.method("Initialize", &RenderEngine::Initialize)
		.method("Destroy", &RenderEngine::Destroy)
		.method("Render", &RenderEngine::Render)
		.method("ScreenSize", &RenderEngine::ScreenSize)
		.method("GetWindowFullscreen", &RenderEngine::GetWindowFullscreen)
		.method("GetWindowBorderless", &RenderEngine::GetWindowBorderless)
		.method("GetVSYNC", &RenderEngine::GetVSYNC)
		.method("IsShowingCursor", &RenderEngine::IsShowingCursor)
		.method("GetSharedShaderCode", &RenderEngine::GetSharedShaderCode)
		.method("OnResize", &RenderEngine::OnResize)
		.method("SetWindowFullscreen", &RenderEngine::SetWindowFullscreen)
		.method("SetWindowResolution", static_cast<void(*)(const ScreenCoordinate&)>(&RenderEngine::SetWindowResolution))
		.method("SetWindowResolution", static_cast<void(*)(unsigned int, unsigned int)>(&RenderEngine::SetWindowResolution))
		.method("SetWindowBorderless", &RenderEngine::SetWindowBorderless)
		.method("SetVSYNC", &RenderEngine::SetVSYNC)
		.method("ShowCursor", &RenderEngine::ShowCursor)
		.method("RegisterCamera", &RenderEngine::RegisterCamera)
		.method("RegisterMeshRender", &RenderEngine::RegisterMeshRender)
		.method("RegisterTextRender", &RenderEngine::RegisterTextRender)
		.method("RegisterDirectionalLight", &RenderEngine::RegisterDirectionalLight)
		.method("RegisterPointLight", &RenderEngine::RegisterPointLight)
		.method("RegisterSpotLight", &RenderEngine::RegisterSpotLight)
		.method("RegisterParticleSystem", &RenderEngine::RegisterParticleSystem)
		.method("RegisterAnimMeshRender", &RenderEngine::RegisterAnimMeshRender)
		.method("UnregisterCamera", &RenderEngine::UnregisterCamera)
		.method("UnregisterMeshRender", &RenderEngine::UnregisterMeshRender)
		.method("UnregisterTextRender", &RenderEngine::UnregisterTextRender)
		.method("UnregisterDirectionalLight", &RenderEngine::UnregisterDirectionalLight)
		.method("UnregisterPointLight", &RenderEngine::UnregisterPointLight)
		.method("UnregisterSpotLight", &RenderEngine::UnregisterSpotLight)
		.method("UnregisterParticleSystem", &RenderEngine::UnregisterParticleSystem)
		.method("UnregisterAnimMeshRender", &RenderEngine::UnregisterAnimMeshRender)
		.method("GetFogColor", &RenderEngine::GetFogColor)
		.method("GetFogDensity", &RenderEngine::GetFogDensity)
		.method("GetFogGradient", &RenderEngine::GetFogGradient)
		.method("GetWaterFogDensity", &RenderEngine::GetWaterFogDensity)
		.method("GetWaterFogGradient", &RenderEngine::GetWaterFogGradient)
		.method("SetFogColor", &RenderEngine::SetFogColor)
		.method("SetFogDensity", &RenderEngine::SetFogDensity)
		.method("SetFogGradient", &RenderEngine::SetFogGradient)
		.method("SetWaterFogDensity", &RenderEngine::SetWaterFogDensity)
		.method("SetWaterFogGradient", &RenderEngine::SetWaterFogGradient);
}
#pragma warning( pop )

bool RenderEngine::Initialize(){
	//Load shared source code for all shaders
	if(!FileSystem::FileExists(sharedShaderName)){
		Debug::LogError("Shared shader file does not exist!", __FILE__, __LINE__);
		return false;
	}

	sharedShaderCode = FileSystem::ReadFileToString(sharedShaderName);
	if(sharedShaderCode.empty()){
		Debug::LogError("Could not load shared shader source!", __FILE__, __LINE__);
		return false;
	}

	//Create and initialize a Window
	ScreenCoordinate screenSize = ScreenCoordinate(Config::GetInt("WindowX"), Config::GetInt("WindowY"));
	window = new Window(GameManager::Name(), screenSize, Config::GetBool("Fullscreen"), Config::GetBool("Borderless"));
	if(window->Initialize() == false){
		Debug::LogError("Window could not be initialized!", __FILE__, __LINE__);
		return false;
	}

	//Print out the graphics card being used
	std::string graphicsCard = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	Debug::Log("Active Graphic Card: " + graphicsCard);

	#ifdef _DEBUG
	if(glDebugMessageCallback){
		//Setup our debug callback if debug messages are available
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback((GLDEBUGPROC)Debug::GLDebugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
		Debug::Log("Registered OpenGL Debug Callback function", __FILE__, __LINE__);
	}else{
		Debug::LogWarning("Could not setup OpenGL Debug Callback");
	}
	#endif //_DEBUG

	//Set OpenGL stuff
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	multisampleFBO = new MultisampleFBO(screenSize);
	if(multisampleFBO->Initialize() == false){
		Debug::LogError("Multisample FBO could not be initialized!", __FILE__, __LINE__);
		return false;
	}

	postProcessFBO = new MainFBO(screenSize);
	if(postProcessFBO->Initialize() == false){
		Debug::LogError("FBO could not be initialized!", __FILE__, __LINE__);
		return false;
	}

	if(PostProcessing::Initialize("ScreenShader") == false){
		Debug::LogError("PostProcessing could not be initialized!", __FILE__, __LINE__);
		return false;
	}

	shadowHandler = new Shadows("DepthShader", 8192);
	if (shadowHandler->Initialize() == false) {
		Debug::LogError("Shadows could not be initialized!", __FILE__, __LINE__);
		return false;
	}

	SetClearColor(Color::Black);

	//Set the clear color to dark gray if we're in the Debug configuration
	#ifdef _DEBUG
	SetClearColor(Color::DarkGray);

	Debug::InitializeRays();
	#endif //_DEBUG

	if(UIManager::Initialize() == false){
		Debug::LogError("UI Manager could not be initialized!");
		return false;
	}

	//Clear and update the screen to that's it's filled in with something
	ClearScreen();
	window->UpdateWindow();

	return true;
}

void RenderEngine::Destroy(){
	UIManager::Destroy();

	//I'm not sure if or how many of these disables are necessary
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_CLAMP);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_MULTISAMPLE);

	#ifdef _DEBUG
	Debug::DestroyRays();
	#endif //_DEBUG

	if(shadowHandler != nullptr){
		shadowHandler->Destroy();
		delete shadowHandler;
		shadowHandler = nullptr;
	}

	PostProcessing::Destroy();

	if(postProcessFBO != nullptr){
		postProcessFBO->Destroy();
		delete postProcessFBO;
		postProcessFBO = nullptr;
	}

	if(multisampleFBO != nullptr){
		multisampleFBO->Destroy();
		delete multisampleFBO;
		multisampleFBO = nullptr;
	}

	if(window != nullptr){
		window->Destroy();
		delete window;
		window = nullptr;
	}
}

void RenderEngine::Render(){  
	//Get Sky component
	Sky* sky = SceneManager::CurrentScene()->GetSky();

	//Remove any disabled components since we don't need to do anything with them
	auto cameras = cams;
	cameras.erase(std::remove_if(cameras.begin(), cameras.end(), [](const Camera* c){ return !c->GetEnable(); }), cameras.end());
	auto mrList = mrs;
	mrList.erase(std::remove_if(mrList.begin(), mrList.end(), [](const MeshRender* mr){ return !mr->GetEnable(); }), mrList.end());
	auto trList = trs;
	trList.erase(std::remove_if(trList.begin(), trList.end(), [](const TextRender* tr){ return !tr->GetEnable(); }), trList.end());
	auto dirList = dirs;
	dirList.erase(std::remove_if(dirList.begin(), dirList.end(), [](const DirectionalLight* dl) { return !dl->GetEnable(); }), dirList.end());
	auto pointList = points;
	pointList.erase(std::remove_if(pointList.begin(), pointList.end(), [](const PointLight* pl) { return !pl->GetEnable(); }), pointList.end());
	auto spotList = spots;
	spotList.erase(std::remove_if(spotList.begin(), spotList.end(), [](const SpotLight* sl) { return !sl->GetEnable(); }), spotList.end());
	auto particleSystemList = pSystems;
	particleSystemList.erase(std::remove_if(particleSystemList.begin(), particleSystemList.end(), [](const ParticleSystem* ps) { return !ps->GetEnable(); }), particleSystemList.end());
	auto amrList = animMeshRenders;
	amrList.erase(std::remove_if(amrList.begin(), amrList.end(), [](const AnimMeshRender* amr){ return !amr->GetEnable(); }), amrList.end());

	shadowHandler->Render(cameras, mrList, amrList, dirList, spotList);

	multisampleFBO->Bind();
	ClearScreen();
	for(Camera* cam : cameras){
		cam->CalculateViewMatrix();

		SetViewport(cam->GetViewportRect());
		//Render the sky background if there is one
		if(sky != nullptr){
			sky->Render(cam);
		}

		//Loop through all MeshRender's in the scene and render them (if they're enabled)
		for(MeshRender* mr : mrList){
			mr->Render(cam, dirList, pointList, spotList);
		}

		//Do the same for all AnimMeshRenders
		for(AnimMeshRender* amr : amrList){
			amr->Render(cam, dirList, pointList, spotList);
		}

		//Loop through all ParticleSystem's in the scene and render them (if they're enabled)
		for(ParticleSystem* ps : particleSystemList){
			ps->Update();
			ps->Render(cam);
		}
		
		//Render raycast if we're in the Debug configuration
		#ifdef _DEBUG
		Debug::DrawRayCast(cam);
		#endif //_DEBUG
	}
	multisampleFBO->ResolveToFBO(postProcessFBO);

	glClear(GL_COLOR_BUFFER_BIT);
	
	postProcessFBO->Bind();
	for(Camera* cam : cameras){
		SetViewport(cam->GetViewportRect());
		
		//Clear the depth buffer before drawing text
		//This guarantees that text will always be visible no matter how close anything else is to the camera
		//glClear(GL_DEPTH_BUFFER_BIT);
		//Loop through all TextRender components in the scene and render them if they're enabled
		for(TextRender* tr : trList){
			tr->Render(cam);
		} 
	}

	//Reset the viewport to fullscreen before rendering UI
	SetViewport(ViewportRect::fullScreen);
	UIManager::Update();
	UIManager::Render();

	postProcessFBO->Unbind();
	
	//PostProcessing::DoPostProcessing(info.depthMap); 
	PostProcessing::DoPostProcessing(postProcessFBO->GetColorTexture());
	  
	//print out the id of the frame buffer being used
		/*GLint result;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
		std::cout << "FrameBuffer being used: " << result << std::endl;*/

	//Update the window to show the new rendered frame
	window->UpdateWindow();
}

ScreenCoordinate RenderEngine::ScreenSize(){
	return window->GetSize();
}

bool RenderEngine::GetWindowFullscreen(){
	return window->IsFullscreen();
}

bool RenderEngine::GetWindowBorderless(){
	return window->IsBorderless();
}

Window::VSYNC RenderEngine::GetVSYNC(){
	return window->GetVSYNC();
}

void RenderEngine::OnResize(int w_, int h_){
	window->SetSize(w_, h_);

	if(multisampleFBO != nullptr){
		multisampleFBO->Destroy();
		delete multisampleFBO;
		multisampleFBO = nullptr;
	}

	multisampleFBO = new MultisampleFBO(window->GetSize());
	if(multisampleFBO->Initialize() == false){
		Debug::LogError("Multi sampling FBO could not be initialized!", __FILE__, __LINE__);
	}

	if(postProcessFBO != nullptr){
		postProcessFBO->Destroy();
		delete postProcessFBO;
		postProcessFBO = nullptr;
	}

	postProcessFBO = new MainFBO(window->GetSize());
	if(postProcessFBO->Initialize() == false){
		Debug::LogError("FBO could not be initialized!", __FILE__, __LINE__);
	}

	for(Camera* cam : SceneManager::CurrentScene()->GetComponentsInScene<Camera>()){
		cam->Reset();
	}

	PostProcessing::OnResize();

	Config::SetInt("WindowX", window->GetWidth());
	Config::SetInt("WindowY", window->GetHeight());

	ClearScreen();
	window->UpdateWindow();
}

void RenderEngine::SetWindowFullscreen(bool fullscreen_){
	window->SetFullscreen(fullscreen_);
	Config::SetBool("Fullscreen", fullscreen_);
}

void RenderEngine::SetWindowResolution(const ScreenCoordinate& sc_){
	//Doing this will cause OnResize to happen so we don't need to do anything else here
	SDL_SetWindowSize(window->GetWindow(), sc_.x, sc_.y);
}

void RenderEngine::SetWindowResolution(unsigned int w_, unsigned int h_){
	SetWindowResolution(ScreenCoordinate(w_, h_));
}

void RenderEngine::SetWindowBorderless(bool borderless_){
	window->SetBorderless(borderless_);
	Config::SetBool("Borderless", borderless_);
}

void RenderEngine::SetVSYNC(Window::VSYNC vsync_){
	window->SetVSYNC(vsync_);
}

void RenderEngine::ShowCursor(bool show_){
	int result;
	if(show_){
		result = SDL_SetRelativeMouseMode(SDL_FALSE);
	}else{
		result = SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	if(result != 0){
		Debug::LogError("Could not change cursor display mode! SDL Error: " + std::string(SDL_GetError()));
		return;
	}

	isShowingCursor = show_;
}

void RenderEngine::RegisterCamera(Camera* cam_){
	_ASSERT(cam_ != nullptr);
	cams.push_back(cam_);
}

void RenderEngine::RegisterMeshRender(MeshRender* mr_){
	_ASSERT(mr_ != nullptr);
	mrs.push_back(mr_);
}

void RenderEngine::RegisterTextRender(TextRender* tr_){
	_ASSERT(tr_ != nullptr);
	trs.push_back(tr_);
}

void RenderEngine::RegisterDirectionalLight(DirectionalLight* dir_){
	_ASSERT(dir_ != nullptr);
	dirs.push_back(dir_);
}

void RenderEngine::RegisterPointLight(PointLight* point_){
	_ASSERT(point_ != nullptr);
	points.push_back(point_);
}

void RenderEngine::RegisterSpotLight(SpotLight* spot_){
	_ASSERT(spot_ != nullptr);
	spots.push_back(spot_);
}

void RenderEngine::RegisterParticleSystem(ParticleSystem* particles_){
	_ASSERT(particles_ != nullptr);
	pSystems.push_back(particles_);
}

void RenderEngine::RegisterAnimMeshRender(AnimMeshRender* amr_){
	_ASSERT(amr_ != nullptr);
	animMeshRenders.push_back(amr_);
}

void RenderEngine::UnregisterCamera(Camera* cam_){
	_ASSERT(cam_ != nullptr);
	cams.erase(std::remove(cams.begin(), cams.end(), cam_), cams.end());
}

void RenderEngine::UnregisterMeshRender(MeshRender* mr_){
	_ASSERT(mr_ != nullptr);
	mrs.erase(std::remove(mrs.begin(), mrs.end(), mr_), mrs.end());
}

void RenderEngine::UnregisterTextRender(TextRender* tr_){
	_ASSERT(tr_ != nullptr);
	trs.erase(std::remove(trs.begin(), trs.end(), tr_), trs.end());
}

void RenderEngine::UnregisterDirectionalLight(DirectionalLight* dir_){
	_ASSERT(dir_ != nullptr);
	dirs.erase(std::remove(dirs.begin(), dirs.end(), dir_), dirs.end());
}

void RenderEngine::UnregisterPointLight(PointLight* point_){
	_ASSERT(point_ != nullptr);
	points.erase(std::remove(points.begin(), points.end(), point_), points.end());
}

void RenderEngine::UnregisterSpotLight(SpotLight* spot_){
	_ASSERT(spot_ != nullptr);
	spots.erase(std::remove(spots.begin(), spots.end(), spot_), spots.end());
}

void RenderEngine::UnregisterParticleSystem(ParticleSystem* particles_){
	_ASSERT(particles_ != nullptr);
	pSystems.erase(std::remove(pSystems.begin(), pSystems.end(), particles_), pSystems.end());
}

void RenderEngine::UnregisterAnimMeshRender(AnimMeshRender* amr_){
	_ASSERT(amr_ != nullptr);
	animMeshRenders.erase(std::remove(animMeshRenders.begin(), animMeshRenders.end(), amr_), animMeshRenders.end());
}

void RenderEngine::ClearScreen(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderEngine::SetClearColor(const Color& color_){
	glClearColor(color_.r, color_.g, color_.b, color_.a);
}

void RenderEngine::SetViewport(const ViewportRect& v_){
	glViewport(static_cast<GLint>(window->GetWidth() * v_.x),
				static_cast<GLint>(window->GetHeight() * v_.y),
				static_cast<GLsizei>(window->GetWidth() * v_.width),
				static_cast<GLsizei>(window->GetHeight() * v_.height));
}

Color RenderEngine::GetFogColor(){
	return fogColor;
}

float RenderEngine::GetFogDensity(){
	return fogDensity;
}

float RenderEngine::GetFogGradient(){
	return fogGradient;
}

float RenderEngine::GetWaterFogDensity(){
	return waterFogDensity;
}

float RenderEngine::GetWaterFogGradient(){
	return waterFogGradient;
}

void RenderEngine::SetFogColor(const Color& fogColor_){
	fogColor = fogColor_;
}

void RenderEngine::SetFogDensity(float density_){
	fogDensity = density_;
}

void RenderEngine::SetFogGradient(float gradient_){
	fogGradient = gradient_;
}

void RenderEngine::SetWaterFogDensity(float waterDensity_){
	waterFogDensity = waterDensity_;
}

void RenderEngine::SetWaterFogGradient(float waterGradient_){
	waterFogGradient = waterGradient_;
}
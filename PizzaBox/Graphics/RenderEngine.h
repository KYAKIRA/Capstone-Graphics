#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include <vector>

#include "Camera.h"
#include "Color.h"
#include "Effects/Shadows.h"
#include "FBO/MainFBO.h"
#include "FBO/MultisampleFBO.h"
#include "Lighting/DirectionalLight.h"
#include "Lighting/PointLight.h"
#include "Lighting/SpotLight.h"
#include "Models/MeshRender.h"
#include "Particles/ParticleSystem.h"
#include "Text/TextRender.h"
#include "Animation/AnimMeshRender.h"
#include "Core/Window.h"

namespace PizzaBox{ 
	class RenderEngine{
	public:
		static bool Initialize();
		static void Destroy();

		static void Render();

		static ScreenCoordinate ScreenSize();
		static bool GetWindowFullscreen();
		static bool GetWindowBorderless();
		static Window::VSYNC GetVSYNC();
		static bool IsShowingCursor(){ return isShowingCursor; }
		inline static std::string GetSharedShaderCode(){ return sharedShaderCode; }

		static void OnResize(int w_, int h_);
		static void SetWindowFullscreen(bool fullscreen_);
		static void SetWindowResolution(const ScreenCoordinate& sc_);
		static void SetWindowResolution(unsigned int w_, unsigned int h_);
		static void SetWindowBorderless(bool borderless_);
		static void SetVSYNC(Window::VSYNC vsync_);
		static void ShowCursor(bool show_);

		static void RegisterCamera(Camera* cam_);
		static void RegisterMeshRender(MeshRender* mr_);
		static void RegisterTextRender(TextRender* tr_);
		static void RegisterDirectionalLight(DirectionalLight* dir_);
		static void RegisterPointLight(PointLight* point_);
		static void RegisterSpotLight(SpotLight* spot_);
		static void RegisterParticleSystem(ParticleSystem* sys_);
		static void RegisterAnimMeshRender(AnimMeshRender* amr_);

		static void UnregisterCamera(Camera* cam_);
		static void UnregisterMeshRender(MeshRender* mr_);
		static void UnregisterTextRender(TextRender* tr_);
		static void UnregisterDirectionalLight(DirectionalLight* dir_);
		static void UnregisterPointLight(PointLight* point_);
		static void UnregisterSpotLight(SpotLight* spot_);
		static void UnregisterParticleSystem(ParticleSystem* sys_);
		static void UnregisterAnimMeshRender(AnimMeshRender* amr_);

		static Color baseAmbient;
		
		//Fog
		static Color GetFogColor();
		static float GetFogDensity();
		static float GetFogGradient();
		static float GetWaterFogDensity();
		static float GetWaterFogGradient();

		static void SetFogColor(const Color& fogColor_);
		static void SetFogDensity(float density_);
		static void SetFogGradient(float gradient_);
		static void SetWaterFogDensity(float waterDensity_);
		static void SetWaterFogGradient(float waterGradient_);

		//Delete unwanted compiler generated constructors, destructors and assignment operators
		RenderEngine() = delete;
		RenderEngine(const RenderEngine&) = delete;
		RenderEngine(RenderEngine&&) = delete;
		RenderEngine& operator=(const RenderEngine&) = delete;
		RenderEngine& operator=(RenderEngine&&) = delete;
		~RenderEngine() = delete;

	private:
		static std::vector<Camera*> cams;
		static std::vector<MeshRender*> mrs;
		static std::vector<TextRender*> trs;
		static std::vector<DirectionalLight*> dirs;
		static std::vector<PointLight*> points;
		static std::vector<SpotLight*> spots;
		static std::vector<ParticleSystem*> pSystems;
		static std::vector<AnimMeshRender*> animMeshRenders;

		static Window* window;
		static MultisampleFBO* multisampleFBO;
		static MainFBO* postProcessFBO;
		static Shadows* shadowHandler;
		static bool isShowingCursor;
		static std::string sharedShaderName;
		static std::string sharedShaderCode;

		static Color fogColor;
		static float fogDensity, waterFogDensity;
		static float fogGradient, waterFogGradient;		

		static void ClearScreen();
		static void SetClearColor(const Color& color_);
		static void SetViewport(const ViewportRect& v_);
	};
}

#endif //!RENDER_ENGINE_H
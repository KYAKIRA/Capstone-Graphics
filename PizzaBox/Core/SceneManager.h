#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <vector>

#include "Scene.h"

namespace PizzaBox{
	class SceneManager{
	public:
		static bool Initialize();
		static void Destroy();

		static bool Update();

		static void AddScene(Scene* newScene_);
		static bool LoadInitialScene();
		static void LoadScene(unsigned int sceneIndex_);
		static void LoadNextScene();
		static void ReloadCurrentScene();
		static Scene* CurrentScene();
		static int CurrentSceneIndex();
		static unsigned int NumScenes();

		static bool TestLoadingAllScenes();

		//Delete unwanted compiler-generated constructors, destructors and assignment operators
		SceneManager() = delete;
		SceneManager(const SceneManager&) = delete;
		SceneManager(SceneManager&&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;
		SceneManager& operator=(SceneManager&&) = delete;
		~SceneManager() = delete;

	private:
		static std::vector<Scene*> sceneList;
		static int currentSceneIndex;
		static std::queue<unsigned int> sceneLoadQueue;

		static bool ChangeToNewScene();
	};
}

#endif //!SCENE_MANAGER_H
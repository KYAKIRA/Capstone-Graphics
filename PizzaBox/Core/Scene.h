#ifndef SCENE_H
#define SCENE_H

#include <algorithm>
#include <queue>

#include "Graphics/Sky/Sky.h"
#include "Object/GameObject.h"

namespace PizzaBox{
	class Scene{
	public:
		Scene();
		virtual ~Scene();

		virtual bool Initialize() = 0;
		virtual void Destroy();

		bool Update();

		template <class T, typename... ARGS> T* CreateObject(const Vector3& pos_ = Vector3(), const Euler& rot_ = Euler(), const Vector3& scale_ = Vector3(1.0f, 1.0f, 1.0f), const ARGS&... args_){
			static_assert(std::is_base_of<GameObject, T>::value, "T must inherit from GameObject");
			return CreateObject<T>(nullptr, pos_, rot_, scale_, args_...);
		}

		template <class T, typename... ARGS> T* CreateObject(GameObject* parent_, const Vector3& pos_ = Vector3(), const Euler& rot_ = Euler(), const Vector3& scale_ = Vector3(1.0f, 1.0f, 1.0f), const ARGS&... args_){
			static_assert(std::is_base_of<GameObject, T>::value, "T must inherit from GameObject");

			T* t = new T(pos_, rot_, scale_, args_...);

			//If this object has a parent, then the parent will handle object construction
			//If not then we have to do it ourselves
			if(parent_ != nullptr){
				parent_->AddChild(t);
			}else{
				createQueue.push(t);
			}

			return t;
		}

		void DestroyObject(GameObject* go_);

		void AttachObject(GameObject* go_);
		void DetachObject(GameObject* go_);

		template <class T> T* GetComponentInScene(){
			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
			T* component = nullptr;

			//Go through every object in the scene
			for(GameObject* go : gameObjectList){
				//If this object has a component of type T, grab it and return it
				component = go->GetComponent<T>();
				if(component != nullptr){
					return component;
				}

				//If any of this object's children have a component of type T, grab it and return it
				component = go->GetComponentInChildren<T>();
				if(component != nullptr){
					return component;
				}
			}

			//If no component is found, return nullptr
			return nullptr;
		}

		template <class T> std::vector<T*> GetComponentsInScene(){
			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
			std::vector<T*> tmp = std::vector<T*>();
			std::vector<T*> componentsInScene = std::vector<T*>();

			//Go through ever GameObject in the scene
			for(GameObject* go : gameObjectList){
				//Grab all components in this object's family and add it to our list
				tmp = go->GetComponentsInFamily<T>();
				for(T* t : tmp){
					componentsInScene.push_back(t);
				}
			}

			return componentsInScene;
		}

		GameObject* FindWithTag(const std::string& tag_);
		std::vector<GameObject*> FindObjectsWithTag(const std::string& tag_);

		Sky* GetSky();
		void SetSky(Sky* sky_);

	private:
		std::vector<GameObject*> gameObjectList;
		Sky *sky;
		std::queue<GameObject*> createQueue;
		std::queue<GameObject*> destroyQueue;
		std::queue<GameObject*> attachQueue;
		std::queue<GameObject*> detachQueue;
		std::queue<Sky*> skyQueue;

		bool MaintainQueues();
	};
}

#endif //!SCENE_H
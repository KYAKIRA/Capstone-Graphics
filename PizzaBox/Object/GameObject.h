#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <queue>
#include <set>
#include <vector>

#include "Component.h"
#include "Transform.h"

namespace PizzaBox{
	class GameObject{
	public:
		GameObject(const Vector3& position_ = PizzaBox::Vector3(), const Euler& rotation_ = PizzaBox::Euler(), const Vector3& scale_ = PizzaBox::Vector3(1.0f, 1.0f, 1.0f));
		virtual ~GameObject();

		bool Initialize();
		void Destroy();

		bool Update();

		void AddChild(GameObject* child_);
		void AddComponent(Component* component_);
		void RemoveChild(GameObject* child_);
		void RemoveAllChildren();
		void AttachChild(GameObject* child_);
		void DetachChild(GameObject* child_);

		Transform* GetTransform();
		const Transform* GetTransform() const;
		GameObject* GetChild(unsigned int index_) const;

		Vector3 GetPosition() const;
		Vector3 GlobalPosition() const;
		Euler GetRotation() const;
		Quaternion GetRotationQuat() const;
		Euler GlobalRotation() const;
		Quaternion GlobalRotationQuat() const;
		Vector3 GetScale() const;
		Vector3 GlobalScale() const;
		bool IsStatic() const;

		inline Vector3 GetForward() const{ return transform.GetForward(); }
		inline Vector3 GetUp() const{ return transform.GetUp(); }
		inline Vector3 GetRight() const{ return transform.GetRight(); }

		void SetPosition(const Vector3& position_);
		void SetPosition(float x_, float y_, float z_);
		void SetRotation(const Euler& rotation_);
		void SetRotation(float x_, float y_, float z_);
		void SetRotation(const Matrix3& rotation_);
		void SetRotation(const Quaternion& rotation_);
		void SetScale(const Vector3& scale_);
		void SetScale(float x_, float y_, float z_);
		void SetScale(float s_);
		void SetStatic(bool static_);

		void SetGlobalPosition(const Vector3& position_);
		void SetGlobalPosition(const float x, const float y, const float z);
		void SetGlobalRotation(const Euler& rotation_);
		void SetGlobalRotation(const float x, const float y, const float z);
		void SetGlobalRotation(const Matrix3& rotation_);
		void SetGlobalRotation(const Quaternion& rotation_);
		void SetGlobalScale(const Vector3& scale_);
		void SetGlobalScale(const float x, const float y, const float z);
		void SetGlobalScale(const float s);

		void Translate(const Vector3& translate_);
		void Translate(float x_, float y_, float z_);
		void Rotate(const Euler& rotation_);
		void Rotate(float x_, float y_, float z_);
		void Rotate(const Matrix3& rotation_);
		void Rotate(const Quaternion& rotation_);

		void SetTag(const std::string& tag_);
		void RemoveTag(const std::string& tag_);
		bool HasTag(const std::string& tag_) const;

		template <class T> T* GetComponent(){
			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
			T* comp = nullptr;

			//Check every component on this object
			for(Component* c : components){
				//Cast this Component* to a T* (whatever type of component you're looking for)
				comp = dynamic_cast<T*>(c);
				//If the cast was successful (did not return nullptr), then we have that type of component, so return it
				if(comp != nullptr){
					return comp;
				}
			}

			//If no component of this type was found, return nullptr
			return nullptr;
		}

		template <class T> std::vector<T*> GetComponents(){
			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
			std::vector<T*> compList = std::vector<T*>();

			for(Component* c : components){
				T* comp = dynamic_cast<T*>(c);
				if(comp != nullptr){
					compList.push_back(comp);
				}
			}

			return compList;
		}

		template <class T> T* GetComponentInChildren(){
			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
			T* comp = nullptr;

			//Check all children of this GameObject
			for(GameObject* go : children){
				//If the current checked object has this component, return it
				comp = go->GetComponent<T>();
				if(comp != nullptr){
					return comp;
				}

				//If not, check the children of this child for the component
				comp = go->GetComponentInChildren<T>();
				if(comp != nullptr){
					return comp;
				}
			}

			//If no component of this type was found, return nullptr
			return nullptr;
		}

		template <class T> std::vector<T*> GetComponentsInChildren(){
			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
			std::vector<T*> tmpList = std::vector<T*>();
			std::vector<T*> compList = std::vector<T*>();

			//Check all children of this object
			for(GameObject* go : children){
				//Add all the T*'s this child has to the list
				tmpList = go->GetComponents<T>();
				for(T* comp : tmpList){
					compList.push_back(comp);
				}

				//Get all the components from the children of this object and add them to the list
				std::vector<T*> c = go->GetComponentsInChildren<T>();
				for(T* t : c){
					compList.push_back(t);
				}
			}

			return compList;
		}

		template <class T> std::vector<T*> GetComponentsInFamily(){
			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
			//Get the list of any T*'s that the parent has
			std::vector<T*> compList = GetComponents<T>();

			//Get all the components on all child objects
			std::vector<T*> chList = GetComponentsInChildren<T>();
			for(T* t : chList){
				compList.push_back(t);
			}

			return compList;
		}

		template <class T> void RemoveComponent(){
			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
			T* comp = nullptr;

			for(unsigned int i = 0; i < components.size(); i++){
				comp = dynamic_cast<T*>(components[i]);
				if(comp != nullptr){
					removeComponentQueue.push(&components[i]);
				}
			}
		}

		template <class T> void RemoveComponents(){
			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
			for(unsigned int i = 0; i < components.size(); i++){
				T* comp = dynamic_cast<T*>(components[i]);
				if(comp != nullptr){
					removeComponentQueue.push(&components[i]);
				}
			}
		}

	protected:
		Transform transform;
		std::vector<GameObject*> children;
		std::vector<Component*> components;
		bool hasInitialized;
		bool isStatic;

		std::set<std::string> tags;

		std::queue<Component*> addComponentQueue;
		std::queue<Component**> removeComponentQueue;
		std::queue<GameObject*> createChildQueue;
		std::queue<GameObject*> removeChildQueue;
		std::queue<GameObject*> attachChildQueue;
		std::queue<GameObject*> detachChildQueue;
	};
}

#endif //!GAME_OBJECT_H
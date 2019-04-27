#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <algorithm>
#include <map>

#include "Resource.h"
#include"../Tools/Debug.h"

namespace PizzaBox{
	struct ResourceProfile{
		Resource* resourcePtr;
		unsigned int loadCount;
		bool isPermanent;

		ResourceProfile(Resource* resource_, const bool permanent_) : resourcePtr(resource_), loadCount(0), isPermanent(permanent_){}

		~ResourceProfile(){ 
			if(resourcePtr != nullptr){
				resourcePtr->Unload();
				delete resourcePtr;
				resourcePtr = nullptr;
			}
		}
	};
	
	class ResourceManager{
	public:
		static bool Initialize();
		static void Destroy();

		static void AddResource(const std::string& resourceName_, Resource* resource_);
		static void AddPermanentResource(const std::string& resourceName_, Resource* resource_);

		template <class T> static T* LoadResource(const std::string& resourceName_){
			//We'll use a lower case version of the name so that loading resources is always case-insensitive
			std::string name;
			std::transform(resourceName_.begin(), resourceName_.end(), std::back_inserter(name), tolower);

			//This assertion will trigger if this resource name is invalid
			_ASSERT(resources.find(name) != resources.end());

			//Convert the resource to a T*
			T* source = dynamic_cast<T*>(resources[name]->resourcePtr);
			//If the conversion fails then the resource name doesn't match the resource type, and this assertion will trigger
			_ASSERT(source != nullptr);

			if(resources[name]->loadCount == 0){
				//Load the resource if it hasn't already been loaded
				if(resources[name]->resourcePtr->Load() == false){
					Debug::Log("Resource couldn't be loaded!", __FILE__, __LINE__);
					return nullptr;
				}

				if(resources[name]->isPermanent){
					resources[name]->loadCount++; //Add an extra load to the load count to ensure the permanent resource never gets reloaded
				}
			}

			resources[name]->loadCount++;
			return source;
		}
		
		static void UnloadResource(const std::string& resourceName_);

		static void LoadPermanentResources();
		static void UnloadPermanentResources();

		//Delete unwanted compiler generated constructors, assignment operators and destructors
		ResourceManager() = delete;
		ResourceManager(const ResourceManager&) = delete;
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;
		ResourceManager& operator=(ResourceManager&&) = delete;
		~ResourceManager() = delete;

	private:
		static std::map<const std::string, ResourceProfile*> resources;
	};
}

#endif //!RESOURCE_MANAGER_H
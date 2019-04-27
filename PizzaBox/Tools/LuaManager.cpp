#include "LuaManager.h"

#include "Debug.h"
#include "Math/Euler.h"

using namespace PizzaBox;

union PizzaBox::PassByValue{
	char charVal;
	char16_t char16Val;
	char32_t char32Val;
	wchar_t wCharVal;

	signed char sCharVal;
	signed short int shortVal;
	signed int sIntVal;
	signed long int sLongVal;
	signed long long int sLongLongVal;

	unsigned char uCharVal;
	unsigned short int uShortVal;
	unsigned int uIntVal;
	unsigned long int uLongVal;
	unsigned long long int uLongLongVal;

	float floatVal;
	double doubleVal;
	long double longDoubleVal;

	bool boolVal;
	void* ptrVal;

	char* csVal;
	const char* ccsVal;
};

std::vector<std::string> LuaManager::enabledScripts;
lua_State* LuaManager::state = nullptr;
std::vector<PassByValue> LuaManager::argValues;
std::vector<std::string> LuaManager::stringArgValues;
std::vector<Vector3> vector3ArgValues;
std::vector<Euler> eulerArgValues;

template <class T>
std::vector<T> args;

bool LuaManager::Initialize(){
	#ifdef _DEBUG
	if(state != nullptr){
		Debug::LogError("Attempted to re-initialize LuaManager!", __FILE__, __LINE__);
		return false;
	}
	#endif //_DEBUG

	state = luaL_newstate();

	BindGlobalMethods();
	BindClasses();

	return true;
}

void LuaManager::Destroy(){
	argValues.clear();
	argValues.shrink_to_fit();
	stringArgValues.clear();
	stringArgValues.shrink_to_fit();
	vector3ArgValues.clear();
	vector3ArgValues.shrink_to_fit();
	eulerArgValues.clear();
	eulerArgValues.shrink_to_fit();

	if(state != nullptr){
		lua_close(state);
		state = nullptr;
	}
}

bool LuaManager::EnableScript(const std::string& scriptName_, const std::string& script_){
	#ifdef _DEBUG
	if(state == nullptr){
		Debug::LogError("LuaState was not active!", __FILE__, __LINE__);
		return false;
	}
	#endif //_DEBUG

	int result = luaL_dostring(state, script_.c_str());
	if(result != LUA_OK){
		Debug::LogError("Script was not executed successfully! Lua Error: " + std::string(lua_tostring(state, -1)));
		return false;
	}

	enabledScripts.push_back(scriptName_);
	return true;
}

void LuaManager::DisableScript(const std::string& scriptName_){
	if(state == nullptr || std::find(enabledScripts.begin(), enabledScripts.end(), scriptName_) == enabledScripts.end()){
		return;
	}

	//Set the script table to nil
	lua_pushnil(state);
	lua_setglobal(state, scriptName_.c_str());

	enabledScripts.erase(std::remove(enabledScripts.begin(), enabledScripts.end(), scriptName_), enabledScripts.end());
}

bool LuaManager::BindGlobalMethods(){
	//Create a table called 'Global'
	//Any global functions can be called through Lua scripts with the syntax:
	//Global.Function()

	lua_checkstack(state, 1); //Make sure we have enough space on the Lua stack

	lua_newtable(state);
	lua_pushvalue(state, -1);
	lua_setglobal(state, "Global");

	lua_pushvalue(state, -1);
	for(const auto& method : rttr::type::get_global_methods()){
		lua_checkstack(state, 3); //Make sure we have enough space on the Lua stack

		//Push every registered global method into the Global table
		lua_pushstring(state, method.get_name().to_string().c_str());
		lua_pushlightuserdata(state, (void*)&method);
		lua_pushcclosure(state, CallGlobalFromLua, 1);
		lua_settable(state, -3);
	}

	return true;
}

bool LuaManager::BindClasses(){
	for(const auto& classToRegister : rttr::type::get_types()){
		if(!classToRegister.is_class()){
			continue;
		}

		std::string typeName = classToRegister.get_name().to_string();

		lua_checkstack(state, 1); //Make sure we have enough space on the stack for our table

		lua_newtable(state);
		lua_pushvalue(state, -1);
		lua_setglobal(state, typeName.c_str());

		lua_pushvalue(state, -1);
		lua_pushstring(state, typeName.c_str());
		lua_pushcclosure(state, CreateUserData, 1);
		lua_setfield(state, -2, "new");

		//Add static functions
		for(const auto& method : classToRegister.get_methods()){
			if(method.is_static()){
				lua_checkstack(state, 1); //Make sure we have enough space on the stack for each function we want to push

				lua_pushvalue(state, -1);
				lua_pushstring(state, typeName.c_str());
				lua_pushstring(state, method.get_name().to_string().c_str());
				lua_pushcclosure(state, InvokeStaticMethod, 2);
				lua_setfield(state, -2, method.get_name().to_string().c_str());
			}
		}

		lua_checkstack(state, 1); //Make sure we have enough space on the stack for our metatable
		
		luaL_newmetatable(state, MetaTableName(classToRegister).c_str());
		lua_pushstring(state, "__gc");
		lua_pushcfunction(state, DestroyUserData);
		lua_settable(state, -3);

		lua_pushstring(state, "__index");
		lua_pushstring(state, typeName.c_str());
		lua_pushcclosure(state, IndexUserData, 1);
		lua_settable(state, -3);

		lua_pushstring(state, "__newindex");
		lua_pushstring(state, typeName.c_str());
		lua_pushcclosure(state, NewIndexUserData, 1);
		lua_settable(state, -3);
		
		//Add mathematic operators to metatable
		lua_pushstring(state, "__unm");
		lua_pushstring(state, typeName.c_str());
		lua_pushcclosure(state, InvokeUNM, 1);
		lua_settable(state, -3);

		lua_pushstring(state, "__add");
		lua_pushstring(state, typeName.c_str());
		lua_pushcclosure(state, InvokeAdd, 1);
		lua_settable(state, -3);

		lua_pushstring(state, "__sub");
		lua_pushstring(state, typeName.c_str());
		lua_pushcclosure(state, InvokeSub, 1);
		lua_settable(state, -3);

		lua_pushstring(state, "__mul");
		lua_pushstring(state, typeName.c_str());
		lua_pushcclosure(state, InvokeMul, 1);
		lua_settable(state, -3);

		lua_pushstring(state, "__div");
		lua_pushstring(state, typeName.c_str());
		lua_pushcclosure(state, InvokeDiv, 1);
		lua_settable(state, -3);
	}

	return true;
}

int LuaManager::InvokeMethod(const rttr::method& methodToInvoke_, const rttr::instance& object_){
	int luaStackOffset = 0;
	rttr::array_range<rttr::parameter_info> nativeParams = methodToInvoke_.get_parameter_infos();
	int numNativeArgs = static_cast<int>(nativeParams.size());
	int numLuaArgs = lua_gettop(state);

	//If we're given too many arguments, just assume that the top of the stack has what we're looking for
	if(numLuaArgs > numNativeArgs){
		luaStackOffset = numLuaArgs - numNativeArgs;
		numLuaArgs = numNativeArgs;
	}else if(numLuaArgs < numNativeArgs){
		//We can't properly call the function if we don't have the right number of arguments
		Debug::LogError("Invalid number of arguments passed to native function [" + methodToInvoke_.get_name().to_string() + "]! Expected " + std::to_string(numNativeArgs) + ", got " + std::to_string(numLuaArgs) + "!", __FILE__, __LINE__);
		return 0;
	}

	std::vector<rttr::argument> nativeArgs;
	nativeArgs.reserve(numNativeArgs);

	auto nativeParamsIt = nativeParams.begin();

	argValues = std::vector<PassByValue>(numLuaArgs);
	stringArgValues = std::vector<std::string>(numLuaArgs);
	vector3ArgValues = std::vector<Vector3>(numLuaArgs);
	eulerArgValues = std::vector<Euler>(numLuaArgs);

	for(int i = 0; i < numLuaArgs; i++, nativeParamsIt++){
		const rttr::type nativeParamType = nativeParamsIt->get_type();

		int luaArgIndex = i + 1 + luaStackOffset;
		int luaArgType = lua_type(state, luaArgIndex);

		nativeArgs.push_back(LuaToNativeArg(nativeParamType, luaArgIndex, luaArgType, i));
	}

	rttr::variant result = methodToInvoke_.invoke_variadic(object_, nativeArgs);
	return ToLuaValue(result, methodToInvoke_.get_name().to_string());
}

rttr::argument LuaManager::LuaToNativeArg(const rttr::type& nativeType_, int luaStackArgIndex_, int luaArgType_, int i_){
	//TODO - Some kind of error return other than nullptr if we can't deduce the argument type
	//TODO - Can this be done with templates somehow?
	rttr::variant* ud = nullptr;
	rttr::argument arg;

	switch(luaArgType_){
		case LUA_TNUMBER:
			if(!lua_isnumber(state, luaStackArgIndex_)){
				Debug::LogError("Expected a number!", __FILE__, __LINE__);
				return nullptr;
			}

			if(nativeType_ == rttr::type::get<char>()){
				argValues[i_].charVal = (char)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].charVal;
			}else if(nativeType_ == rttr::type::get<char16_t>()){
				argValues[i_].char16Val = (char16_t)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].char16Val;
			}else if(nativeType_ == rttr::type::get<char32_t>()){
				argValues[i_].char32Val = (char32_t)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].char32Val;
			}else if(nativeType_ == rttr::type::get<signed char>()){
				argValues[i_].sCharVal = (signed char)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].sCharVal;
			}else if(nativeType_ == rttr::type::get<signed short int>()){
				argValues[i_].shortVal = (signed short int)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].shortVal;
			}else if(nativeType_ == rttr::type::get<signed int>()){
				argValues[i_].sIntVal = (signed int)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].sIntVal;
			}else if(nativeType_ == rttr::type::get<signed long int>()){
				argValues[i_].sLongVal = (signed long int)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].sLongVal;
			}else if(nativeType_ == rttr::type::get<signed long long int>()){
				argValues[i_].sLongLongVal = (signed long long int)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].sLongLongVal;
			}else if(nativeType_ == rttr::type::get<unsigned char>()){
				argValues[i_].uCharVal = (unsigned char)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].uCharVal;
			}else if(nativeType_ == rttr::type::get<unsigned short int>()){
				argValues[i_].uShortVal = (unsigned short int)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].uShortVal;
			}else if(nativeType_ == rttr::type::get<unsigned int>()){
				argValues[i_].uIntVal = (unsigned int)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].uIntVal;
			}else if(nativeType_ == rttr::type::get<unsigned long int>()){
				argValues[i_].uLongVal = (unsigned long int)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].uLongVal;
			}else if(nativeType_ == rttr::type::get<unsigned long long int>()){
				argValues[i_].uLongLongVal = (unsigned long long int)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].uLongLongVal;
			}else if(nativeType_ == rttr::type::get<float>()){
				argValues[i_].floatVal = (float)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].floatVal;
			}else if(nativeType_ == rttr::type::get<double>()){
				argValues[i_].doubleVal = (double)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].doubleVal;
			}else if(nativeType_ == rttr::type::get<long double>()){
				argValues[i_].longDoubleVal = (long double)lua_tonumber(state, luaStackArgIndex_);
				return argValues[i_].longDoubleVal;
			}else{
				Debug::LogError("Unrecognized number parameter type [" + nativeType_.get_name().to_string() + "]!", __FILE__, __LINE__);
				return nullptr;
			}
		case LUA_TBOOLEAN:
			if(!lua_isboolean(state, luaStackArgIndex_)){
				Debug::LogError("Expected a boolean value!", __FILE__, __LINE__);
				return nullptr;
			}

			argValues[i_].boolVal = (bool)lua_toboolean(state, luaStackArgIndex_);
			return argValues[i_].boolVal;
		case LUA_TSTRING:
			if(!lua_isstring(state, luaStackArgIndex_)){
				Debug::LogError("Expected a string value!", __FILE__, __LINE__);
				return nullptr;
			}

			if(nativeType_ == rttr::type::get<char*>()){
				argValues[i_].csVal = (char*)lua_tostring(state, luaStackArgIndex_);
				return argValues[i_].csVal;
			}else if(nativeType_ == rttr::type::get<const char*>()){
				argValues[i_].ccsVal = (const char*)lua_tostring(state, luaStackArgIndex_);
				return argValues[i_].ccsVal;
			}else if(nativeType_ == rttr::type::get<std::string>()){
				stringArgValues[i_] = std::string((const char*)lua_tostring(state, luaStackArgIndex_));
				return stringArgValues[i_];
			}else{
				Debug::LogError("Unrecognized string parameter type [" + nativeType_.get_name().to_string() + "]!", __FILE__, __LINE__);
				return nullptr;
			}
		case LUA_TFUNCTION:
			if(!lua_isfunction(state, luaStackArgIndex_)){
				Debug::LogError("Expected a function value!", __FILE__, __LINE__);
				return nullptr;
			}

			Debug::LogError("Cannot use Lua functions as arguments for native functions!", __FILE__, __LINE__);
			return nullptr;
		case LUA_TUSERDATA:
			if(!lua_isuserdata(state, luaStackArgIndex_)){
				Debug::LogError("Expected a userdata value!", __FILE__, __LINE__);
				return nullptr;
			}

			ud = (rttr::variant*)lua_touserdata(state, luaStackArgIndex_);

			if(ud->is_type<Vector3>()){
				vector3ArgValues[i_] = ud->get_value<Vector3>();
				return vector3ArgValues[i_];
			}else if(ud->is_type<Vector3&>()){
				vector3ArgValues[i_] = ud->get_value<Vector3&>();
				return vector3ArgValues[i_];
			}else if(ud->is_type<std::shared_ptr<Vector3>>()){
				vector3ArgValues[i_] = *(ud->get_value<std::shared_ptr<Vector3>>());
				return vector3ArgValues[i_];
			}else if(ud->is_type<Euler>()){
				eulerArgValues[i_] = ud->get_value<Euler>();
				return eulerArgValues[i_];
			}else if(ud->is_type<Euler&>()){
				eulerArgValues[i_] = ud->get_value<Euler&>();
				return eulerArgValues[i_];
			}else if(ud->is_type<std::shared_ptr<Euler>>()){
				eulerArgValues[i_] = *(ud->get_value<std::shared_ptr<Euler>>());
				return eulerArgValues[i_];
			}else{
				Debug::LogError("Unrecognized userdata type [" + ud->get_type().get_raw_type().get_name().to_string() + "]!", __FILE__, __LINE__);
				return nullptr;
			}
		case LUA_TLIGHTUSERDATA:
			if(!lua_islightuserdata(state, luaStackArgIndex_)){
				Debug::LogError("Expected LightUserData!", __FILE__, __LINE__);
				return nullptr;
			}

			Debug::LogError("Cannot use LightUserData as arguments for native functions!", __FILE__, __LINE__);
			return nullptr;
		case LUA_TNIL:
			argValues[i_].ptrVal = nullptr;
			return argValues[i_].ptrVal;
		case LUA_TNONE:
			Debug::LogError("Argument type does not exist!", __FILE__, __LINE__);
			return nullptr;
		default:
			Debug::LogError("Unknown Lua Type " + std::to_string(luaArgType_) + "!", __FILE__, __LINE__);
			return nullptr;
	}
}

int LuaManager::ToLuaValue(rttr::variant& result_){
	if(!result_.is_valid()){
		Debug::LogError("Result was invalid!", __FILE__, __LINE__);
		return 0;
	}

	if(result_.is_type<void>() == false){
		//TODO - Handle all possible return types
		//TODO - Can this be done with templates somehow?
		if(result_.is_type<int>()){
			lua_pushnumber(state, result_.get_value<int>());
			return 1;
		}else if(result_.is_type<float>()){
			lua_pushnumber(state, result_.get_value<float>());
			return 1;
		}else if(result_.is_type<bool>()){
			lua_pushboolean(state, result_.get_value<bool>());
			return 1;
		}else if(result_.get_type().is_class() || result_.get_type().is_pointer()){
			return CreateUserDataFromVariant(result_);
		}else{
			Debug::LogError("Unhandled return type [" + result_.get_type().get_name().to_string() + "]!");
			return 0;
		}
	}

	//Nothing needs to be returned
	return 0;
}

int LuaManager::ToLuaValue(rttr::variant& result_, const std::string methodName_){
	if(!result_.is_valid()){
		Debug::LogError("Method [" + methodName_ + "] could not be invoked!", __FILE__, __LINE__);
		return 0;
	}

	return ToLuaValue(result_);
}

int LuaManager::CreateUserDataFromVariant(rttr::variant& v_){
	void* ud = lua_newuserdata(state, sizeof(rttr::variant));
	int userDataStackIndex = lua_gettop(state);
	new (ud) rttr::variant(v_);

	luaL_getmetatable(state, MetaTableName(v_.get_type()).c_str());
	lua_setmetatable(state, userDataStackIndex);

	lua_newtable(state);
	lua_setuservalue(state, userDataStackIndex);

	return 1;
}

std::string LuaManager::MetaTableName(const rttr::type& type_){
	if(type_.is_pointer()){
		return type_.get_raw_type().get_name().to_string() + "_MT_";
	}

	return (type_.get_name().to_string() + "_MT_");
}

int LuaManager::CallGlobalFromLua(lua_State* L_){
	if(!lua_isuserdata(L_, lua_upvalueindex(1))){
		Debug::LogError("Expected userdata upvalue!", __FILE__, __LINE__);
		return 0;
	}

	rttr::method* m = (rttr::method*)lua_touserdata(L_, lua_upvalueindex(1));

	if(m == nullptr || !m->is_valid()){
		Debug::LogError("Could not get rttr::method from Lua!", __FILE__, __LINE__);
		return 0;
	}

	rttr::method& methodToInvoke = (*m);
	rttr::instance object = {};

	return InvokeMethod(methodToInvoke, object);
}

int LuaManager::AddToLuaStack(){
	return 0;
}

int LuaManager::CreateUserData(lua_State* L_){
	if(!lua_isstring(L_, lua_upvalueindex(1))){
		Debug::LogError("Expected string upvalue!", __FILE__, __LINE__);
		return 0;
	}

	const char* typeName = (const char*)lua_tostring(L_, lua_upvalueindex(1));
	rttr::type typeToCreate = rttr::type::get_by_name(typeName);

	/*---------------------------------------------------------------------------------------------------------*/
	int numLuaArgs = lua_gettop(L_);

	const rttr::constructor* constructorToUse = nullptr;
	for(const auto& method : typeToCreate.get_constructors()){
		int numNativeArgs = static_cast<int>(method.get_parameter_infos().size());
		if(numNativeArgs == numLuaArgs){
			constructorToUse = &method;
			break;
		}
	}

	if(constructorToUse == nullptr){
		Debug::LogError("Invalid number of arguments passed to native constructor for [" + typeToCreate.get_name().to_string() + "]! Got " + std::to_string(numLuaArgs) + " arguments!", __FILE__, __LINE__);
		return 0;
	}

	std::vector<rttr::argument> nativeArgs;
	nativeArgs.reserve(constructorToUse->get_parameter_infos().size());

	auto nativeParamsIt = constructorToUse->get_parameter_infos().begin();

	LuaManager::argValues = std::vector<PassByValue>(numLuaArgs);
	LuaManager::stringArgValues = std::vector<std::string>(numLuaArgs);
	vector3ArgValues = std::vector<Vector3>(numLuaArgs);
	eulerArgValues = std::vector<Euler>(numLuaArgs);

	for(int i = 0; i < numLuaArgs; i++, nativeParamsIt++){
		nativeArgs.push_back(LuaManager::LuaToNativeArg(nativeParamsIt->get_type(), i + 1, lua_type(L_, i + 1), i));
	}
	/*---------------------------------------------------------------------------------------------------------*/

	void* ud = lua_newuserdata(L_, sizeof(rttr::variant));
	new (ud) rttr::variant(typeToCreate.create(nativeArgs));

	auto udptr = (rttr::variant*)ud;
	if(udptr == nullptr || !udptr->is_valid()){
		Debug::LogError("Userdata could not be created!", __FILE__, __LINE__);
		return 0;
	}

	luaL_getmetatable(L_, LuaManager::MetaTableName(typeToCreate).c_str());
	lua_setmetatable(L_, 1);

	lua_newtable(L_);
	lua_setuservalue(L_, numLuaArgs + 1);

	return 1;
}

int LuaManager::DestroyUserData(lua_State* L_){
	if(!lua_isuserdata(L_, -1)){
		Debug::LogError("Expected userdata value!", __FILE__, __LINE__);
		return 0;
	}

	rttr::variant* ud = (rttr::variant*)lua_touserdata(L_, -1);

	if(ud == nullptr){
		Debug::LogWarning("Attempted to destroy null userdata!", __FILE__, __LINE__);
		return 0;
	}

	ud->~variant();
	return 0;
}

int LuaManager::InvokeFuncOnUserData(lua_State* L_){
	if(!lua_isstring(L_, lua_upvalueindex(1))){
		Debug::LogError("Expected string upvalue!", __FILE__, __LINE__);
		return 0;
	}

	std::string methodName = (const char*)lua_tostring(L_, lua_upvalueindex(1));

	auto udPtr = (rttr::variant*)lua_touserdata(L_, 1);
	if(udPtr == nullptr || !udPtr->is_valid()){
		Debug::LogError("Could not retrieve userdata from Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	for(const auto& method : udPtr->get_type().get_methods()){
		if(method.get_name().to_string() == methodName){
			rttr::array_range<rttr::parameter_info> nativeParams = method.get_parameter_infos();
			int numNativeArgs = static_cast<int>(nativeParams.size());
			int numLuaArgs = lua_gettop(L_);

			if(numLuaArgs == numNativeArgs + 1){
				auto ud = *udPtr;
				rttr::instance object(ud);
				return LuaManager::InvokeMethod(method, object);
			}
		}
	}

	Debug::LogError("Could not find an overload of [" + methodName + "] with " + std::to_string(lua_gettop(L_) - 1) + " parameters!");
	return 0;
}

int LuaManager::IndexUserData(lua_State* L_){
	if(!lua_isstring(L_, lua_upvalueindex(1))){
		Debug::LogError("Expected string upvalue!", __FILE__, __LINE__);
		return 0;
	}

	const char* typeName = (const char*)lua_tostring(L_, lua_upvalueindex(1));
	rttr::type typeInfo = rttr::type::get_by_name(typeName);

	if(lua_isuserdata(L_, 1) == false){
		Debug::LogError("Userdata not found on Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	if(lua_isstring(L_, 2) == false){
		Debug::LogError("Name of native property/method found on Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	auto udPtr = (rttr::variant*)lua_touserdata(L_, 1);
	if(udPtr == nullptr || !udPtr->is_valid()){
		Debug::LogError("Could not retrieve userdata from Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	auto var = *udPtr;

	const char* fieldName = (const char*)lua_tostring(L_, 2);
	rttr::method m = typeInfo.get_method(fieldName);
	if(m.is_valid()){
		//void* methodUD = lua_newuserdata(L_, sizeof(rttr::method));
		//new (methodUD) rttr::method(m);
		lua_pushstring(L_, m.get_name().to_string().c_str());
		lua_pushcclosure(L_, InvokeFuncOnUserData, 1);
		return 1;
	}

	rttr::property p = typeInfo.get_property(fieldName);
	if(p.is_valid()){
		auto ud = *(rttr::variant*)lua_touserdata(L_, 1);
		rttr::variant result = p.get_value(ud);
		if(result.is_valid()){
			return LuaManager::ToLuaValue(result);
		}
	}

	//If not a method or property, assume it's a uservalue
	lua_getuservalue(L_, 1);
	lua_pushvalue(L_, 2);
	lua_gettable(L_, -2);
	return 1; //This will return nil even if nothing is there
}

int LuaManager::NewIndexUserData(lua_State* L_){
	if(!lua_isstring(L_, lua_upvalueindex(1))){
		Debug::LogError("Expected string upvalue!", __FILE__, __LINE__);
		return 0;
	}

	const char* typeName = (const char*)lua_tostring(L_, lua_upvalueindex(1));
	rttr::type typeInfo = rttr::type::get_by_name(typeName);

	if(lua_isuserdata(L_, 1) == false){
		Debug::LogError("Userdata not found on Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	if(lua_isstring(L_, 2) == false){
		Debug::LogError("Could not retrieve userdata from Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	const char* fieldName = (const char*)lua_tostring(L_, 2);
	rttr::property p = typeInfo.get_property(fieldName);
	if(p.is_valid()){
		auto ud = *(rttr::variant*)lua_touserdata(L_, 1);

		int luaType = lua_type(L_, 3);

		LuaManager::argValues = std::vector<PassByValue>(1);
		LuaManager::stringArgValues = std::vector<std::string>(1);
		vector3ArgValues = std::vector<Vector3>(1);
		eulerArgValues = std::vector<Euler>(1);

		p.set_value(ud, LuaManager::LuaToNativeArg(p.get_type(), 3, luaType, 0));

		return 0;
	}

	lua_getuservalue(L_, 1);
	lua_pushvalue(L_, 2);
	lua_pushvalue(L_, 3);
	lua_settable(L_, -3); //1[2] = 3

	return 0;
}

int LuaManager::InvokeStaticMethod(lua_State* L_){
	if(!lua_isstring(L_, lua_upvalueindex(1)) || !lua_isstring(L_, lua_upvalueindex(1))){
		Debug::LogError("Expected two string upvalues!", __FILE__, __LINE__);
		return 0;
	}

	const char* typeName = (const char*)lua_tostring(L_, lua_upvalueindex(1));
	rttr::type typeToCreate = rttr::type::get_by_name(typeName);

	const char* staticMethodName = (const char*)lua_tostring(L_, lua_upvalueindex(2));
	rttr::method staticMethod = typeToCreate.get_method(staticMethodName);

	rttr::instance obj = {};
	return LuaManager::InvokeMethod(staticMethod, obj);
}

int LuaManager::InvokeUNM(lua_State* L_){
	if(!lua_isstring(L_, lua_upvalueindex(1))){
		Debug::LogError("Expected string upvalue!", __FILE__, __LINE__);
		return 0;
	}

	const char* typeName = (const char*)lua_tostring(L_, lua_upvalueindex(1));
	rttr::type typeInfo = rttr::type::get_by_name(typeName);

	if(lua_isuserdata(L_, 1) == false){
		Debug::LogError("Userdata not found on Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	auto udPtr = (rttr::variant*)lua_touserdata(L_, 1);
	if(udPtr == nullptr || !udPtr->is_valid()){
		Debug::LogError("Could not retrieve userdata from Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	rttr::method multiplyMethod = typeInfo.get_method("__unm");
	return LuaManager::InvokeMethod(multiplyMethod, *udPtr);
}

int LuaManager::InvokeAdd(lua_State* L_){
	if(!lua_isstring(L_, lua_upvalueindex(1))){
		Debug::LogError("Expected string upvalue!", __FILE__, __LINE__);
		return 0;
	}

	const char* typeName = (const char*)lua_tostring(L_, lua_upvalueindex(1));
	rttr::type typeInfo = rttr::type::get_by_name(typeName);

	if(lua_isuserdata(L_, 1) == false){
		Debug::LogError("Userdata not found on Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	if(lua_isnumber(L_, 2) == false && lua_isuserdata(L_, 2) == false){
		Debug::LogError("Second argument of Add operator (number or userdata) not found on Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	auto udPtr = (rttr::variant*)lua_touserdata(L_, 1);
	if(udPtr == nullptr || !udPtr->is_valid()){
		Debug::LogError("Could not retrieve userdata from Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	rttr::method multiplyMethod = typeInfo.get_method("__add");
	return LuaManager::InvokeMethod(multiplyMethod, *udPtr);
}

int LuaManager::InvokeSub(lua_State* L_){
	if(!lua_isstring(L_, lua_upvalueindex(1))){
		Debug::LogError("Expected string upvalue!", __FILE__, __LINE__);
		return 0;
	}

	const char* typeName = (const char*)lua_tostring(L_, lua_upvalueindex(1));
	rttr::type typeInfo = rttr::type::get_by_name(typeName);

	if(lua_isuserdata(L_, 1) == false){
		Debug::LogError("Userdata not found on Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	if(lua_isnumber(L_, 2) == false && lua_isuserdata(L_, 2) == false){
		Debug::LogError("Second argument of Subtract operator (number or userdata) not found on Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	auto udPtr = (rttr::variant*)lua_touserdata(L_, 1);
	if(udPtr == nullptr || !udPtr->is_valid()){
		Debug::LogError("Could not retrieve userdata from Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	rttr::method multiplyMethod = typeInfo.get_method("__sub");
	return LuaManager::InvokeMethod(multiplyMethod, *udPtr);
}

int LuaManager::InvokeMul(lua_State* L_){
	if(!lua_isstring(L_, lua_upvalueindex(1))){
		Debug::LogError("Expected string upvalue!", __FILE__, __LINE__);
		return 0;
	}

	const char* typeName = (const char*)lua_tostring(L_, lua_upvalueindex(1));
	rttr::type typeInfo = rttr::type::get_by_name(typeName);

	if(lua_isuserdata(L_, 1) == false){
		Debug::LogError("Userdata not found on Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	if(lua_isnumber(L_, 2) == false && lua_isuserdata(L_, 2) == false){
		Debug::LogError("Second argument of Multiply operator (number or userdata) not found on Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	auto udPtr = (rttr::variant*)lua_touserdata(L_, 1);
	if(udPtr == nullptr || !udPtr->is_valid()){
		Debug::LogError("Could not retrieve userdata from Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	rttr::method multiplyMethod = typeInfo.get_method("__mul");
	return LuaManager::InvokeMethod(multiplyMethod, *udPtr);
}

int LuaManager::InvokeDiv(lua_State* L_){
	if(!lua_isstring(L_, lua_upvalueindex(1))){
		Debug::LogError("Expected string upvalue!", __FILE__, __LINE__);
		return 0;
	}

	const char* typeName = (const char*)lua_tostring(L_, lua_upvalueindex(1));
	rttr::type typeInfo = rttr::type::get_by_name(typeName);

	if(lua_isuserdata(L_, 1) == false){
		Debug::LogError("Userdata not found on Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	if(lua_isnumber(L_, 2) == false && lua_isuserdata(L_, 2) == false){
		Debug::LogError("Second argument of Divide operator (number or userdata) not found on Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	auto udPtr = (rttr::variant*)lua_touserdata(L_, 1);
	if(udPtr == nullptr || !udPtr->is_valid()){
		Debug::LogError("Could not retrieve userdata from Lua stack!", __FILE__, __LINE__);
		return 0;
	}

	rttr::method multiplyMethod = typeInfo.get_method("__div");
	return LuaManager::InvokeMethod(multiplyMethod, *udPtr);
}
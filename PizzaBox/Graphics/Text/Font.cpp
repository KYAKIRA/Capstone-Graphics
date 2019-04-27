#include "Font.h"

#include "../../Tools/Debug.h"
#include "FontEngine.h"

using namespace PizzaBox;

Font::Font(const std::string fontPath_) : Resource(fontPath_), characters(std::map<char, FontCharacter>()){
}

Font::~Font(){
}

bool Font::Load(){
	characters = FontEngine::LoadFont(fileName);
	if(characters.size() == 0){
		Debug::LogError("Could not load font!");
		return false;
	}

	return true;
}

void Font::Unload(){
	if(characters.size() > 0){
		characters.clear();
	}
}

std::map<char, FontCharacter>& Font::Characters(){
	return characters;
}
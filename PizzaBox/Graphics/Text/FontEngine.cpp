#include "FontEngine.h"

#include "../../Tools/Debug.h"

using namespace PizzaBox;

FT_Library FontEngine::ftLib = nullptr;

bool FontEngine::Initialize(){
	//If this assertion fires, then we're attempting to initialize this more than once
	_ASSERT(ftLib == nullptr);

	//Attempt to initialize FreeType
	FT_Error err = FT_Init_FreeType(&ftLib);
	if(err != FT_Err_Ok){
		Debug::LogError("Could not initialize FreeType! FreeType Error Code: " + std::to_string(err));
		return false;
	}

	return true;
}

void FontEngine::Destroy(){
	if(ftLib != nullptr){
		FT_Error err = FT_Done_FreeType(ftLib);
		if(err != FT_Err_Ok){
			Debug::LogError("Could not cleanup FreeType! FreeType Error Code: " + std::to_string(err));
		}

		ftLib = nullptr;
	}
}

std::map<char, FontCharacter> FontEngine::LoadFont(const std::string fontPath_){
	//FontEngine must be initialized before this is ever called
	_ASSERT(ftLib != nullptr);

	//Create the font face and the character map
	FT_Face fontFace = nullptr;
	std::map<char, FontCharacter> characters = std::map<char, FontCharacter>();

	//Attempt to initialize the font face
	FT_Error err = FT_New_Face(ftLib, fontPath_.c_str(), 0, &fontFace);
	if(err != FT_Err_Ok){
		Debug::LogError("Could not load font face! FreeType Error Code: " + std::to_string(err));
		characters.clear();
		return characters;
	}

	//Set size to load glyphs as
	err = FT_Set_Pixel_Sizes(fontFace, 0, 48);
	if(err != FT_Err_Ok){
		Debug::LogError("Could not set FreeType pixel sizes! FreeType Error Code: " + std::to_string(err));
		characters.clear();
		return characters;
	}

	//Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

	//Load the first 128 ASCII characters
	for(unsigned char c = 0; c < 128; c++) {  
		//Load character glyph 
		err = FT_Load_Char(fontFace, c, FT_LOAD_RENDER);
		if(err != FT_Err_Ok){
			Debug::LogError("Could not load glyph for character " + static_cast<char>(c) + std::string("! FreeType Error Code" + std::to_string(err)));
			characters.clear();
			return characters;
		}

		//Generate texture
		GLuint texture;
		glActiveTexture(GL_TEXTURE1);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RED,
			fontFace->glyph->bitmap.width,
			fontFace->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE,
			fontFace->glyph->bitmap.buffer
		);

		//Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		//Now store character for later use
		FontCharacter character = {
			texture,
			static_cast<int>(fontFace->glyph->bitmap.width),
			static_cast<int>(fontFace->glyph->bitmap.rows),
			fontFace->glyph->bitmap_left,
			fontFace->glyph->bitmap_top,
			fontFace->glyph->advance.x
		};
		characters.insert(std::pair<char, FontCharacter>(c, character));
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	
	//Clear out the font face since we don't need it anymore
	FT_Done_Face(fontFace);

	//Reset byte alignment to initial value
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	return characters;
}
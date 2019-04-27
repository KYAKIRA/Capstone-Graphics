#ifndef FONT_ENGINE_H
#define FONT_ENGINE_H

#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "FontCharacter.h"

namespace PizzaBox{
	class FontEngine{
	public:
		static bool Initialize();
		static void Destroy();

		static std::map<char, FontCharacter> LoadFont(const std::string fontPath_);

	private:
		static FT_Library ftLib;
	};
}

#endif //!FONT_ENGINE_H
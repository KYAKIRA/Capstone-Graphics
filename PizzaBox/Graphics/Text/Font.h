#ifndef FONT_H
#define FONT_H

#include <map>

#include "../../Resource/Resource.h"
#include "FontCharacter.h"

namespace PizzaBox{
	class Font : public Resource{
	public:
		Font(const std::string fontPath_);
		~Font() override;

		bool Load() override;
		void Unload() override;

		std::map<char, FontCharacter>& Characters();
	private:
		std::map<char, FontCharacter> characters;
	};
}

#endif //!FONT_H
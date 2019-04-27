#ifndef FONT_CHARACTER_H
#define FONT_CHARACTER_H

#include <glew.h>

#include "../../Core/ScreenCoordinate.h"

namespace PizzaBox{
	struct FontCharacter{
		GLuint textureID;
		int sizeX, sizeY;
		int offsetX, offsetY;
		int next;
	};
}

#endif //!FONT_CHARACTER_H
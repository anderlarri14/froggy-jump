#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define GLT_IMPLEMENTATION
#include "gltext.h"
#include "text.h"

Text::Text()
{
    gltInit();
    this->GLtext = gltCreateText();
}

Text::~Text()
{
    gltTerminate();
}


void Text::Draw(const char *text, glm::vec2 position, glm::vec4 color, float scale)
{
    gltSetText(this->GLtext, text);
    gltBeginDraw();
    gltColor(color.r, color.g, color.b, color.a);
	gltDrawText2D(this->GLtext, position.x, position.y, scale);
    gltEndDraw();
}

void Text::Clear()
{
    gltDeleteText(this->GLtext);
}

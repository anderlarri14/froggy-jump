#ifndef TEXT_H
#define TEXT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gltext.h"
#include <glm/glm.hpp>

class Text
{
public:
    GLTtext *GLtext;

    Text();
    ~Text();
    void Init();
    void Draw(const char *text, glm::vec2 position, glm::vec4 color, float scale);
    void Clear();
};

#endif

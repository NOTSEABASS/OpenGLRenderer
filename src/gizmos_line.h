#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
class GLine
{
public:
    struct Line
    {
        glm::vec3 start_pos;
        glm::vec3 end_pos;
    } line;

    GLine(glm::vec3 start, glm::vec3 end)
    {
        line.start_pos = start;
        line.end_pos = end;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(line), &line, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
    }

    void Draw()
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 2);
    }

private:
    unsigned int VAO;
    unsigned int VBO;
};
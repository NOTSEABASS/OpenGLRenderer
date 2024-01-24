#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <transform.h>
class GLine
{
public:
    struct Line
    {
        glm::vec3 start_pos;
        glm::vec3 end_pos;
    } line;
    glm::vec3 color = glm::vec3(1, 1, 1);
    GLine() {}
    ~GLine() 
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

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

    void DrawInGlobal()
    {
        Shader::LoadedShaders["color.fs"]->use();
        Shader::LoadedShaders["color.fs"]->setVec3("color", color);
        Shader::LoadedShaders["color.fs"]->setMat4("model", glm::mat4(1));
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 2);
    }

    void Draw()
    {
        Shader::LoadedShaders["color.fs"]->use();
        Shader::LoadedShaders["color.fs"]->setVec3("color", color);
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 2);
    }

private:
    unsigned int VAO;
    unsigned int VBO;
};

class GCube
{
public:
    Transform transform;
    glm::vec3 color = glm::vec3(1, 1, 1);
    float radians;

    GCube(float _radians) : radians(_radians)
    {
        A = -glm::vec3(_radians);
        B = A + glm::vec3(_radians * 2, 0, 0);
        C = B + glm::vec3(0, _radians * 2, 0);
        D = A + glm::vec3(0, _radians * 2, 0);
        E = A + glm::vec3(0, 0, _radians * 2);
        F = E + glm::vec3(_radians * 2, 0, 0);
        G = F + glm::vec3(0, _radians * 2, 0);
        H = E + glm::vec3(0, _radians * 2, 0);

        line[0] = GLine(A, B);
        line[1] = GLine(B, C);
        line[2] = GLine(C, D);
        line[3] = GLine(D, A);

        line[4] = GLine(E, F);
        line[5] = GLine(F, G);
        line[6] = GLine(G, H);
        line[7] = GLine(H, E);

        line[8] = GLine(A, E);
        line[9] = GLine(B, F);
        line[10] = GLine(C, G);
        line[11] = GLine(D, H);
    }

    ~GCube() { }

    void Draw()
    {
        Shader::LoadedShaders["color.fs"]->use();
        Shader::LoadedShaders["color.fs"]->setVec3("color", color);
        Shader::LoadedShaders["color.fs"]->setMat4("model", transform.GetTransformMatrix());
        for (int i = 0; i < 12; i++)
        {
            line[i].color = color;
            line[i].Draw();
        }
    }

private:
    glm::vec3 A;
    glm::vec3 B;
    glm::vec3 C;
    glm::vec3 D;
    glm::vec3 E;
    glm::vec3 F;
    glm::vec3 G;
    glm::vec3 H;

    GLine line[12];
};
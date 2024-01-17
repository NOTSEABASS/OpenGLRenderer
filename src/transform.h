#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

class Transform
{
public:
    glm::vec3 Position;
    glm::vec3 Rotation; // Euler
    glm::vec3 Scale;
    const glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    Transform(glm::vec3 pos = glm::vec3(0,0,0), glm::vec3 rot = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(1,1,1)) : Position(pos), Rotation(rot), Scale(scale)
    {
        UpdateVectors();
    }

    ~Transform() {}
    
    glm::vec3 GetFront()
    {
        UpdateVectors();
        return Front;
    }

    glm::vec3 GetRight()
    {
        UpdateVectors();
        return Right;
    }

    glm::vec3 GetUp()
    {
        UpdateVectors();
        return Up;
    }

    void UpdateVectors()
    {
        // calculate the new Front vector
        glm::vec4 front(0,0,1,1);
        glm::mat4 rot_mat = glm::eulerAngleXYZ(glm::radians(Rotation.x), glm::radians(Rotation.y), glm::radians(Rotation.z));
        front = front * rot_mat;
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

    glm::mat4 GetTransformMatrix()
    {
        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, Position);
        glm::mat4 rot_mat = glm::eulerAngleXYZ(glm::radians(Rotation.x), glm::radians(Rotation.y), glm::radians(Rotation.z));
        mat = mat * rot_mat;
        mat = glm::scale(mat, Scale);
        return mat;
    }

private:
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up;

};
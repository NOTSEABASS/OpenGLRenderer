#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
public:
    Transform(glm::vec3 pos = glm::vec3(0,0,0), glm::vec3 rot = glm::vec3(0,0,0), glm::vec3 scal = glm::vec3(1,1,1)) : position(pos), rotation(rot), scale(scal)
    {
        UpdateVectors();
    }

    ~Transform() {}

    Transform& operator=(const Transform& rhs)
    {
        this->position = rhs.position;
        this->rotation = rhs.rotation;
        this->scale = rhs.scale;
        this->UpdateVectors();
        return *this;
    }

        void SetPosition(float x, float y, float z)
    {
        position.r = x;
        position.g = y;
        position.b = z;
    }

    void SetRotation(float Pitch, float Yaw, float Roll)
    {
        rotation.r = Pitch;
        rotation.g = Yaw;
        rotation.b = Roll;
        UpdateVectors();
    }

    void SetScale(float x, float y, float z)
    {
        scale.r = x;
        scale.g = y;
        scale.b = z;
    }

    const glm::vec3 Position()
    {
        return position;
    }

    const glm::vec3 Scale()
    {
        return scale;
    }

    const glm::vec3 Rotation()
    {
        return rotation;
    }

    const glm::vec3 GetFront()
    {
        UpdateVectors();
        return Front;
    }

    const glm::vec3 GetRight()
    {
        UpdateVectors();
        return Right;
    }

    const glm::vec3 GetUp()
    {
        UpdateVectors();
        return Up;
    }

    void UpdateVectors()
    {
        // calculate the new Front vector
        glm::vec4 front(0,0,1,0);
        glm::vec4 up(0,1,0,0);
        glm::quat quat(glm::radians(rotation));
        glm::mat4 rot = glm::mat4_cast(quat);
        Front = rot * front;
        Up = rot * up;
        Right = glm::normalize(glm::cross(Front, Up));
    }

    glm::mat4 GetTransformMatrix()
    {
        glm::mat4 mat = glm::mat4(1.0f);
        glm::mat4 s = glm::scale(mat, scale);
        glm::quat quat(glm::radians(rotation));
        glm::mat4 r = glm::mat4_cast(quat);
        glm::mat4 t = glm::translate(mat, position);
        return t * r * s;
    }

private:
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up;

    glm::vec3 position;
    glm::vec3 rotation; // Euler
    glm::vec3 scale;
    const glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

};
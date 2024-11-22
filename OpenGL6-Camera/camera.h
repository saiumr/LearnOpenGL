#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement {
    kForward,
	kBackward,
	kLeft,
	kRight
};

const float YAW         = -90.0f;
const float PITCH       = 0.0f;
const float SPEED	    = 5.0f;
const float SENSITIVITY = 0.5f;
const float ZOOM        = 45.0f;

class Camera {
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3 up = glm::vec3{ 0.0f, 1.0f, 0.0f }, float yaw = YAW, float pitch = PITCH) :
        Front(glm::vec3{ 0.0f, 0.0f, -1.0f }), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        Yaw      = yaw;
        WorldUp  = up;
        Pitch    = pitch;
        updateCameraVectors();
    }

    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
        Front(glm::vec3{ 0.0f, 0.0f, -1.0f }), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3{ posX, posY, posZ };
        WorldUp  = glm::vec3{ upX, upY, upZ };
        Yaw      = yaw;
        Pitch    = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        // practice2
        return MyLookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(CameraMovement direction, float deltaTime, GLboolean jump=false)
    {
        float velocity = MovementSpeed * deltaTime;
        glm::vec3 v { 0.0f };
        if (direction == kForward) {
            v += Front * velocity;
        }
        if (direction == kBackward) {
            v -= Front * velocity;
        }
        if (direction == kLeft) {
            v -= Right * velocity;
        }
        if (direction == kRight) {
            v += Right * velocity;
        }

        // practice1: real fps camera
        v.y = 0;
        Position += v;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset * 2.0f;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // practice2: MyLookAt
    glm::mat4 MyLookAt(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp) {
        glm::vec3 zaxis = glm::normalize(position - target);
        glm::vec3 xaxis = glm::cross(worldUp, zaxis);
        glm::vec3 yaxis = glm::cross(zaxis, xaxis);

        // In glm we access elements as mat[col][row] due to column-major layout
        glm::mat4 translation = glm::mat4 {1.0f};
        translation[3][0] = -position.x;
        translation[3][1] = -position.y;
        translation[3][2] = -position.z;
        
        glm::mat4 rotation = glm::mat4{1.0f};
        rotation[0][0] = xaxis.x; // First column, first row
        rotation[1][0] = xaxis.y;
        rotation[2][0] = xaxis.z;
        rotation[0][1] = yaxis.x; // First column, second row
        rotation[1][1] = yaxis.y;
        rotation[2][1] = yaxis.z;
        rotation[0][2] = zaxis.x; // First column, third row
        rotation[1][2] = zaxis.y;
        rotation[2][2] = zaxis.z;

        return rotation * translation;
    }

    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));  // In the "Camera" section of the tutorial, x/z represents the xoz plane. Initially, the projection of the direction onto the xoz plane is calculated.
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

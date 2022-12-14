#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 0.75f;
const float SENSITIVITY = 0.2f;
const float ZOOM = 45.0f;

    


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
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
    // these two are used to turn off camera update with mouse when pressing C
    bool useMouse = true;
    bool Creleased = true; 
    
    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.5f, -0.5f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    glm::mat4 GetViewCubeMatrix(int faceId);

    glm::mat4 GetProjectionMatrix(float fov = 45.0, float ratio = 1.0, float near = 0.01, float far = 100.0);

    glm::mat4 GetProjectionMatrixCube(const float& angleOfView, const float& near, const float& far);

    glm::mat4 GetReflectionMatrix(glm::vec3 mirrorP, glm::vec3 mirrorN);

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboardMovement(Camera_Movement direction, float deltaTime);

    void ProcessKeyboardRotation(float YawRot, float PitchRot, float deltaTime, GLboolean constrainPitch = true);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    void MouseSwitchActivation(bool released, GLFWwindow* window);

    //reset the position to the origin, pointing towards positive x
    void resetPosition();

private:
    
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
 };

#endif
#include "../headers/camera.h"



// constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up , float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    this->Position = position;
    this->WorldUp = up;
    this->Yaw = yaw;
    this->Pitch = pitch;
    this->updateCameraVectors();
}
// constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    this->Position = glm::vec3(posX, posY, posZ);
    this->WorldUp = glm::vec3(upX, upY, upZ);
    this->Yaw = yaw;
    this->Pitch = pitch;
    this->updateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

glm::mat4 Camera::GetProjectionMatrix(float fov , float ratio , float near , float far)
{
    return glm::perspective(fov, ratio, near, far);
}

glm::mat4 Camera::GetReflectionMatrix(glm::vec3 mirrorP, glm::vec3 mirrorN) {
    glm::mat4 R = glm::mat4(1.0f);

    /*R[0][0] = 1.0 - 2.0 * mirrorN.x * mirrorN.x;
    R[0][1] = -2.0 * mirrorN.x * mirrorN.y;
    R[0][2] = -2.0 * mirrorN.x * mirrorN.z;
    R[0][3] = 2.0 * glm::dot(mirrorP, mirrorN) * mirrorN.x;

    R[1][1] = - 2.0 * mirrorN.y * mirrorN.x;
    R[1][1] = 1.0 - 2.0 * mirrorN.y * mirrorN.y;
    R[1][2] = -2.0 * mirrorN.y * mirrorN.z;
    R[1][3] = 2.0 * glm::dot(mirrorP, mirrorN) * mirrorN.y;

    R[2][0] = -2.0 * mirrorN.z * mirrorN.x;
    R[2][1] = -2.0 * mirrorN.z * mirrorN.y;
    R[2][2] = 1.0 - 2.0 * mirrorN.z * mirrorN.z;
    R[2][3] = 2.0 * glm::dot(mirrorP, mirrorN) * mirrorN.z;*/
    glm::vec3 Z = glm::vec3(0.0, 0.0, 1.0);
    glm::vec3 perp = glm::cross(Z, mirrorN);
    

    R = glm::translate(R, mirrorP); // translate mirror center at the origin
    if( glm::length(perp)>0.01f)
       R = glm::rotate(R, -glm::asin(glm::length(perp)), glm::normalize(perp)); // rotate the mirror into XY plane
    R = glm::scale(R, glm::vec3(1.0, 1.0, -1.0));
    if ( glm::length(perp) > 0.01f)
       R = glm::rotate(R, glm::asin(glm::length(perp)), glm::normalize(perp)); // rotate back the mirror into XY plane (if not already 
    R = glm::translate(R, -mirrorP); //translate back the scene 
    return R;
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboardMovement(Camera_Movement direction, float deltaTime)
{
    float velocity = this->MovementSpeed * deltaTime;
    if (direction == FORWARD)
        this->Position += this->Front * velocity;
    if (direction == BACKWARD)
        this->Position -= this->Front * velocity;
    if (direction == LEFT)
        this->Position -= this->Right * velocity;
    if (direction == RIGHT)
        this->Position += this->Right * velocity;
    if (this->Position.y <= 0.5)
        this->Position.y = 0.5;
}

void Camera::ProcessKeyboardRotation(float YawRot, float PitchRot, float deltaTime, GLboolean constrainPitch)
{
    float velocity = this->MovementSpeed * deltaTime;
    YawRot *= velocity;
    PitchRot *= velocity;

    this->Yaw += YawRot;
    this->Pitch += PitchRot;


    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (this->Pitch > 89.0f)
            this->Pitch = 89.0f;
        if (this->Pitch < -89.0f)
            this->Pitch = -89.0f;
    }
    updateCameraVectors();

}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    /* The motivated students can implement rotation using the mouse rather than the keyboard
    * You can draw inspiration from the ProcessKeyboardMovement function
    */

    if (useMouse) {
        float YawRot = SENSITIVITY * xoffset;
        float PitchRot = SENSITIVITY * yoffset;

        this->Yaw += YawRot;
        this->Pitch += PitchRot;


        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (this->Pitch > 89.0f)
                this->Pitch = 89.0f;
            if (this->Pitch < -89.0f)
                this->Pitch = -89.0f;
        }
        updateCameraVectors();
    }
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::MouseSwitchActivation(bool released, GLFWwindow* window) {
    if (Creleased && !released) {
        useMouse = !useMouse;
        if (useMouse) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    Creleased = released;

}
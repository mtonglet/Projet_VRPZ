#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "object.h"




// BallObject holds the mouvement equation of a ball falling with bounces
class Ball
{
public:
    // ball state	
    glm::vec3 positions;
    glm::vec3 velocity;
    bool    Stuck;
    //parameter for the mouvement equation
    float   E;      //coefficient de restitution
    int     N;      //number of bounce
    float   t;      //bounce time


    Ball();
    // moves the ball following a mouvement equations, returns new position
    glm::vec4 Move(float dt, glm::vec3 positions);
    // AABB - AABB collision detection
    bool CheckCollision(glm::vec3 positions);
    // resets the ball to original position and velocity
    void Reset();
};

#endif
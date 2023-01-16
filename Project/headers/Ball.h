#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "object.h"




// BallObject holds the state of the Ball object inheriting
// relevant state data from GameObject. Contains some extra
// functionality specific to Breakout's ball object that
// were too specific for within GameObject alone.
class Ball
{
public:
    // ball state	
    float   E;
    glm::vec3 positions;
    bool    Stuck;
    glm::vec3 velocity;
    int     N;
    float   t;      //temps rebond
    int position_x0;

    // constructor(s)
    Ball();

    // moves the ball, keeping it constrained within the window bounds (except bottom edge); returns new position
    glm::vec4 Move(float dt, glm::vec3 positions);
    bool CheckCollision();
    // resets the ball to original state with given position and velocity
    void      Reset(glm::vec3 position, glm::vec3 velocity);
};

#endif
#include "../headers/Ball.h"
#include <iostream>






Ball::Ball(){
    positions = glm::vec3(0.0f, 0.0f, 0.0f);
    velocity = glm::vec3(1.4f, 0.0f, 1.8f);
    E = 0.8f;
    N = -1;
    t = 0.0f;
}

glm::vec4 Ball::Move(float dt, glm::vec3 pos)
{
    positions = pos;
    // if not stuck to christmas tree
    if (!Stuck)
    {
        if (CheckCollision(positions)) {
            velocity = -velocity;
        }
        //everytime the ball touch the ground
        //restart time t
        if (positions.y < 0.4) {
            N += 1;
            
            if (N > 13) {
                Stuck = true;
  
                N = 0;
            }
            else {
                t = 0.0;
            }
        }
        //chute initiale (gravity)
        if (N == -1) {
            t += dt;
            positions.x += velocity.x * dt;
            positions.z += velocity.z * dt;
            positions.y =  float(3.5 - (9.81 / 2.0) * t * t) ;
        }
        // move the ball
        if (N > 0) {
       
            t += dt;
            positions.x += velocity.x * dt;
            positions.z +=  velocity.z * dt;
            positions.y =  float(pow(E, N) * pow(2.0 * 9.81 * 3.5, 0.5) * t - (9.81 / 2.0) * t * t + 0.5);
        } 
    }
    return glm::vec4(positions , 1.0);
}

//check for collition: AABB - AABB collision detection
// check if the right side of the ball is greater than the left side of the fireplace object
// and if the fireplace object's right side is greater than the ball's left side
bool Ball::CheckCollision(glm::vec3 positions)
{
  
    bool collisionX = positions.x + 0.5 >= -2.5 &&
        -2.5 + 6.0 >= positions.x;

    bool collisionZ = positions.z + 0.5 >= 6.2 &&
        6.2 + 2.0 >= positions.z;

    return collisionZ && collisionX;
}

// resets the ball to initial stuck state and velocity
void Ball::Reset()
{
    velocity = glm::vec3(1.4f, 0.0f, 1.8f);
    Stuck = true;
    N = -1;
}
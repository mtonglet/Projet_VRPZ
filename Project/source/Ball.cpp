#include "../headers/Ball.h"
#include <iostream>






Ball::Ball(){
    positions = glm::vec3(0.0f, 0.0f, 0.0f);
    velocity = glm::vec3(0.0f, 0.0f, 1.8f);
    E = 0.8f;
    Stuck = true;
    N = 0;
    t = 0.0f;
    position_x0 = 0;
}

glm::vec4 Ball::Move(float dt, glm::vec3 pos)
{
    positions = pos;
    // if not stuck to christmas tree
    if (!this->Stuck)
    {
       
        if (positions.y < 0.5) {
            N += 1;
            
            if (positions.z > 6.0) {
                Stuck = true;
  
                N = -1;
            }
            else {
                t = 0.0;
            }
        }
        if (N == 0) {
            t += dt;
            positions.z += velocity.z * dt;
            positions.y =  float(3.5 - (9.81 / 2.0) * t * t) ;
        }
        // move the ball
        if (N > 0) {
       
            t += dt;
            positions.z +=  velocity.z * dt;
            positions.y =  float(pow(E, N) * pow(2.0 * 9.81 * 3.5, 0.5) * t - (9.81 / 2.0) * t * t + 0.5);

            std::cout << "positions.y " << positions.y << std::endl;
        } 
        //std::cout << "temps " << t << std::endl;
   
    }
    return glm::vec4(positions , 1.0);
}

//check for collition
bool Ball::CheckCollision()
{
    bool collisionX = positions.x + 0.1 >= 2.0 &&
        2.0 + 0.1 >= positions.x;

    return collisionX;
}

// resets the ball to initial Stuck Position (if ball is outside window bounds)
void Ball::Reset(glm::vec3 position, glm::vec3 velocity)
{
    positions = position;
    velocity = velocity;
    Stuck = true;
}
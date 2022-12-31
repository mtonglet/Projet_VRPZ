#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include <time.h>

#include "shader.h"

//Emitter model & construction

class Emitter
{
    public:
        Emitter();
        //~Emitter();
        glm::vec3 randomPos();
        float randomLife();

        void update(const float dt);
        void draw(Shader shader);

        //GLuint VBO, VAO;

    private:
        struct Particle
        {
            glm::vec3 position;
            float lifetime;
        };
        std::vector<Particle> particles;

        float positions[400];

        GLuint vertexBuffer, positionBuffer;
};

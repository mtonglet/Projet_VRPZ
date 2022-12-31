#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include<iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleGenerator
{
    public:
        ParticleGenerator(Shader shader, Texture texture, unsigned int amount);

        void Update(float dt, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));

        void Draw();
    private:
        std::vector<Particle> particles;
        unsigned int amount;
        Shader shader;
        Texture texture;
        unsigned int VAO;
        void init();
        unsigned int firstUnusedParticle();
        void respawnParticle(Particle &particle, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif
#include "../headers/emitter.h"
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//Emitter model & construction

Emitter::Emitter(int name)
{
    if (name == 1) {
        srand(static_cast <unsigned> (time(0)));
        particles.resize(1000);

        for (int i = 0; i < particles.size(); ++i) {
            particles[i].position = randomPos();
            std::cout << particles[i].position.x << std::endl;
            particles[i].lifetime = randomLife(13);
        }

        std::vector<float> vertices;

        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &positionBuffer);

        //quad in xy plane
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);

        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * 4 * sizeof(float), positions, GL_DYNAMIC_DRAW);
    }
    if (name == 2) {
        srand(static_cast <unsigned> (time(0)));
        particles.resize(1000);

        for (int i = 0; i < particles.size(); ++i) {
            particles[i].position = randomPos(0.0, 0.1, 7.0);
           // std::cout << particles[i].position.x << std::endl;
            particles[i].lifetime = randomLife(2);
        }

        std::vector<float> vertices;

        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &positionBuffer);

        //quad in xy plane
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);

        vertices.push_back(1.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * 4 * sizeof(float), positions, GL_DYNAMIC_DRAW);
    }
}

glm::vec3 Emitter::randomPos()
{
    glm::vec3 randomPosition;
    //srand(time(NULL));
    randomPosition.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100 + 10;
    randomPosition.y = 20;
    randomPosition.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100 - 50;

    return randomPosition;
}

glm::vec3 Emitter::randomPos(int x, int y, int z)
{
    glm::vec3 randomPosition;
    //srand(time(NULL));
    randomPosition.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 3 + x - 1.5;
    randomPosition.y = y;
    randomPosition.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 + z;

    return randomPosition;
}

float Emitter::randomLife(int num)
{
    //srand(time(NULL));
    float lifeTime = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * num;
    return lifeTime;
}

void Emitter::update(const float dt, const int name)
{
    for (int i = 0; i < particles.size(); ++i)
    {
        particles[i].lifetime -= dt;

        if (name == 1) {
            if (particles[i].lifetime <= 0.0f)
            {
                particles[i].position = randomPos();
                particles[i].lifetime = randomLife(13);
            }
            particles[i].position -= glm::vec3(0.0f, dt * 2.0f, 0.0f);
        }
        if (name == 2) {
            if (particles[i].lifetime <= 0.0f)
            {
                particles[i].position = randomPos(0.0, 0.1, 7.0);
                particles[i].lifetime = randomLife(2);
            }
            if (particles[i].position[0] > 0.3f)
            {
                particles[i].lifetime -= dt * (particles[i].position[0] - 0.3);

            }
            if (particles[i].position[0] < -0.3f)
            {
                particles[i].lifetime -= dt * (-particles[i].position[0] - 0.3);

            }
            particles[i].position += glm::vec3(0.0f, dt * 2.0f, 0.0f);
            //particles[i].position[1] = sin(particles[i].position[1]);
        }


        positions[i * 4 + 0] = particles[i].position[0];
        positions[i * 4 + 1] = particles[i].position[1];
        positions[i * 4 + 2] = particles[i].position[2];
        positions[i * 4 + 3] = particles[i].lifetime;
    }
}

void Emitter::draw(Shader shader)
{

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * 4 * sizeof(float), positions);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    auto att_pos = glGetAttribLocation(shader.ID, "vertex_position");
    glEnableVertexAttribArray(att_pos);
    glVertexAttribPointer(att_pos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    auto att_pos2 = glGetAttribLocation(shader.ID, "position");
    glEnableVertexAttribArray(att_pos2);
    glVertexAttribPointer(att_pos2, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glVertexAttribDivisor(4, 1);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particles.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(4);

}

//Controller

/**
Controller::Controller()
{
    this->shader = new Shader();
    this->emitter = new SimpleEmitter();
}

Controller::DeleteController()
{
    delete this->shader();
    delete this->emitter();
}

void Controller::draw(const float dt, const mat4& M_p, const mat4& M_v)
{
    this->emitter->update(dt);
    this->shader->setUniformProjectionMatrix(M_p);
    this->shader->setUniformViewnMatrix(M_v);
    this->shader->setUniformParticleSize(0.1f);
    this->emitter->draw();
}
**/
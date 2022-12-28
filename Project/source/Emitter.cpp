#include "../headers/emitter.h"

Emitter::Emitter()
{
    this->particles.resize(100);

    for ( uint i = 0 ; i < this-> particles.size() ; ++i ) {
        this->particles[i].position = lstd_random_between(vec3(-1.0f)),(vec3(1.0f));
        this->particles[i].lifetime = lstd_random_between(1.0f,2.0f);
    }

    this->vertexBuffer = glGenBuffers();
    this->positionBuffer = glGenBuffers();

    std::vector<float> vertices;

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

    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float),vertices.data(),GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, this->particles.size()*4*sizeof(float), this->positions, GL_DYNAMIX_DRAW);
    
}

Emitter::deleteEmitter()
{
    glDeleteBuffer(this->vertexBuffer);
    glDeleteBuffer(this->positionBuffer);
}

void Emitter::update(const float dt)
{
    for (uint i=0; i < this.particles.size(); ++i)
    {
        this->particles[i].lifetime -= dt;

        if (this->particles[i].lifetime <= 0.0f )
        {
            this->particles[i].position = lstd_random_between(vec3(-1.0f)),(vec3(1.0f));
            this->particles[i].lifetime = lstd_random_between(1.0f,2.0f);
        }

        this->particles[i].position -= vec3(0.0f, dt*2.0f, 0.0f);

        this->positions[i*4+0] = this->particles[i].positions[0];
        this->positions[i*4+1] = this->particles[i].positions[1];
        this->positions[i*4+2] = this->particles[i].positions[2];
        this->positions[i*4+3] = this->particles[i].lifetime;
    }
}

void Emitter::draw()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, this->particles.size()*4*sizeof(float), this->positions);

    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, null_ptr);

    glBindBuffer(GL_ARRAY_BUFFER, this->positionsBuffer);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glVertexAttribPointer(4, 1);

    glDrawArraysInstanced(GL_TRAINGLES_STRIP, 0, 4, this->particles.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(4);
}




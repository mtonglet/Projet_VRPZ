#include "../headers/emitter.h"

//Emitter model & construction

Emitter::Emitter()
{
    particles.resize(100);

    for ( int i = 0 ; i < particles.size() ; ++i ) {
        particles[i].position = randomPos();
        particles[i].lifetime = randomLife();
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
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float),vertices.data(),GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, particles.size()*4*sizeof(float), positions, GL_DYNAMIC_DRAW);
    
}

glm::vec3 Emitter::randomPos()
{
    glm::vec3 randomPosition;
    srand(time(NULL));
    randomPosition.x = rand() % 10;
    randomPosition.y = 20;
    randomPosition.z = rand() % 10;

    return randomPosition;
}

float Emitter::randomLife()
{
    srand(time(NULL));
    float lifeTime = rand() % 2 + 3;
    return lifeTime;
}

void Emitter::update(const float dt)
{
    for (int i=0; i < particles.size(); ++i)
    {
        particles[i].lifetime -= dt;

        if (particles[i].lifetime <= 0.0f )
        {
            particles[i].position = randomPos();
            particles[i].lifetime = randomLife();
        }

        particles[i].position -= glm::vec3(0.0f, dt*2.0f, 0.0f);

        positions[i*4+0] = particles[i].position[0];
        positions[i*4+1] = particles[i].position[1];
        positions[i*4+2] = particles[i].position[2];
        positions[i*4+3] = particles[i].lifetime;
    }
}

void Emitter::draw(Shader shader)
{

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size()*4*sizeof(float), positions);

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


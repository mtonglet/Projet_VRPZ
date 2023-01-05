#ifndef ELEMENT_H
#define ELEMENT_H

#include <glad/glad.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include  "../headers/shader.h"
#include "../headers/object.h"

class Element
{
public:
    Element();
    Element(Object object);
    Element(char path[],Shader shader);

    Shader getShader();
    
    void initialize(float x, float y, float z,float scale);
    void display(glm::mat4 view, glm::mat4 projection);
    void updatePos(double time);
    void Element::move(glm::vec3 newPos, float scale);
    void Element::move(float newPosX, float newPosY, float newPosZ);

    glm::vec3 getPos();
    glm::mat4 getMatM();

private:
    bool moving;
    Shader shader;
    Object sourceObj;

    float posX,posY,posZ;
    glm::mat4 M, V, P, R;
    glm::mat4 invM;

};
#endif
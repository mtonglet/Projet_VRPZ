#include  "../headers/Element.h"


Element::Element()
{
	char path[] = PATH_TO_OBJECTS "/sphere_smooth.obj";
	shader = Shader();
	Element(path, shader);
}


Element::Element(Object object):Element()
{
	this->sourceObj = object;
}


Element::Element(char path[], Shader shader):Element(){
	this->sourceObj = Object(path);
	this->sourceObj.makeObject(shader);
}

Shader Element::getShader() {
	return this->shader;
}

void Element::initialize(float x = 0.0, float y = 1.0, float z = 0.0, float scale = 1.0) {
	glm::mat4 m = glm::mat4(1.0);
	m = glm::translate(m, glm::vec3(x, y, z));
	m = glm::scale(m, glm::vec3(scale, scale, scale));
	this->M = m;
	this->invM = glm::transpose(glm::inverse(m));
}


void Element::display(glm::mat4 view, glm::mat4 projection) {
	(this->shader).use();//delete if display() called from the shader
	(this->shader).setMatrix4("M",(this->M));
	(this->shader).setMatrix4("V", view);
	(this->shader).setMatrix4("P", projection);
	(this->shader).setMatrix4("R", glm::mat4(1.0));
	(this->sourceObj).draw();
}

void Element::updatePos(double time) {

}

void Element::move(glm::vec3 newPos) {

}

void Element::move(float newPosX, float newPosY, float newPosZ) {

}

glm::vec3 Element::getPos() {
	return glm::vec3(this->posX, this->posY, this->posZ);
}

glm::mat4 Element::getMatM() {
	return this->M;
}


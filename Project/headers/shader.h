#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>


class Shader
{
public:
	GLuint ID;

    Shader(const char* vertexPath, const char* fragmentPath);
	

    Shader(std::string vShaderCode, std::string fShaderCode);

    Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);

    void use();
    void setInteger(const GLchar* name, GLint value);
    void setFloat(const GLchar* name, GLfloat value);
    void setVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z);
    void setVector3f(const GLchar* name, const glm::vec3& value);
    void setMatrix4(const GLchar* name, const glm::mat4& matrix);
    void setTexUnit(const GLchar* name, GLuint unit);
    void setUniformParticleSize(const GLchar* name, const float particleSize);

private:
    GLuint compileShader(std::string shaderCode, GLenum shaderType);
    GLuint compileProgram(GLuint vertexShader, GLuint fragmentShader);
    GLuint compileProgram(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader);
    
};
#endif
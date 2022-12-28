
#include "../headers/shader.h"





Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
    GLuint fragment = compileShader(fragmentCode, GL_FRAGMENT_SHADER);
    ID = compileProgram(vertex, fragment);
} 

Shader::Shader():Shader("../shaders/classic.vert", "../shaders/classic.frag") {

}

//Constructor that takes the geometry shader into account.
Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        gShaderFile.open(geometryPath);
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        gShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        geometryCode = gShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
    GLuint geometry = compileShader(geometryCode, GL_GEOMETRY_SHADER);
    GLuint fragment = compileShader(fragmentCode, GL_FRAGMENT_SHADER);
    ID = compileProgram(vertex, geometry, fragment);
} 

Shader::Shader(std::string vShaderCode, std::string fShaderCode)
{
    GLuint vertex = compileShader(vShaderCode, GL_VERTEX_SHADER);
    GLuint fragment = compileShader(fShaderCode, GL_FRAGMENT_SHADER);
    ID = compileProgram(vertex, fragment);
} 

void Shader::use() {
    glUseProgram(ID);
}


//Unfinished
//void Shader::drawElements() {
//    this->use();
//    for (Element e : this->attachedElements) {
//        e.display();
//    }
//}
void Shader::setInteger(const GLchar* name, GLint value) {
    glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::setFloat(const GLchar* name, GLfloat value) {
    glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::setVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z) {
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void Shader::setVector3f(const GLchar* name, const glm::vec3& value) {
    glUniform3f(glGetUniformLocation(ID, name), value.x, value.y, value.z);
}


void Shader::setLightsParams(const int maxLightsNumber, const float(&params)[6]) {
    for (int i = 0; i < maxLightsNumber; i++) {
        std::string name = "lights[" + std::to_string(i) + "].";

        std::string param;
        param = name + "ambient_strength";
        this->setFloat(param.c_str(), params[0]);
        param = name + "diffuse_strength";
        this->setFloat(param.c_str(), params[1]);
        param = name + "specular_strength";
        this->setFloat(param.c_str(), params[2]);
        param = name + "constant";
        this->setFloat(param.c_str(), params[3]);
        param = name + "linear";
        this->setFloat(param.c_str(), params[4]);
        param = name + "quadratic";
        this->setFloat(param.c_str(), params[5]);
    }
}


void Shader::setLightsPos(const int nLights, std::vector<glm::vec3> &array) {
    this->setInteger("n_lights", nLights);
    
    for (int i = 0; i < nLights; i++) {
        std::string name = "lights["+std::to_string(i)+"].";
        
        std::string param;
        param = name + "light_pos";
        this->setVector3f(param.c_str(), array[i]);
    }   
}

void Shader::setMatrix4(const GLchar* name, const glm::mat4& matrix) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
} 

// Assigns a texture unit to a texture 
void Shader::setTexUnit(const GLchar* name, GLuint unit) {
    // Shader needs to be activated before changing the value of a uniform
    use();
    // Gets the location of the uniform
    GLuint texUni = glGetUniformLocation(ID, name);
    // Sets the value of the uniform
    glUniform1i(texUni, unit);

}
GLuint Shader::compileShader(std::string shaderCode, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    const char* code = shaderCode.c_str();
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    GLchar infoLog[1024];
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::string t = "undetermined";
        if (shaderType == GL_VERTEX_SHADER) {
            t = "vertex shader";
        }
        else if (shaderType == GL_GEOMETRY_SHADER) {
            t = "geometry shader";
        }
        else if (shaderType == GL_FRAGMENT_SHADER) {
            t = "fragment shader";
        }
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of the " << t << ": " << shaderType << infoLog << std::endl;
    }
    return shader;
}

GLuint Shader::compileProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint programID = glCreateProgram();

    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);


    GLchar infoLog[1024];
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programID, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR:  " << infoLog << std::endl;
    }
    return programID;
} 

GLuint Shader::compileProgram(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader)
{
    GLuint programID = glCreateProgram();

    glAttachShader(programID, vertexShader);
    glAttachShader(programID, geometryShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);


    GLchar infoLog[1024];
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programID, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR:  " << infoLog << std::endl;
    }
    return programID;
} 


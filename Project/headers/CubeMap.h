
#include<iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include "stb_image.h"


class CubeMap {
public:
    CubeMap(
        std::string sourceFile
    );
    CubeMap();

    ~CubeMap();

    void Bind(GLenum unit = 0);

    void operator=(const CubeMap& other) = delete;
    CubeMap(const CubeMap& other) = delete;
    GLuint m_texture;
private:
    
    std::string m_fileNames[6];
};
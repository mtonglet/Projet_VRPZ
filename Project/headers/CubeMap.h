
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
    GLuint ID;
    int reference = 0;
    CubeMap(
        std::string sourceFile
    );
    CubeMap(int width, int height, int ref);

    //~CubeMap();
    //void operator=(const CubeMap& other) = delete;
    //CubeMap(const CubeMap& other) = delete;
    void Bind(GLenum unit = 0);

    
    
    GLuint getID();
private:
    
    std::string m_fileNames[6];
};
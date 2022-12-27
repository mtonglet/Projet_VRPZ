

#include "../headers/cubeMap.h"

const int CUBE_TEXTURE_SIZE = 128;

static const GLenum GL_TEXTURE_CUBE_MAP_types[6] =
{
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

CubeMap::CubeMap(
    std::string sourceFile
)
    : ID(0)
{

    m_fileNames[0] = sourceFile + "posx.jpg";
    m_fileNames[1] = sourceFile + "negx.jpg";
    m_fileNames[2] = sourceFile + "posy.jpg";
    m_fileNames[3] = sourceFile + "negy.jpg";
    m_fileNames[4] = sourceFile + "posz.jpg";
    m_fileNames[5] = sourceFile + "negz.jpg";


    glGenTextures(1, &ID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(false);

    for (unsigned i = 0; i < 6; ++i) {  //(sizeof(GL_TEXTURE_CUBE_MAP_types) / sizeof(GL_TEXTURE_CUBE_MAP_types[0]))
        int w, h, bytesPerPixel;
        unsigned char* data = stbi_load((m_fileNames[i].c_str()), &w, &h, &bytesPerPixel, 0);

        if (data == nullptr) {
            std::cout << "CubeMap(Texture) Failed to load Texture from file"+ m_fileNames[i] << std::endl;
            const char* reason = stbi_failure_reason();
            std::cout << reason << std::endl;
            assert(0);
        }
        else {
            if (bytesPerPixel == 4)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_types[i], 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            else if (bytesPerPixel == 3)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_types[i], 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            else if (bytesPerPixel == 1)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_types[i], 0, GL_RGB, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);

            else
                throw std::invalid_argument("Automatic Texture type recognition failed");
        }
        stbi_image_free(data);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    std::cout << "CubeMap(Texture): Has init successfully : ID is"+ ID << std::endl;
}
CubeMap::CubeMap(int width, int height, int ref)
    : ID(0)
{   
    reference = ref;

    glGenTextures(1, &ID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

    std::vector<GLubyte> testData(width * height * 256, 128);
    std::vector<GLubyte> xData(width * height * 256, 255);

    for (int loop = 0; loop < 6; ++loop)
    {
        if (loop)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + loop, 0, GL_RGB,
                width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &testData[0]);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + loop, 0, GL_RGB,
                width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &xData[0]);
        }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    std::cout << "CubeMap(Texture): Has init successfully : ID is" + ID << std::endl;
}


/*CubeMap::~CubeMap() {
    if (ID != 0) {
        glDeleteTextures(1, &ID);
        std::cout << "CubeMap(Texture): Delete texture object successfully : ID was"+ ID << std::endl;
    }
}*/


void CubeMap::Bind(GLenum unit) {
    assert(unit >= 0 && unit <= GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS-GL_TEXTURE0-1);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

GLuint CubeMap::getID() {
    return ID;
}
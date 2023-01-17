#include<iostream>
#include<vector>
//include glad before GLFW to avoid header conflict or define "#define GLFW_INCLUDE_NONE"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>


#include <map>

#include "headers/Texture.h"
#include "headers/CubeMap.h"
#include "headers/FrameBuffer.h"
#include "headers/Emitter.h"
#include "headers/shader.h"
#include "headers/object.h"
#include "headers/Element.h"
#include "headers/camera.h"



const int DIR_SHADOW_MAPPING_RESOLUTION = 512 ;//4096 is a good value to prevent some rendering bugs
const int POINT_SHADOW_MAPPING_RESOLUTION = 512;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "headers/Ball.h"

GLuint currentTextSlot = 0;

const int width = 1024;
const int height = 1024;

int main(int argc, char* argv[]);

void processInput(GLFWwindow* window);

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

void renderShadowMapTest();


float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;
bool lampsActivated = true;
bool lampDecreasing = false;
bool inKeyA = false;
float fasterMoon = 0.0;
bool ballfall = true;

void loadCubemapFace(const char* file, const GLenum& targetCube);

#ifndef NDEBUG
void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}
#endif

Camera camera(glm::vec3(0.0, 0.5, 0.2));


int main(int argc, char* argv[])
{
	//Boilerplate
	//Create the OpenGL context 
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialise GLFW \n");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef NDEBUG
	//create a debug context to help with Debugging
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif


	//Create the window
	GLFWwindow* window = glfwCreateWindow(width, height, "Project VRPZ", nullptr, nullptr);
	//GLFWwindow* debugWindow = glfwCreateWindow(width/2, height/2, "DEBUG - Project VRPZ", nullptr, nullptr);

	if (window == NULL )//|| debugWindow == NULL)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window\n");
	}

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//load openGL function
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}

	glEnable(GL_DEPTH_TEST);

#ifndef NDEBUG
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif

	//shader files read, shader initialised , linked and compiled
	
	char pathSourceF[] = PATH_TO_SHADER "/reflective.frag";
	char pathSourceV[] = PATH_TO_SHADER "/reflective.vert";
	Shader shader(pathSourceV,pathSourceF);

	char pathBumpF[] = PATH_TO_SHADER "/textureBump.frag";
	char pathBumpG[] = PATH_TO_SHADER "/textureBump.geom";
	char pathBumpV[] = PATH_TO_SHADER "/textureBump.vert";
	Shader shaderBump(pathBumpV, pathBumpG, pathBumpF);

	char pathSourceFGND[] = PATH_TO_SHADER "/texture2D.frag";
	char pathSourceVGND[] = PATH_TO_SHADER "/texture2D.vert";
	Shader shaderGND(pathSourceVGND, pathSourceFGND);


	char pathCubemapF[] = PATH_TO_SHADER "/cubemap.frag";
	char pathCubemapV[] = PATH_TO_SHADER "/cubemap.vert";
	Shader cubeMapShader = Shader(pathCubemapV, pathCubemapF);

	char pathglassF[] = PATH_TO_SHADER "/glass.frag";
	char pathglassV[] = PATH_TO_SHADER "/glass.vert";
	Shader glassShader = Shader(pathglassV, pathglassF);

	char pathclassicF[] = PATH_TO_SHADER "/classic.frag";
	char pathclassicV[] = PATH_TO_SHADER "/classic.vert";
	Shader classicShader = Shader(pathclassicV, pathclassicF);

	char pathLightF[] = PATH_TO_SHADER "/textureLight.frag";
	char pathLightV[] = PATH_TO_SHADER "/textureLight.vert";
	Shader lightShader = Shader(pathLightV, pathLightF);

	char pathEmitterF[] = PATH_TO_SHADER "/emitter.frag";
	char pathEmitterV[] = PATH_TO_SHADER "/emitter.vert";
	Shader emitterShader = Shader(pathEmitterV, pathEmitterF);

	char pathShadowF[] = PATH_TO_SHADER "/shadowmap.frag";
	char pathShadowV[] = PATH_TO_SHADER "/shadowmap.vert";
	Shader shadowShader = Shader(pathShadowV, pathShadowF);

	char pathDebugShadowF[] = PATH_TO_SHADER "/debug_shadow.frag";
	char pathDebugShadowV[] = PATH_TO_SHADER "/debug_shadow.vert";
	Shader shadowDebugShader = Shader(pathDebugShadowV, pathDebugShadowF);
	
	char pathS[] = PATH_TO_OBJECTS "/sphere_smooth.obj";
	Object sphere(pathS);
	sphere.makeObject(shader);


//	Element elem_moon(pathS, lightShader);
	Object moon(pathS);
	moon.makeObject(lightShader);
	
	char pathB[] = PATH_TO_OBJECTS "/bunny_small.obj";
	Object bunny(pathB);
	bunny.makeObject(classicShader);

	Object bunnyText(pathB);
	bunnyText.makeObject(shaderGND);

	char pathCube[] = PATH_TO_OBJECTS "/cube.obj";
	Object cubeMap(pathCube);
	cubeMap.makeObject(cubeMapShader);

	char pathplane[] = PATH_TO_OBJECTS "/window.obj";
	Object mirror(pathplane);
	mirror.makeObject(glassShader);
	glm::vec3 mirrorPos = glm::vec3(9.0, 4.5, 0.0);
	glm::vec3 mirrorNorm = glm::vec3(1.0, 0.0, 0.0);

	

	char pathplaneH[] = PATH_TO_OBJECTS "/planeH.obj";
	Object ground(pathplaneH);
	ground.makeObject(lightShader,true);

	char pathRoom[] = PATH_TO_OBJECTS "/salon_wood.obj";
	Object room(pathRoom);
	room.makeObject(shaderBump);

	Object roomShadow(pathRoom);
	roomShadow.makeObject(shadowShader);
	
	char pathSapin[] = PATH_TO_OBJECTS "/sapin_maison.obj";
	Object sapin(pathSapin);
	sapin.makeObject(lightShader);

	char pathBoule1[] = PATH_TO_OBJECTS "/sphere_smooth.obj";
	Object boule1(pathBoule1);
	boule1.makeObject(lightShader);

	char pathBoule2[] = PATH_TO_OBJECTS "/sphere_smooth.obj";
	Object boule2(pathBoule2);
	boule2.makeObject(lightShader);

	char pathBoule3[] = PATH_TO_OBJECTS "/sphere_smooth.obj";
	Object boule3(pathBoule3);
	boule3.makeObject(lightShader);

	char pathBoule4[] = PATH_TO_OBJECTS "/sphere_smooth.obj";
	Object boule4(pathBoule4);
	boule4.makeObject(lightShader);

	char pathBoule5[] = PATH_TO_OBJECTS "/sphere_smooth.obj";
	Object boule5(pathBoule5);
	boule5.makeObject(lightShader);

	char pathBouletest[] = PATH_TO_OBJECTS "/sphere_smooth.obj";
	Object bouletest(pathBouletest);
	bouletest.makeObject(lightShader);

	char pathChaise[] = PATH_TO_OBJECTS "/chaise_salon.obj";
	Object chaise(pathChaise);
	chaise.makeObject(lightShader);

	char pathMeuble[] = PATH_TO_OBJECTS "/meuble.obj";
	Object meuble(pathMeuble);
	meuble.makeObject(lightShader);

	char pathPeinture[] = PATH_TO_OBJECTS "/peintureRPZ.obj";
	Object peinture(pathPeinture);
	peinture.makeObject(lightShader);

	char pathWoodFloor[] = PATH_TO_OBJECTS "/woodfloor.obj";
	Object woodfloor(pathWoodFloor);
	woodfloor.makeObject(lightShader);

	char pathWoodParvis[] = PATH_TO_OBJECTS "/woodparvis.obj";
	Object woodparvis(pathWoodParvis);
	woodparvis.makeObject(lightShader);

	char pathFirePlace[] = PATH_TO_OBJECTS "/fireplace.obj";
	Object fireplace(pathFirePlace);
	fireplace.makeObject(lightShader);

	char pathLampePlafond[] = PATH_TO_OBJECTS "/lampe_plafond.obj";
	Object lampeplafond(pathLampePlafond);
	lampeplafond.makeObject(lightShader);
	Object lampeplafond2(pathLampePlafond);
	lampeplafond2.makeObject(lightShader);
	Object lampeplafond3(pathLampePlafond);
	lampeplafond3.makeObject(lightShader);

	Emitter emitter = Emitter(1);
	Emitter emitter_fire = Emitter(2);
	Ball crazyball = Ball();

	double prev = 0;
	int deltaFrame = 0;
	//fps function
	auto fps = [&](double now) {
		double deltaTime = now - prev;
		deltaFrame++;
		if (deltaTime > 0.5) {
			prev = now;
			const double fpsCount = (double)deltaFrame / deltaTime;
			deltaFrame = 0;
			std::cout << "\r FPS: " << fpsCount;
			std::cout.flush();
		}
	};

	//--------------LIGHT PARAMS---------------//

	/*	Refraction indices :
	Air:      1.0	|	Water:    1.33	|
	Ice:      1.309	|	Glass:    1.52	|	Diamond:  2.42*/

	double moonSpeed = 0.1;
	double moonDist = 100.0;
	
	std::vector<glm::vec3> lights_positions = {//length must be less than MAX_LIGHTS_NUMBER defined in textureLight.frag
		glm::vec3(0.0,moonDist,0.0), //moon
		glm::vec3(0.0, 2.5, 7.0), //fire 
		glm::vec3(2.0, 8.2, 0.0), //moving in front of the paint
		glm::vec3(-2.0, 8.2, -2.0), //moving in front of the paint
		glm::vec3(-2.0, 8.2, 2.0) //moving in front of the paint

	};
	glm::vec3 defaultfirePos = lights_positions[1];
	const int lights_number = lights_positions.size();


	//							             amb   diff   spec  cst  linear  quadr
	const float default_lights_params[] = { 0.15f/(lights_number-1), 0.3f/(lights_number-1), 
											0.45f /(lights_number-1) , 0.8f, 0.0f, 0.0f };//for point lights
//	const float default_lights_params[] = { 0.05f, 0.45f, 0.4f , 1.0f, 0.0f, 0.0f };//for spot lights
	const float moon_light_params[] =     { 0.2f , 0.1f, 0.3f , 1.0f, 0.0f, 0.0f };//for directional lights
//	const float moon_light_params[] = { 0.0f , 0.0f, 0.0f , 0.0f, 0.0f, 0.0f };



	//------*-------BALLS POSITIONS------------------//
	std::vector<glm::vec3> ball_positions = {//length must be less than MAX_LIGHTS_NUMBER defined in textureLight.frag
		glm::vec3(-4.3, 2.0, -3.3),
		glm::vec3(-5.0, 3.5, -3.5),
		glm::vec3(-3.8, 4.8, -4.8),
		glm::vec3(3.0, 3.0, 3.0)
	};
	std::vector<glm::vec3> ball_color = {//length must be less than MAX_LIGHTS_NUMBER defined in textureLight.frag
		glm::vec3(0.8, 0.2, 0.2),
		glm::vec3(0.2, 1.0, 0.2),
		glm::vec3(1.0, 0.2, 1.0),
		glm::vec3(1.0, 0.2, 1.0)
	};
	int ball_number = ball_color.size();
	//---------------MODELS MATRICES------------------//

	glm::mat4 modelS = glm::mat4(1.0);
	glm::vec3 mirrorSpherePos = glm::vec3(5.0, 4.0, 0.0); // position of the sphere
	modelS = glm::translate(modelS, mirrorSpherePos); // position of the sphere
	modelS = glm::scale(modelS, glm::vec3(0.8, 0.8, 0.8));	
	glm::mat4 inverseModelS = glm::transpose(glm::inverse(modelS));

	glm::mat4 modelMoon = glm::mat4(1.0);
	modelMoon = glm::translate(modelMoon, lights_positions[0]);
	modelMoon = glm::scale(modelMoon, glm::vec3(2.5));
	modelMoon = glm::rotate(modelMoon, 3.14159265358979f,glm::vec3(0.0,1.0,0.0));
	glm::vec3 moonColor = 0.9f * glm::vec3(1.0,1.0,0.9);
	glm::mat4 inverseModelMoon = glm::transpose(glm::inverse(modelMoon));


	glm::mat4 modelBunny = glm::mat4(1.0);
	modelBunny = glm::translate(modelBunny, glm::vec3(5.0, 4.0, -20.0)); // position of the colored bunny
	modelBunny = glm::scale(modelBunny, glm::vec3(0.8, 0.8, 0.8));
	glm::mat4 inverseModelBunny = glm::transpose(glm::inverse(modelBunny));

	glm::mat4 modelBunnyText = glm::mat4(1.0);
	modelBunnyText = glm::translate(modelBunnyText, glm::vec3(-10.0, 4.0, 0.0)); // position of the sand-textured bunny
	modelBunnyText = glm::scale(modelBunnyText, glm::vec3(0.8, 0.8, 0.8));
	glm::mat4 inverseModelBunnyText = glm::transpose(glm::inverse(modelBunnyText));

	glm::mat4 modelBunnyText2 = glm::mat4(1.0);
	modelBunnyText2 = glm::translate(modelBunnyText2, glm::vec3(5.0, 4.0, 5.0)); // position of the bunny
	modelBunnyText2 = glm::scale(modelBunnyText2, glm::vec3(0.8, 0.8, 0.8));
	glm::mat4 inverseModelBunnyText2 = glm::transpose(glm::inverse(modelBunnyText2));

	glm::mat4 modelPlane = glm::mat4(1.0);
	modelPlane = glm::translate(modelPlane, mirrorPos );
	modelPlane = glm::scale(modelPlane, glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 inverseModelPlane = glm::transpose(glm::inverse(modelPlane));


	glm::mat4 modelSol = glm::mat4(1.0);
	modelSol = glm::translate(modelSol, glm::vec3(0.0, 0.01, 0.0));
	modelSol = glm::scale(modelSol, glm::vec3(100, 100, 100));
	glm::mat4 inverseModelSol = glm::transpose(glm::inverse(modelSol));//could be deleted since it is unused

	glm::mat4 modelRoom = glm::mat4(1.0);
	modelRoom = glm::translate(modelRoom, glm::vec3(0.0, 0.0, 0.0));
	modelRoom = glm::scale(modelRoom, glm::vec3(3.0, 3.0, 3.0));
	glm::mat4 inverseModelRoom = glm::transpose(glm::inverse(modelRoom));
	//std::cout << "room " << room.vertices[0].x << std::endl;

	glm::mat4 modelSapin = glm::mat4(1.0);				//Z	  //X  //Y
	modelSapin = glm::translate(modelSapin, glm::vec3(-5.0, 0.5, -5.0));
	modelSapin = glm::scale(modelSapin, glm::vec3(3.0, 3.0, 3.0));
	glm::mat4 inversemodelSapin = glm::transpose(glm::inverse(modelSapin));

	glm::mat4 modelBoule1 = glm::mat4(1.0);				//Z	  //X  //Y
	modelBoule1 = glm::translate(modelBoule1, glm::vec3(-3.2, 0.6, -3.5));
	modelBoule1 = glm::scale(modelBoule1, glm::vec3(0.3, 0.3, 0.3));
	glm::mat4 inversemodelBoule1 = glm::transpose(glm::inverse(modelBoule1));
	
	glm::mat4 modelBoule2 = glm::mat4(1.0);				//Z	  //X  //Y
	modelBoule2 = glm::translate(modelBoule2, glm::vec3(-6.5, 2.0, -3.5));
	modelBoule2 = glm::scale(modelBoule2, glm::vec3(0.34, 0.34, 0.34));
	glm::mat4 inversemodelBoule2 = glm::transpose(glm::inverse(modelBoule2));

	glm::mat4 modelBoule3 = glm::mat4(1.0);				//Z	  //X  //Y
	modelBoule3 = glm::translate(modelBoule3, glm::vec3(-4.0, 3.5, -3.5));
	modelBoule3 = glm::scale(modelBoule3, glm::vec3(0.34, 0.34, 0.34));
	glm::mat4 inversemodelBoule3 = glm::transpose(glm::inverse(modelBoule3));

	glm::mat4 modelBoule4 = glm::mat4(1.0);				//Z	  //X  //Y
	modelBoule4 = glm::translate(modelBoule4, glm::vec3(-6.2, 4.8, -4.8));
	modelBoule4 = glm::scale(modelBoule4, glm::vec3(0.34, 0.34, 0.34));
	glm::mat4 inversemodelBoule4 = glm::transpose(glm::inverse(modelBoule4));

	glm::mat4 modelBoule5 = glm::mat4(1.0);				//Z	  //X  //Y
	modelBoule5 = glm::translate(modelBoule5, glm::vec3(-5.0, 6.2, -4.1 ));
	modelBoule5 = glm::scale(modelBoule5, glm::vec3(0.26, 0.26, 0.26));
	glm::mat4 inversemodelBoule5 = glm::transpose(glm::inverse(modelBoule5));

	glm::mat4 modelBouletest = glm::mat4(1.0);				//Z	  //X  //Y
	modelBouletest = glm::translate(modelBouletest, glm::vec3(-5.0, 0.5, -1.1));
	modelBouletest = glm::scale(modelBouletest, glm::vec3(0.26, 0.26, 0.26));
	glm::mat4 inversemodelBouletest = glm::transpose(glm::inverse(modelBouletest));


	
	std::vector<glm::mat4> ballModel;				//Z	  //X  //Y
	std::vector<glm::mat4> ballModelInv;
	for (int i = 0; i < ball_number; i++) {
		glm::mat4 matrice = glm::mat4(1.0);
		matrice = glm::translate(matrice, ball_positions[i]);
		matrice = glm::scale(matrice, glm::vec3(0.35, 0.35, 0.35));
		glm::mat4 inverseMatrice = glm::transpose(glm::inverse(matrice));
		ballModel.push_back(matrice);
		ballModelInv.push_back(inverseMatrice);
	}
	
	glm::mat4 modelChaise = glm::mat4(1.0);				//Z	  //X  //Y
	modelChaise = glm::translate(modelChaise, glm::vec3(3.0, 0.0, -26.0));
	modelChaise = glm::scale(modelChaise, glm::vec3(4.0, 4.0, 4.0));
	//rotattion not working yet
	//modelChaise = glm::rotate(modelChaise, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 inversemodelChaise = glm::transpose(glm::inverse(modelChaise));

	glm::mat4 modelMeuble = glm::mat4(1.0);				//Z	  //X  //Y
	modelMeuble = glm::translate(modelMeuble, glm::vec3(1.0, 0.0, -7.0));
	modelMeuble = glm::scale(modelMeuble, glm::vec3(4.0, 4.0, 4.0));
	glm::mat4 inversemodelMeuble = glm::transpose(glm::inverse(modelMeuble));

	glm::mat4 modelPeinture = glm::mat4(1.0);				//Z	  //X  //Y
	modelPeinture = glm::translate(modelPeinture, glm::vec3(-9.0, -3.50, 0.0));
	modelPeinture = glm::scale(modelPeinture, glm::vec3(2.0, 2.0, 2.0));
	glm::mat4 inversemodelPeinture = glm::transpose(glm::inverse(modelPeinture));

	glm::mat4 modelWoodFloor = glm::mat4(1.0);				//Z	  //X  //Y
	modelWoodFloor = glm::translate(modelWoodFloor, glm::vec3(0.0, 0.05, 0.0));
	modelWoodFloor = glm::scale(modelWoodFloor, glm::vec3(3.0, 3.0, 3.0));
	glm::mat4 inversemodelWoodFloor = glm::transpose(glm::inverse(modelWoodFloor));

	glm::mat4 modelWoodParvis = glm::mat4(1.0);				//Z	  //X  //Y
	modelWoodParvis = glm::translate(modelWoodParvis, glm::vec3(0.0, 0.0, 0.0));
	modelWoodParvis = glm::scale(modelWoodParvis, glm::vec3(3.0, 3.0, 3.0));
	glm::mat4 inversemodelWoodParvis = glm::transpose(glm::inverse(modelWoodParvis));

	glm::mat4 modelLampePlafond = glm::mat4(1.0);				//Z	  //X  //Y
	modelLampePlafond = glm::translate(modelLampePlafond, glm::vec3(2.0, 0.0, 0.0));
	modelLampePlafond = glm::scale(modelLampePlafond, glm::vec3(3.0, 3.0, 3.0));
	glm::mat4 inversemodelLampePlafond = glm::transpose(glm::inverse(modelLampePlafond));

	glm::mat4 modelLampePlafond2 = glm::mat4(1.0);				//Z	  //X  //Y
	modelLampePlafond2 = glm::translate(modelLampePlafond2, glm::vec3(-2.0, 0.0, -2.0));
	modelLampePlafond2 = glm::scale(modelLampePlafond2, glm::vec3(3.0, 3.0, 3.0));
	glm::mat4 inversemodelLampePlafond2 = glm::transpose(glm::inverse(modelLampePlafond2));

	glm::mat4 modelLampePlafond3 = glm::mat4(1.0);				//Z	  //X  //Y
	modelLampePlafond3 = glm::translate(modelLampePlafond3, glm::vec3(-2.0, 0.0, 2.0));
	modelLampePlafond3 = glm::scale(modelLampePlafond3, glm::vec3(3.0, 3.0, 3.0));
	glm::mat4 inversemodelLampePlafond3 = glm::transpose(glm::inverse(modelLampePlafond3));

	glm::mat4 modelFirePlace = glm::mat4(1.0);				//Z	  //X  //Y
	modelFirePlace = glm::translate(modelFirePlace, glm::vec3(0.0, 0.0, 0.0));
	modelFirePlace = glm::scale(modelFirePlace, glm::vec3(3.0, 3.0, 3.0));
	glm::mat4 inversemodelFirePlace = glm::transpose(glm::inverse(modelFirePlace));

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 perspective = camera.GetProjectionMatrixCube(45.0f, 0.01f, 200.0f);

	glm::vec3 mirrorCenter = mirrorPos; 
	glm::mat4 reflection = camera.GetReflectionMatrix(mirrorCenter, mirrorNorm);

	glm::mat4 modelLamp = glm::mat4(1.0);

	glm::vec3 materialColour = glm::vec3(0.5f, 0.6, 0.8);

	//----------------SHADERS------------------//
	// 
	// /!\ dont .use() another shader before having put all uniforms on this one

	//BUMP
	const float light_params_bump[6] = {0.2,0.5,0.8,1.0,0.00014,0.0007};
	shaderBump.use();
	shaderBump.setFloat("shininess", 32.0f);
	//shaderBump.setVector3f("materialColour", materialColour);
	shaderBump.setLightsPosBump(lights_number, lights_positions);
	shaderBump.setLightsParamsBump(moon_light_params, "dir");
	shaderBump.setLightsParamsBump(default_lights_params, "point");
	shaderBump.setFloat("ratioTexture", 1.0f);

	shader.use();
	shader.setFloat("refractionIndice", 1.52);
	shader.setFloat("shininess", 32.0f);
	shader.setVector3f("materialColour", materialColour);
	shader.setFloat("light.ambient_strength", 0.1);
	shader.setFloat("light.diffuse_strength", 0.3);
	shader.setFloat("light.specular_strength", 0.2);
	shader.setFloat("light.constant", 1.0);
	shader.setFloat("light.linear", 0.14);
	shader.setFloat("light.quadratic", 0.07);

	//LIGHT
	lightShader.use();
	lightShader.setVector3f("emitted", glm::vec3(0.0));
	lightShader.setLightsParamsBump(default_lights_params,"point");
	lightShader.setLightsParamsBump(moon_light_params, "dir");
	lightShader.setLightsPosBump(lights_number,lights_positions);
	lightShader.setFloat("shininess", 32.0f);
	lightShader.setVector3f("materialColour", materialColour);

	//----------------TEXTURES------------------//

	char pathim[] = PATH_TO_TEXTURE "/text_snow.jpg";
	Texture GNDTex(pathim, "");
	
	char pathimG[] = PATH_TO_TEXTURE "/GroundTex.png";
	Texture GNDTexDirt(pathimG, "");
	 
	char pathimW[] = PATH_TO_TEXTURE "/wood.png";
	Texture roomTex(pathimW, "");
	 
	char pathimSapin[] = PATH_TO_TEXTURE "/sapinrep.jpg";
	Texture sapinTex(pathimSapin, "");

	char pathimBoule1[] = PATH_TO_TEXTURE "/red-xmas.jpg";
	Texture boule1Tex(pathimBoule1, "");

	char pathimBoule2[] = PATH_TO_TEXTURE "/red-glitter.jpg";
	Texture boule2Tex(pathimBoule2, "");

	char pathimBoule3[] = PATH_TO_TEXTURE "/gold-glitter.jpg";
	Texture boule3Tex(pathimBoule3, "");

	char pathimBoule4[] = PATH_TO_TEXTURE "/gold-star.jpg";
	Texture boule4Tex(pathimBoule4, "");

	char pathimBoule5[] = PATH_TO_TEXTURE "/red-xmas.jpg";
	Texture boule5Tex(pathimBoule5, "");

	char pathimBouletest[] = PATH_TO_TEXTURE "/red-glitter.jpg";
	Texture bouletestTex(pathimBouletest, "");

	char pathNormal[] = PATH_TO_TEXTURE "/woodBump.png";
	Texture normalMap(pathNormal, "normal");

	char pathimChaise[] = PATH_TO_TEXTURE "/texture_chaise.png";
	Texture chaiseTex(pathimChaise, "");

	char pathimMeuble[] = PATH_TO_TEXTURE "/texture_meuble.png";
	Texture meubleTex(pathimMeuble, "");

	char pathimPeinture[] = PATH_TO_TEXTURE "/texture_rpz.jpg";
	Texture peintureTex(pathimPeinture, "");

	char pathimWoodFloor[] = PATH_TO_TEXTURE "/texture_woodfloor.jpeg";
	Texture woodFloorTex(pathimWoodFloor, "");

	char pathimWoodParvis[] = PATH_TO_TEXTURE "/texture_woodparvis.jpeg";
	Texture woodParvisTex(pathimWoodParvis, "");

	char pathimLampePlafond[] = PATH_TO_TEXTURE "/text_lampe.jpg";
	Texture lampePlafondTex(pathimLampePlafond, "");

	char pathimFirePlace[] = PATH_TO_TEXTURE "/text_fireplace.jpg";
	Texture firePlaceTex(pathimFirePlace, "");
	
	char pathMoonTex[] = PATH_TO_TEXTURE "/moon_texture_hd_001.jpg";
	Texture moonTex(pathMoonTex, "");

	char pathWhiteTex[] = PATH_TO_TEXTURE "/white.png";
	Texture whiteTex(pathWhiteTex, "");

	std::string PathCM( PATH_TO_TEXTURE "/cubemaps/yokohama3/sky2_");
	//cube map creation 
	CubeMap skybox(PathCM);	

	//-----mirror---//
	// 
	//Frame buffer creation for mirror
	FrameBuffer framebufferMirror(width, height);



	//------------------SHADOWS------------------//
	
	//directional light
	ShadowFrameBuffer directionalFBShadow(1024, 1024); //4096 to avoid some rendering bugs outside

	//points lights (multiples -> not possible yet, limited to 1)
//	std::vector<ShadowFrameBuffer> pointFBShadow; 
//	pointFBShadow.push_back(ShadowFrameBuffer(1024, 1024, GL_TEXTURE_CUBE_MAP));
//	std::vector<Camera> pointLightsCams;
//	pointLightsCams.push_back(Camera(lights_positions[1]));
	std::vector<ShadowFrameBuffer> pointFBShadows;
	for (int i= 1; i < lights_number; i++) {
		pointFBShadows.push_back(ShadowFrameBuffer(1024, 1024, GL_TEXTURE_CUBE_MAP));
	}
	float farBackCubeShadMap; //INCREASE to avoid frustrum visibility outside

	char pathPoShadowF[] = PATH_TO_SHADER "/shadowCubeMap.frag";
	char pathPoShadowV[] = PATH_TO_SHADER "/shadowCubeMap.vert";
	char pathPoShadowG[] = PATH_TO_SHADER "/shadowCubeMap.geom";
	Shader cubeShadowShader = Shader(pathPoShadowV, pathPoShadowG, pathPoShadowF);

	//--------------MIRROR SPHERES--------------//

	//Framebuffer for cubemap
	std::vector<CubeMap> cubeReflection;
	for (int i = 1; i < ball_number; i++) {
		cubeReflection.push_back(CubeMap(1024, 1024, 1));
	}
	//CubeMap cubeRef = CubeMap(1024, 1024, 1);
	FrameBuffer framebufferCube(1024, 1024);

//	Camera cameraCube(glm::vec3(0.0, 4.0, -15.0));
	Camera cameraCube(ball_positions[0]);
	glm::mat4 projectionCube = cameraCube.GetProjectionMatrixCube(90.0f, 0.01f, 200.0f);

	glm::mat4 viewCube = cameraCube.GetViewCubeMatrix(0);

	//specify how we want the transparency to be computed (here ColorOut= Cfrag * alphaf + Cprev * (1-alphaf)  )
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);


	//------------------TIMING------------------//
	float lampEmissionIntensity = 0.999f;
	bool firstLoop = true;
	glfwSwapInterval(1);
	double init_now = glfwGetTime();
	double init_t = 0.0;
	float df = 0;
	if (fasterMoon > 0) {
		double init_t = glfwGetTime();
	}

	bool renderDebuggingWindow = false;
	//---------------------------------------//
	//-------------      W      -------------//
	//-------------      H      -------------//
	//-------------      I      -------------//
	//-------------      L      -------------//
	//-------------      E      -------------//
	//-------------             -------------//
	//-------------      |      -------------//
	//-------------      V      -------------//
	//---------------------------------------//
	
	while (!glfwWindowShouldClose(window)) {
		
		glfwMakeContextCurrent(window);
		processInput(window);
		view = camera.GetViewMatrix();
		glfwPollEvents();
		double now = glfwGetTime();

////////////////////////////////////////////////////////////////////////////////////////////
// Model handling :
		//decreasing lamp intensity
		if (lampsActivated) {
			lampEmissionIntensity = 1.0f;
		}
		else {
			lampDecreasing = true;
			lampEmissionIntensity *= 0.94f;
			if (std::max(lampEmissionIntensity,0.001f) == 0.001f) {
				lampDecreasing = false;
			}
		}
		glm::vec3 lampEmission = lampEmissionIntensity * glm::vec3(1.0f, 0.98f, 0.41f);

		//moving lights
//		lights_positions[0] = glm::vec3(90.0, 40.0f + 38.0f * std::sin(now), 36.0f); //TODO: uncomment to move the moon
		float vibration = 0.1 * std::sin(6*(now)) + 0.1*std::sin(15*(now)) + 0.1 * std::sin(2.4*(now)) +0.05 ;
			lights_positions[1] = defaultfirePos + glm::vec3(vibration, 0.0, 0.0);
		lights_positions[0] = glm::vec3(
			100.0f* std::sin(moonSpeed*(now - init_now)),
			100.0f* std::cos(moonSpeed*(now - init_now)),
			40.0f * std::sin(moonSpeed*(now - init_now)));

		modelMoon = glm::mat4(1.0);
		modelMoon = glm::translate(modelMoon, lights_positions[0]);//0
		modelMoon = glm::scale(modelMoon, glm::vec3(2.5));//2.5
		modelMoon = glm::rotate(modelMoon, 3.14159265358979f, glm::vec3(0.0, 1.0, 0.0));
		inverseModelMoon = glm::transpose(glm::inverse(modelMoon));

		//lights_positions[1] = glm::vec3(-7.0, 7.0, 7.0 *std::sin(now));

		std::vector<glm::mat4> modelsPhysicalLamps = {};
		for (int i = 2; i < lights_number; i++) {
			modelLamp = glm::mat4(1.0);
			modelLamp = glm::translate(modelLamp, lights_positions[i]);
			modelLamp = glm::scale(modelLamp, glm::vec3(0.25));
			modelsPhysicalLamps.push_back(modelLamp);
		}



////////////////////////////////////////////////////////////////////////////////////////////
//Zero pass: Draw scene and compute shadow maps
		directionalFBShadow.BindFB();

		//glm::vec3 pos = camera.Position; // lights_positions[0];
		float near = 50.0f;// moonDist + 15.0f;
		float far = 150.0f;// moonDist - 15.0f;
		glm::mat4 orthoProj = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, near, far);
		//		glm::mat4 orthoProj = camera.GetProjectionMatrix();
		//		glm::vec3 upDirLight = glm::normalize(glm::cross(glm::cross(-1.0f * lights_positions[0], glm::vec3(0.0f, 1.0f, 0.0f)), -1.0f * lights_positions[0]));
		glm::vec3 upDirLight = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 dirLightView = glm::lookAt(lights_positions[0], glm::vec3(0.0f, 0.0f, 0.0f), upDirLight);
		//		glm::mat4 dirLightView = glm::lookAt(lights_positions[0], glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0f, 1.0f, 0.0f));
		//		glm::mat4 dirLightView = glm::lookAt(lights_positions[0], glm::vec3(0.0), upDirLight);
				//glm::mat4 dirLightView = camera.GetViewMatrix();
		glm::mat4 dirLightProj = orthoProj * dirLightView;

		shadowShader.use();
		shadowShader.setMatrix4("light_proj", dirLightProj);

		shadowShader.setMatrix4("M", modelSapin);
		sapin.draw();
		shadowShader.setMatrix4("M", modelBoule1);
		boule1.draw();
		shadowShader.setMatrix4("M", modelBoule2);
		boule2.draw();
		shadowShader.setMatrix4("M", modelBoule3);
		boule3.draw();
		shadowShader.setMatrix4("M", modelBoule4);
		boule4.draw();
		shadowShader.setMatrix4("M", modelBoule5);
		boule5.draw();
		shadowShader.setMatrix4("M", modelBouletest);
		bouletest.draw();
		shadowShader.setMatrix4("M", modelSol);
		ground.draw();
		shadowShader.setMatrix4("M", modelChaise);
		chaise.draw();
		shadowShader.setMatrix4("M", modelMeuble);
		meuble.draw();
		shadowShader.setMatrix4("M", modelWoodFloor);
		woodfloor.draw();
		shadowShader.setMatrix4("M", modelWoodParvis);
		woodparvis.draw();
		shadowShader.setMatrix4("M", modelRoom);
		//room.draw(shadowShader,false,false);
		roomShadow.draw();
		shadowShader.setMatrix4("M", modelPeinture);
		peinture.draw();

		directionalFBShadow.Unbind(width, height);

		/*
		//DEBUGGING CODE to uncomment
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shadowDebugShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, directionalFBShadow.shadowMapping);
		renderShadowMapTest();
		//all that follows should be commented for visualization in OpenGL
		/**/

		if (true) {
			int lighs_number_to_compute = lights_number - 1;
			if (!firstLoop) {
				lighs_number_to_compute = 1;
			}
			farBackCubeShadMap = 30.0f;
			glm::mat4 cubeShadProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farBackCubeShadMap);
			cubeShadowShader.use();
			cubeShadowShader.setFloat("far_back_cube", farBackCubeShadMap);

			for (int i_pl = 0; i_pl < lighs_number_to_compute; i_pl++) {
				Camera camCubeShadow = Camera(lights_positions[i_pl + 1]);//0 is taken by the moon
				//	glm::mat4 P = camCubeShadow.GetProjectionMatrixCube(90.0f,2.0f,farBackCubeShadMap);

				pointFBShadows[i_pl].BindFB();
				cubeShadowShader.setVector3f("light_pos", lights_positions[i_pl + 1]);
				/*Uncomment to render shadows for moving light*/
				camCubeShadow = Camera(lights_positions[i_pl + 1]);
				for (int face = 0; face < 6; face++) {
					//send View*Proj matrice to Geometry shader for shadows cube maps
					std::string VPf = "VPshadows[" + std::to_string(face) + "]";
					cubeShadowShader.setMatrix4(VPf.c_str(), cubeShadProj * camCubeShadow.GetViewCubeMatrix(face));
				}
				cubeShadowShader.setMatrix4("M", modelSapin);
				sapin.draw();
				cubeShadowShader.setMatrix4("M", modelBoule1);
				boule1.draw();
				cubeShadowShader.setMatrix4("M", modelBoule2);
				boule2.draw();
				cubeShadowShader.setMatrix4("M", modelBoule3);
				boule3.draw();
				cubeShadowShader.setMatrix4("M", modelBoule4);
				boule4.draw();
				cubeShadowShader.setMatrix4("M", modelBoule5);
				boule5.draw();
				cubeShadowShader.setMatrix4("M", modelBouletest);
				bouletest.draw();
				cubeShadowShader.setMatrix4("M", modelSol);
				ground.draw();
				cubeShadowShader.setMatrix4("M", modelChaise);
				chaise.draw();
				cubeShadowShader.setMatrix4("M", modelMeuble);
				meuble.draw();
				cubeShadowShader.setMatrix4("M", modelPeinture);
				peinture.draw();
				cubeShadowShader.setMatrix4("M", modelWoodParvis);
				woodparvis.draw();
				cubeShadowShader.setMatrix4("M", modelRoom);
				room.draw();

				pointFBShadows[i_pl].Unbind(width, height);
			}
		}

		ball_number = 3;
////////////////////////////////////////////////////////////////////////////////////////////
// Pass 0 :  REFLECTIVE BALLS
		if (firstLoop) {
			//draw the cubeMap 
			framebufferCube.Bind(0);

			for (int itBall = 0; itBall < ball_number; itBall++) {
				for (int face = 0; face < 6; face++) {
					framebufferCube.attachCubeFace(cubeReflection[itBall].ID, face);

					glEnable(GL_DEPTH_TEST);
					//clear framebuffer contents
					glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					cameraCube.Position = ball_positions[itBall];
					projectionCube = cameraCube.GetProjectionMatrixCube(90.0f, 0.01f, 200.0f);
					viewCube = cameraCube.GetViewCubeMatrix(face);

					glDepthFunc(GL_LEQUAL);

					cubeMapShader.use();
					cubeMapShader.setTexUnit("cubemapSampler", 1);
					cubeMapShader.setMatrix4("V", viewCube);
					cubeMapShader.setMatrix4("P", projectionCube);
					skybox.Bind(1);
					cubeMap.draw();
					glDepthFunc(GL_LESS);

					//room (for objects without bump mapping)
					glEnable(GL_DEPTH_TEST);
					lightShader.use();

					//to VS
					lightShader.setMatrix4("R", glm::mat4(1.0));
					lightShader.setMatrix4("V", viewCube);
					lightShader.setMatrix4("P", projectionCube);
					lightShader.setMatrix4("itM", glm::mat4(1)); //should be modified with regards to R 
					lightShader.setMatrix4("dir_light_proj", dirLightProj);//shadows

					//to FS
					lightShader.setFloat("far_back_cube", farBackCubeShadMap);
					lightShader.setInteger("lampsActivated", lampsActivated);
					lightShader.setLightsPosBump(lights_number, lights_positions);
					lightShader.setFloat("shininess", 32.0f);
					lightShader.setVector3f("u_view_pos", camera.Position);
					lightShader.setVector3f("emitted", glm::vec3(0.0));//objects do not emit light by default
					//lightShader.setFloat("lampRefl", 0.2f*lampEmissionIntensity);
					//for(every shadow maps in every frame_buffer_shadow):
					lightShader.setTexUnit("shadow_map", 1);
					directionalFBShadow.BindTex(1);

					for (int pointLight = 0; pointLight < lights_number - 1; pointLight++) {
						std::string map = "shadow_cube_map[" + std::to_string(pointLight) + "]";
						lightShader.setTexUnit(map.c_str(), 2 + pointLight);
						pointFBShadows[pointLight].BindTex(2 + pointLight);
						//slots 0 & 1 already taken, from 2 for shadow maps
					}

					//Bind correct slot for textures...
					lightShader.setTexUnit("tex0", 1);
					//fix conflict of texture
					glActiveTexture(GL_TEXTURE0+1);
					glBindTexture(GL_TEXTURE_2D, 0);
					//... and draw all model matrices with textures
					lightShader.setMatrix4("M", modelSapin);
					sapinTex.Bind(1);
					sapin.draw();
					lightShader.setMatrix4("M", modelBoule1);
					boule1Tex.Bind(1);
					boule1.draw();
					lightShader.setMatrix4("M", modelBoule2);
					boule2Tex.Bind(1);
					boule2.draw();
					lightShader.setMatrix4("M", modelBoule3);
					boule3Tex.Bind(1);
					boule3.draw();
					lightShader.setMatrix4("M", modelBoule4);
					boule4Tex.Bind(1);
					boule4.draw();
					lightShader.setMatrix4("M", modelBoule5);
					boule5Tex.Bind(1);
					boule5.draw();
					lightShader.setMatrix4("M", modelChaise);
					chaiseTex.Bind(1);
					chaise.draw();
					lightShader.setMatrix4("M", modelMeuble);
					meubleTex.Bind(1);
					meuble.draw();
					lightShader.setMatrix4("M", modelPeinture);
					peintureTex.Bind(1);
					peinture.draw();
					lightShader.setMatrix4("M", modelFirePlace);
					firePlaceTex.Bind(1);
					fireplace.draw();
					lightShader.setMatrix4("M", modelLampePlafond);
					lampePlafondTex.Bind(1);
					lampeplafond.draw();
					lightShader.setMatrix4("M", modelLampePlafond2);
					lampePlafondTex.Bind(1);
					lampeplafond2.draw();
					lightShader.setMatrix4("M", modelLampePlafond3);
					lampePlafondTex.Bind(1);
					lampeplafond3.draw();
					//sol trop fonc�
					//lightShader.setFloat("lights[1].ambient_strength", ambient*1.8f);
					//lightShader.setFloat("lights[2].ambient_strength", ambient * 1.8f);
					lightShader.setMatrix4("M", modelSol);
					GNDTex.Bind(1);
					ground.draw();

					lightShader.setMatrix4("M", modelWoodFloor);
					woodFloorTex.Bind(1);
					woodfloor.draw();
					lightShader.setMatrix4("M", modelWoodParvis);
					woodParvisTex.Bind(1);
					woodparvis.draw();
					lightShader.setMatrix4("M", modelMoon);
					lightShader.setMatrix4("itM", inverseModelMoon);
					lightShader.setVector3f("emitted", 1.5f * moonColor);
					moonTex.Bind(1);
					moon.draw();

					whiteTex.Bind(1);

					lightShader.setVector3f("emitted", lampEmission * lampEmissionIntensity);

					for (glm::mat4 lamp : modelsPhysicalLamps) {
						lightShader.setMatrix4("M", lamp);
						//handle "itM"
						//lightShader.setMatrix4("itM", inverseLamp);
						sphere.draw(lightShader);
					}



					//room with bump mapping
					shaderBump.use();
					shaderBump.setTexUnit("tex0", 1);
					// Binds texture so that is appears in rendering to the right unit
					roomTex.Bind(1);
					shaderBump.setTexUnit("normal0", 2);
					// Binds texture so that is appears in rendering to the right unit
					normalMap.Bind(2);
					//To VS
					shaderBump.setVector3f("u_view_pos", camera.Position);
					shaderBump.setMatrix4("M", modelRoom);
					shaderBump.setMatrix4("R", glm::mat4(1.0));
					shaderBump.setMatrix4("V", viewCube);
					shaderBump.setMatrix4("P", projectionCube);
					shaderBump.setMatrix4("itM", glm::mat4(1.0));
					shaderBump.setMatrix4("dir_light_proj", dirLightProj);//shadows
					//putting  glm::mat4(1.0)) instead of dirLightProj above render bump maps.

					//To VG, VS & VF
					shaderBump.setLightsPosBump(lights_number, lights_positions);

					//To VF
					shaderBump.setFloat("far_back_cube", farBackCubeShadMap);
					shaderBump.setFloat("shininess", 32.0f);
					//lightParams already set
					shaderBump.setInteger("lampsActivated", lampsActivated);
					shaderBump.setTexUnit("shadow_map", 3);
					directionalFBShadow.BindTex(3);

					for (int pointLight = 0; pointLight < lights_number - 1; pointLight++) {
						std::string map = "shadow_cube_map[" + std::to_string(pointLight) + "]";
						shaderBump.setTexUnit(map.c_str(), 4 + pointLight);
						pointFBShadows[pointLight].BindTex(4 + pointLight);
						//slots 0 & 1 & 2 already taken, from 3 for shadow cube-maps
					}

					//shaderBump.setTexUnit("shadow_cube_map", 3);
					//pointFBShadows[0].BindTex(3);

					shaderBump.setFloat("ratioTexture", 2.0f);
					glActiveTexture(GL_TEXTURE0+1);
					glBindTexture(GL_TEXTURE_2D, 0);
					roomTex.Bind(1);
					room.draw(shaderBump);
					shaderBump.setFloat("ratioTexture", 1.0f);

				}
			}
			framebufferCube.Unbind();
		}


		
////////////////////////////////////////////////////////////////////////////////////////////
//First pass: Draw reversed Scene 
		bool useMirror = true;
		if (useMirror) {
			
			//bind the frambuffer for the reversed scene
			framebufferMirror.Bind(0);

			glEnable(GL_DEPTH_TEST);
			//clear framebuffer contents
			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			//refraction sphere
			shader.use();
			shader.setMatrix4("V", view);
			shader.setMatrix4("P", perspective);
			shader.setMatrix4("R", reflection);
			shader.setVector3f("u_view_pos", camera.Position);
			shader.setTexUnit("cubemapSampler", 0);
			float lightIntensity = 0.3f;
			if (lampsActivated) {
				lightIntensity = 0.9f;
			}

			for (int itBall = 0; itBall < ball_number; itBall++) {
				cubeReflection[itBall].Bind(0);
				shader.setVector3f("color", lightIntensity * ball_color[itBall]);
				shader.setMatrix4("M", reflection * ballModel[itBall]);
				shader.setMatrix4("itM", ballModelInv[itBall]);
				glDepthFunc(GL_LEQUAL);
				sphere.draw(shader);
			}

			glDepthFunc(GL_LESS);



			//room (for objects without bump mapping)
			glEnable(GL_DEPTH_TEST);
			lightShader.use();

			//to VS
			lightShader.setMatrix4("R", reflection);
			lightShader.setMatrix4("V", view);
			lightShader.setMatrix4("P", perspective);
			lightShader.setMatrix4("itM", glm::mat4(1)); //should be modified with regards to R 
			lightShader.setMatrix4("dir_light_proj", dirLightProj);//shadows

			//to FS
			lightShader.setFloat("far_back_cube", farBackCubeShadMap);
			lightShader.setInteger("lampsActivated", lampsActivated);
			lightShader.setLightsPosBump(lights_number, lights_positions);
			lightShader.setFloat("shininess", 32.0f);
			lightShader.setVector3f("u_view_pos", camera.Position);
			lightShader.setVector3f("emitted", glm::vec3(0.0));//objects do not emit light by default
			//lightShader.setFloat("lampRefl", 0.2f*lampEmissionIntensity);
			//for(every shadow maps in every frame_buffer_shadow):
			lightShader.setTexUnit("shadow_map", 1);
			directionalFBShadow.BindTex(1);

			for (int pointLight = 0; pointLight < lights_number - 1; pointLight++) {
				std::string map = "shadow_cube_map[" + std::to_string(pointLight) + "]";
				lightShader.setTexUnit(map.c_str(), 2 + pointLight);
				pointFBShadows[pointLight].BindTex(2 + pointLight);
				//slots 0 & 1 already taken, from 2 for shadow maps
			}

			//Bind correct slot for textures...
			lightShader.setTexUnit("tex0", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
			//... and draw all model matrices with textures
			lightShader.setMatrix4("M", modelSapin);
			sapinTex.Bind(0);
			sapin.draw();
			lightShader.setMatrix4("M", modelBoule1);
			boule1Tex.Bind(0);
			boule1.draw();
			lightShader.setMatrix4("M", modelBoule2);
			boule2Tex.Bind(0);
			boule2.draw();
			lightShader.setMatrix4("M", modelBoule3);
			boule3Tex.Bind(0);
			boule3.draw();
			lightShader.setMatrix4("M", modelBoule4);
			boule4Tex.Bind(0);
			boule4.draw();
			lightShader.setMatrix4("M", modelBoule5);
			boule5Tex.Bind(0);
			boule5.draw();
			lightShader.setMatrix4("M", modelChaise);
			chaiseTex.Bind(0);
			chaise.draw();
			lightShader.setMatrix4("M", modelMeuble);
			meubleTex.Bind(0);
			meuble.draw();
			lightShader.setMatrix4("M", modelPeinture);
			peintureTex.Bind(0);
			peinture.draw();
			lightShader.setMatrix4("M", modelFirePlace);
			firePlaceTex.Bind(0);
			fireplace.draw();
			lightShader.setMatrix4("M", modelLampePlafond);
			lampePlafondTex.Bind(0);
			lampeplafond.draw();
			lightShader.setMatrix4("M", modelLampePlafond2);
			lampePlafondTex.Bind(0);
			lampeplafond2.draw();
			lightShader.setMatrix4("M", modelLampePlafond3);
			lampePlafondTex.Bind(0);
			lampeplafond3.draw();
			//sol trop fonc�
			//lightShader.setFloat("lights[1].ambient_strength", ambient*1.8f);
			//lightShader.setFloat("lights[2].ambient_strength", ambient * 1.8f);

			lightShader.setMatrix4("M", modelWoodFloor);
			woodFloorTex.Bind(0);
			woodfloor.draw();
			

			whiteTex.Bind(0);

			lightShader.setVector3f("emitted", lampEmission * lampEmissionIntensity);

			for (glm::mat4 lamp : modelsPhysicalLamps) {
				lightShader.setMatrix4("M", lamp);
				//handle "itM"
				//lightShader.setMatrix4("itM", inverseLamp);
				sphere.draw(lightShader);
			}



			//room with bump mapping
			shaderBump.use();
			shaderBump.setTexUnit("tex0", 0);
			// Binds texture so that is appears in rendering to the right unit
			roomTex.Bind(0);
			shaderBump.setTexUnit("normal0", 1);
			// Binds texture so that is appears in rendering to the right unit
			normalMap.Bind(1);
			//To VS
			shaderBump.setVector3f("u_view_pos", camera.Position);
			shaderBump.setMatrix4("M", modelRoom);
			shaderBump.setMatrix4("R", reflection);
			shaderBump.setMatrix4("V", view);
			shaderBump.setMatrix4("P", perspective);
			shaderBump.setMatrix4("itM", glm::mat4(1.0));
			shaderBump.setMatrix4("dir_light_proj", dirLightProj);//shadows
			//putting  glm::mat4(1.0)) instead of dirLightProj above render bump maps.

			//To VG, VS & VF
			shaderBump.setLightsPosBump(lights_number, lights_positions);

			//To VF
			shaderBump.setFloat("far_back_cube", farBackCubeShadMap);
			shaderBump.setFloat("shininess", 32.0f);
			//lightParams already set
			shaderBump.setInteger("lampsActivated", lampsActivated);
			shaderBump.setTexUnit("shadow_map", 2);
			directionalFBShadow.BindTex(2);

			for (int pointLight = 0; pointLight < lights_number - 1; pointLight++) {
				std::string map = "shadow_cube_map[" + std::to_string(pointLight) + "]";
				shaderBump.setTexUnit(map.c_str(), 3 + pointLight);
				pointFBShadows[pointLight].BindTex(3 + pointLight);
				//slots 0 & 1 & 2 already taken, from 3 for shadow cube-maps
			}

			//shaderBump.setTexUnit("shadow_cube_map", 3);
			//pointFBShadows[0].BindTex(3);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
			shaderBump.setFloat("ratioTexture", 2.0f);
			roomTex.Bind(0);
			room.draw(shaderBump);
			shaderBump.setFloat("ratioTexture", 1.0f);


			//EMITTER
			

			emitterShader.use();
			emitterShader.setMatrix4("P", perspective);
			emitterShader.setMatrix4("V", view * reflection);
			emitterShader.setUniformParticleSize("particleSize", 0.2f);

			emitter_fire.draw(emitterShader);
			framebufferMirror.Unbind();
		}
		
////////////////////////////////////////////////////////////////////////////////////////////
// Second pass : draw normal scene + mirror with texture from 1st pass

		glEnable(GL_DEPTH_TEST);

		// clear all relevant buffers
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//refraction sphere
		shader.use();
		shader.setMatrix4("V", view);
		shader.setMatrix4("P", perspective);
		shader.setMatrix4("R", glm::mat4(1.0));
		shader.setVector3f("u_view_pos", camera.Position);
		shader.setTexUnit("cubemapSampler", 0);
		float lightIntensity = 0.3f;
		if (lampsActivated) {
			lightIntensity = 0.9f;
		}
		
		for (int itBall = 0; itBall < ball_number; itBall++) {
			cubeReflection[itBall].Bind(0);
			shader.setVector3f("color", lightIntensity * ball_color[itBall]);
			shader.setMatrix4("M", ballModel[itBall]);
			shader.setMatrix4("itM", ballModelInv[itBall]);
			glDepthFunc(GL_LEQUAL);
			sphere.draw(shader);
		}
		

		
		cubeMapShader.use();
		cubeMapShader.setMatrix4("V", view);
		cubeMapShader.setMatrix4("P", perspective);
		cubeMapShader.setTexUnit("cubemapSampler", 0);
		skybox.Bind(0);

		cubeMap.draw();
		glDepthFunc(GL_LESS);
		


		//room (for objects without bump mapping)
		glEnable(GL_DEPTH_TEST);
		lightShader.use();
		
		//to VS
		lightShader.setMatrix4("R", glm::mat4(1.0));
		lightShader.setMatrix4("V", view);
		lightShader.setMatrix4("P", perspective);
		lightShader.setMatrix4("itM", glm::mat4(1)); //should be modified with regards to R 
		lightShader.setMatrix4("dir_light_proj", dirLightProj);//shadows

		//to FS
		lightShader.setFloat("far_back_cube", farBackCubeShadMap);
		lightShader.setInteger("lampsActivated", lampsActivated);
		lightShader.setLightsPosBump(lights_number, lights_positions);
		lightShader.setFloat("shininess", 32.0f);
		lightShader.setVector3f("u_view_pos", camera.Position);
		lightShader.setVector3f("emitted", glm::vec3(0.0));//objects do not emit light by default
		//lightShader.setFloat("lampRefl", 0.2f*lampEmissionIntensity);
		//for(every shadow maps in every frame_buffer_shadow):
		lightShader.setTexUnit("shadow_map", 1);
		directionalFBShadow.BindTex(1);

		for (int pointLight = 0; pointLight < lights_number - 1; pointLight++) {
			std::string map = "shadow_cube_map[" + std::to_string(pointLight)+"]";
			lightShader.setTexUnit(map.c_str(),2 + pointLight);
			pointFBShadows[pointLight].BindTex(2 + pointLight); 
			//slots 0 & 1 already taken, from 2 for shadow maps
		}

		//Bind correct slot for textures...
		lightShader.setTexUnit("tex0", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		//... and draw all model matrices with textures
		lightShader.setMatrix4("M", modelSapin);
		sapinTex.Bind(0);
		sapin.draw();
		lightShader.setMatrix4("M", modelBoule1);
		boule1Tex.Bind(0);
		boule1.draw();
		lightShader.setMatrix4("M", modelBoule2);
		boule2Tex.Bind(0);
		boule2.draw();

		if (!ballfall) {
			crazyball.Stuck = ballfall;
			modelBoule3[3] = crazyball.Move(0.05, modelBoule3[3]);
			if (crazyball.Stuck) {
				ballfall = true;
			}
		}
		lightShader.setMatrix4("M", modelBoule3);
		boule3Tex.Bind(0);
		boule3.draw();
		lightShader.setMatrix4("M", modelBoule4);
		boule4Tex.Bind(0);
		boule4.draw();
		lightShader.setMatrix4("M", modelBoule5);
		boule5Tex.Bind(0);
		boule5.draw();
		lightShader.setMatrix4("M", modelBouletest);
		bouletestTex.Bind(0);
		bouletest.draw();
		lightShader.setMatrix4("M", modelChaise);
		chaiseTex.Bind(0);
		chaise.draw();
		lightShader.setMatrix4("M", modelMeuble);
		meubleTex.Bind(0);
		meuble.draw();
		lightShader.setMatrix4("M", modelPeinture);
		peintureTex.Bind(0);
		peinture.draw();
		lightShader.setMatrix4("M", modelFirePlace);
		firePlaceTex.Bind(0);
		fireplace.draw();
		lightShader.setMatrix4("M", modelLampePlafond);
		lampePlafondTex.Bind(0);
		lampeplafond.draw();
		lightShader.setMatrix4("M", modelLampePlafond2);
		lampePlafondTex.Bind(0);
		lampeplafond2.draw();
		lightShader.setMatrix4("M", modelLampePlafond3);
		lampePlafondTex.Bind(0);
		lampeplafond3.draw();
		//sol trop fonc�
		//lightShader.setFloat("lights[1].ambient_strength", ambient*1.8f);
		//lightShader.setFloat("lights[2].ambient_strength", ambient * 1.8f);
		lightShader.setMatrix4("M", modelSol);
		GNDTex.Bind(0);
		ground.draw();

		lightShader.setMatrix4("M", modelWoodFloor);
		woodFloorTex.Bind(0);
		woodfloor.draw();
		lightShader.setMatrix4("M", modelWoodParvis);
		woodParvisTex.Bind(0);
		woodparvis.draw();
		lightShader.setMatrix4("M", modelMoon);
		lightShader.setMatrix4("itM", inverseModelMoon);
		lightShader.setVector3f("emitted", 1.5f*moonColor);
		moonTex.Bind(0);
		moon.draw();

		whiteTex.Bind(0);

		lightShader.setVector3f("emitted", lampEmission*lampEmissionIntensity);

		for (glm::mat4 lamp : modelsPhysicalLamps) {
			lightShader.setMatrix4("M", lamp);
			//handle "itM"
			//lightShader.setMatrix4("itM", inverseLamp);
			sphere.draw(lightShader);
		}



		//room with bump mapping
		shaderBump.use();
		shaderBump.setTexUnit("tex0", 0);
		// Binds texture so that is appears in rendering to the right unit
		roomTex.Bind(0);
		shaderBump.setTexUnit("normal0", 1);
		// Binds texture so that is appears in rendering to the right unit
		normalMap.Bind(1);
		//To VS
		shaderBump.setVector3f("u_view_pos", camera.Position);
		shaderBump.setMatrix4("M", modelRoom);
		shaderBump.setMatrix4("R", glm::mat4(1.0));
		shaderBump.setMatrix4("V", view);
		shaderBump.setMatrix4("P", perspective);
		shaderBump.setMatrix4("itM", glm::mat4(1.0));
		shaderBump.setMatrix4("dir_light_proj",dirLightProj);//shadows
		//putting  glm::mat4(1.0)) instead of dirLightProj above render bump maps.

		//To VG, VS & VF
		shaderBump.setLightsPosBump(lights_number, lights_positions);

		//To VF
		shaderBump.setFloat("far_back_cube", farBackCubeShadMap);
		shaderBump.setFloat("shininess", 32.0f);
		//lightParams already set
		shaderBump.setInteger("lampsActivated", lampsActivated);
		shaderBump.setTexUnit("shadow_map", 2);
		directionalFBShadow.BindTex(2);
		
		for (int pointLight = 0; pointLight < lights_number - 1; pointLight++) {
			std::string map = "shadow_cube_map[" + std::to_string(pointLight) + "]";
			shaderBump.setTexUnit(map.c_str(), 3 + pointLight);
			pointFBShadows[pointLight].BindTex(3 + pointLight);
			//slots 0 & 1 & 2 already taken, from 3 for shadow cube-maps
		}
		
		//shaderBump.setTexUnit("shadow_cube_map", 3);
		//pointFBShadows[0].BindTex(3);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		shaderBump.setFloat("ratioTexture", 2.0f);
		roomTex.Bind(0);
		room.draw(shaderBump); 
		shaderBump.setFloat("ratioTexture", 1.0f);


		//EMITTER
		emitter.update(0.01f, 1);

		emitterShader.use();
		emitterShader.setMatrix4("P", perspective);
		emitterShader.setMatrix4("V", view);
		emitterShader.setUniformParticleSize("particleSize", 0.1f);

		emitter.draw(emitterShader);

		emitter_fire.update(0.04f, 2);

		emitterShader.use();
		emitterShader.setMatrix4("P", perspective);
		emitterShader.setMatrix4("V", view);
		emitterShader.setUniformParticleSize("particleSize", 0.2f);

		emitter_fire.draw(emitterShader);

	// now draw the mirror quad with screen texture
	// --------------------------------------------
		if (useMirror) {
			glassShader.use();

			glassShader.setMatrix4("M", modelRoom);
			glassShader.setMatrix4("V", view);
			glassShader.setMatrix4("P", perspective);

			//std::cout << framebufferMirror.unit << std::endl;
			glassShader.setInteger("screenTexture", 0);  // need to check why need of - 1 (depends on previously bounded textures) 

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, framebufferMirror.textureColorbufferID);


			//enable transparency 
			glEnable(GL_BLEND);
			mirror.draw(glassShader);
			glDisable(GL_BLEND);
		}

		
		


		glEnable(GL_DEPTH_TEST);// Enable the depth buffer*/
		fps(now);
		

		//FOR DEBUGGING
		glfwSwapBuffers(window);

		firstLoop = false; //UNCOMMENT/COMMENT

	}

	//clean up ressource
	
	framebufferMirror.Delete();
	
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


//could be sent to camera.cpp
void processInput(GLFWwindow* window) {
	//3. Use the cameras class to change the parameters of the camera
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(LEFT, 0.1);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(RIGHT, 0.1);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(FORWARD, 0.1);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(BACKWARD, 0.1);

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyboardRotation(1, 0.0, 1);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyboardRotation(-1, 0.0, 1);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboardRotation(0.0, 1.0, 1);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboardRotation(0.0, -1.0, 1);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.MovementSpeed = 4.0;}
	else {
		camera.MovementSpeed = 0.75;
	}

	//Pressing C (des)activate the camera folowing the mouse
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		camera.MouseSwitchActivation(false, window);}
	else {
		camera.MouseSwitchActivation(true, window);}
	
	//Pressing A (des)activate lighting inside the house
	if (inKeyA && glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
		lampsActivated = !lampsActivated;
		inKeyA = false;
		std::cout << "Lamp set " << lampsActivated << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		inKeyA = true;
	}

	//Pressing L accelerates the moon
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		fasterMoon = 0.3;
	}
	else {
		fasterMoon = 0.0;
	}

	//Pressing F leaves the ball fall
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		ballfall = false;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		ballfall = true;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		camera.resetPosition();
	}

}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}



unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderShadowMapTest()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

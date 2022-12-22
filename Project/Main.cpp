#include<iostream>

//include glad before GLFW to avoid header conflict or define "#define GLFW_INCLUDE_NONE"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>


#include <map>

#include "headers/Texture.h"
#include "headers/camera.h"
#include "headers/shader.h"
#include "headers/object.h"
#include "headers/FrameBuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint currentTextSlot = 0;

const int width = 1000;
const int height = 1000;


void processInput(GLFWwindow* window);

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;

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
	std::cout << "Welcome to exercice 10: " << std::endl;
	std::cout << "Implement refraction on an object\n"
		"\n";


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
	GLFWwindow* window = glfwCreateWindow(width, height, "Exercise 10", nullptr, nullptr);
	if (window == NULL)
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

	char pathSourceF[] = PATH_TO_SHADER "/source.frag";
	char pathSourceV[] = PATH_TO_SHADER "/source.vert";
	Shader shader(pathSourceV,pathSourceF);


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

	//char pathQuadShaderF[] = PATH_TO_SHADER "/quad.frag";
	//char pathQuadShaderV[] = PATH_TO_SHADER "/quad.vert";
	//Shader quadShader = Shader(pathQuadShaderV, pathQuadShaderF);

	char pathS[] = PATH_TO_OBJECTS "/sphere_smooth.obj";
	Object sphere1(pathS);
	sphere1.makeObject(shader);

	char pathB[] = PATH_TO_OBJECTS "/bunny_small.obj";
	Object bunny(pathB);
	bunny.makeObject(classicShader);

	char pathCube[] = PATH_TO_OBJECTS "/cube.obj";
	Object cubeMap(pathCube);
	cubeMap.makeObject(cubeMapShader);

	char pathplane[] = PATH_TO_OBJECTS "/planeYZ.obj";
	Object mirror(pathplane);
	mirror.makeObject(glassShader);
	glm::vec3 mirrorPos = glm::vec3(2.0, 2.0, -2.5);
	glm::vec3 mirrorNorm = glm::vec3(1.0, 0.0, 0.0);

	
	Object pinkmirror(pathplane);
	pinkmirror.makeObject(classicShader);
	

	//GND object 
	// First object!
	const float positionsData[] = {
		// vertices				//texture
		-1.0, 0.0, -1.0,		0.0, 0.0,// 1.0,
		 1.0, 0.0, -1.0,		100.0, 0.0,// 0.0,
		 -1.0,  0.0, 1.0,		0.0, 100.0, //0.0,

		 1.0, 0.0, 1.0,			100.0, 100.0,// 0.0,
		 1.0, 0.0, -1.0,		100.0, 0.0, //0.0,
		 -1.0,  0.0, 1.0,		0.0, 100.0 //0.0
	};


	//Create the buffer
	GLuint gndVBO, gndVAO;
	//generate the buffer and the vertex array
	glGenVertexArrays(1, &gndVAO);
	glGenBuffers(1, &gndVBO);

	//define VBO and VAO as active buffer and active vertex array
	glBindVertexArray(gndVAO);
	glBindBuffer(GL_ARRAY_BUFFER, gndVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positionsData), positionsData, GL_STATIC_DRAW);

	auto attribute1 = glGetAttribLocation(shaderGND.ID, "position");
	glEnableVertexAttribArray(attribute1);
	glVertexAttribPointer(attribute1, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)0);

	//5. VertexAttribPointer also read the texture coordinates
	auto att_tex1 = glGetAttribLocation(shaderGND.ID, "texcoord");
	glEnableVertexAttribArray(att_tex1);
	glVertexAttribPointer(att_tex1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	//desactive the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);




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


	glm::vec3 light_pos = glm::vec3(1.0, 2.0, 1.5);


	glm::mat4 modelS = glm::mat4(1.0);
	modelS = glm::translate(modelS, glm::vec3(0.0, 4.0, -10.0)); // position of the object
	modelS = glm::scale(modelS, glm::vec3(0.8, 0.8, 0.8));	
	glm::mat4 inverseModelS = glm::transpose(glm::inverse(modelS));


	glm::mat4 modelBunny = glm::mat4(1.0);
	modelBunny = glm::translate(modelBunny, glm::vec3(0.0, 4.0, -5.0)); // position of the object
	modelBunny = glm::scale(modelBunny, glm::vec3(0.8, 0.8, 0.8));
	glm::mat4 inverseModelBunny = glm::transpose(glm::inverse(modelBunny));


	
	glm::mat4 modelPlane = glm::mat4(1.0);
	modelPlane = glm::translate(modelPlane, mirrorPos );
	modelPlane = glm::scale(modelPlane, glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 inverseModelPlane = glm::transpose(glm::inverse(modelPlane));


	glm::mat4 modelSol = glm::mat4(1.0);
	modelSol = glm::translate(modelSol, glm::vec3(0.0, 0.0, 0.0));
	modelSol = glm::scale(modelSol, glm::vec3(100, 100, 100));
	glm::mat4 inverseModelSol = glm::transpose(glm::inverse(modelSol));


	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 perspective = camera.GetProjectionMatrix();

	glm::vec3 mirrorCenter = mirrorPos; 
	glm::mat4 reflection = camera.GetReflectionMatrix(mirrorCenter, mirrorNorm);

	float ambient = 0.1;
	float diffuse = 0.5;
	float specular = 0.8;

	glm::vec3 materialColour = glm::vec3(0.5f, 0.6, 0.8);

	//Rendering
	// /!\ dont .use() another shader before having put all uniforms on this one

	shader.use();
	shader.setFloat("shininess", 32.0f);
	shader.setVector3f("materialColour", materialColour);
	shader.setFloat("light.ambient_strength", ambient);
	shader.setFloat("light.diffuse_strength", diffuse);
	shader.setFloat("light.specular_strength", specular);
	shader.setFloat("light.constant", 1.0);
	shader.setFloat("light.linear", 0.14);
	shader.setFloat("light.quadratic", 0.07);
/*	Refraction indices :
	Air:      1.0	|	Water:    1.33	|
	Ice:      1.309	|	Glass:    1.52	|	Diamond:  2.42*/
	shader.setFloat("refractionIndice", 1.52);



	//Texture object generation for the ground 
	char pathim[] = PATH_TO_TEXTURE "/Sand.jpg";
	Texture GNDTex(pathim, GL_TEXTURE_2D, currentTextSlot);
	currentTextSlot = currentTextSlot + 1; // this is incremented at each new Texture

	//associate the texture to shader mirror
	GNDTex.texUnit(shaderGND, "tex0"); //the unit is the texture slot at instantiation if not said otherwize
	

	//cubemap texture 
	stbi_set_flip_vertically_on_load(false);
	GLuint cubeMapTexture;
	glGenTextures(1, &cubeMapTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

	// texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//stbi_set_flip_vertically_on_load(true);

	std::string pathToCubeMap = PATH_TO_TEXTURE "/cubemaps/yokohama3/";

	std::map<std::string, GLenum> facesToLoad = {
		{pathToCubeMap + "sky2.jpg",GL_TEXTURE_CUBE_MAP_POSITIVE_X},
		{pathToCubeMap + "sky2_1.jpg",GL_TEXTURE_CUBE_MAP_POSITIVE_Y},
		{pathToCubeMap + "sky2_2.jpg",GL_TEXTURE_CUBE_MAP_POSITIVE_Z},
		{pathToCubeMap + "sky2_3.jpg",GL_TEXTURE_CUBE_MAP_NEGATIVE_X},
		{pathToCubeMap + "sky2_4.jpg",GL_TEXTURE_CUBE_MAP_NEGATIVE_Y},
		{pathToCubeMap + "sky2_5.jpg",GL_TEXTURE_CUBE_MAP_NEGATIVE_Z},
	};
	//load the six faces
	for (std::pair<std::string, GLenum> pair : facesToLoad) {
		loadCubemapFace(pair.first.c_str(), pair.second);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	
	
	//Frame buffer creation for mirror
	FrameBuffer framebufferMirror(width, height);
	

	//specify how we want the transparency to be computed (here ColorOut= Cfrag * alphaf + Cprev * (1-alphaf)  )
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enables the Depth Buffer
	//glEnable(GL_DEPTH_TEST);
	

	glfwSwapInterval(1);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		view = camera.GetViewMatrix();
		glfwPollEvents();
		double now = glfwGetTime();
		//moving light
		auto delta = light_pos + glm::vec3(0.0, 0.0, 2 * std::sin(now));

		//bind the frambuffer for the reversed scene
		framebufferMirror.Bind();
		glEnable(GL_DEPTH_TEST);
		//clear framebuffer contents
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//First pass: Draw reversed Scene + pink mirror
		// 
		//draw bunny
		classicShader.use();

		classicShader.setMatrix4("M", modelBunny);
		classicShader.setMatrix4("V", view);
		classicShader.setMatrix4("P", perspective);
		classicShader.setMatrix4("R", reflection);

		bunny.draw();

		//refraction sphere
		shader.use();

		shader.setMatrix4("M", modelS);
		shader.setMatrix4("itM", inverseModelS);
		shader.setMatrix4("V", view);
		shader.setMatrix4("P", perspective);
		shader.setMatrix4("R", reflection);
		shader.setVector3f("u_view_pos", camera.Position);

		

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		shader.setInteger("cubemapTexture", 0);
		cubeMapShader.setInteger("cubemapTexture", 0);
	
		glDepthFunc(GL_LEQUAL);
		sphere1.draw();


		cubeMapShader.use();
		cubeMapShader.setMatrix4("V", view);
		cubeMapShader.setMatrix4("P", perspective);
		cubeMapShader.setInteger("cubemapTexture", 0);

		cubeMap.draw();
		glDepthFunc(GL_LESS);
		

		glBindVertexArray(gndVAO);
		//ground 
		shaderGND.use();

		shaderGND.setMatrix4("M", modelSol);
		shaderGND.setMatrix4("V", view);
		shaderGND.setMatrix4("P", perspective);
		
		// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
		GNDTex.texUnit(shaderGND, "tex0");
		// Binds texture so that is appears in rendering
		GNDTex.Bind();

		//glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		classicShader.use();

		classicShader.setMatrix4("M", modelPlane);
		classicShader.setMatrix4("V", view);
		classicShader.setMatrix4("P", perspective);
		classicShader.setMatrix4("R", glm::mat4(1.0));

		pinkmirror.draw();
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// Enables the Stencil Buffer
		/*

		glassShader.use();

		glassShader.setMatrix4("M", modelPlane);
		glassShader.setMatrix4("V", view);
		glassShader.setMatrix4("P", perspective);
		glassShader.setMatrix4("R", reflection);

		//enable transparency 
		glEnable(GL_BLEND);
		//glDisable(GL_DEPTH_TEST);
		mirror.draw();
		glDisable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST);
		*/

		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		framebufferMirror.Unbind();
		
		// clear all relevant buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Second pass : draw normal scene + mirror with texture from 1st pass
		// 
		//draw bunny
		
		classicShader.use();

		classicShader.setMatrix4("M", modelBunny);
		classicShader.setMatrix4("V", view);
		classicShader.setMatrix4("P", perspective);
		classicShader.setMatrix4("R", glm::mat4(1.0));

		bunny.draw();

		//refraction sphere
		shader.use();

		shader.setMatrix4("M", modelS);
		shader.setMatrix4("itM", inverseModelS);
		shader.setMatrix4("V", view);
		shader.setMatrix4("P", perspective);
		shader.setMatrix4("R", glm::mat4(1.0));
		shader.setVector3f("u_view_pos", camera.Position);



		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		shader.setInteger("cubemapTexture", 0);
		cubeMapShader.setInteger("cubemapTexture", 0);

		glDepthFunc(GL_LEQUAL);
		sphere1.draw();


		cubeMapShader.use();
		cubeMapShader.setMatrix4("V", view);
		cubeMapShader.setMatrix4("P", perspective);
		cubeMapShader.setInteger("cubemapTexture", 0);

		cubeMap.draw();
		glDepthFunc(GL_LESS);


		glBindVertexArray(gndVAO);
		//ground 
		shaderGND.use();

		shaderGND.setMatrix4("M", modelSol);
		shaderGND.setMatrix4("V", view);
		shaderGND.setMatrix4("P", perspective);

		// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
		GNDTex.texUnit(shaderGND, "tex0");
		// Binds texture so that is appears in rendering
		GNDTex.Bind();

		//glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// now draw the mirror quad with screen texture
	   // --------------------------------------------
		/*
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

		screenShader.use();
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
		glDrawArrays(GL_TRIANGLES, 0, 6);*/

		glassShader.use();

		glassShader.setMatrix4("M", modelPlane);
		glassShader.setMatrix4("V", view);
		glassShader.setMatrix4("P", perspective);
		glassShader.setInteger("screenTexture", 0);

		glBindVertexArray(mirror.VAO);
		glBindTexture(GL_TEXTURE_2D, framebufferMirror.textureColorbufferID);
		mirror.draw();

		//enable transparency 
		//glEnable(GL_BLEND);
		//glDisable(GL_DEPTH_TEST);
		//mirror.draw();
		//glDisable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST);

		// Enable the depth buffer
		glEnable(GL_DEPTH_TEST);
		fps(now);
		glfwSwapBuffers(window);
	}

	//clean up ressource
	glDeleteVertexArrays(1, &gndVAO);
	framebufferMirror.Delete();
	glDeleteBuffers(1, &gndVBO);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}




void loadCubemapFace(const char* path, const GLenum& targetFace)
{
	int imWidth, imHeight, imNrChannels;
	unsigned char* data = stbi_load(path, &imWidth, &imHeight, &imNrChannels, 0);
	if (data)
	{

		glTexImage2D(targetFace, 0, GL_RGB, imWidth, imHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(targetFace);
	}
	else {
		std::cout << "Failed to Load texture" << std::endl;
		const char* reason = stbi_failure_reason();
		std::cout << reason << std::endl;
	}
	stbi_image_free(data);
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

	//Pressing C (des)activate the camera folowing the mouse
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		camera.MouseSwitchActivation(false, window);}
	else {
		camera.MouseSwitchActivation(true, window);}}


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
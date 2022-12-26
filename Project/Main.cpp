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
#include "headers/CubeMap.h"

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

	char pathSourceF[] = PATH_TO_SHADER "/reflective.frag";
	char pathSourceV[] = PATH_TO_SHADER "/reflective.vert";
	Shader shader(pathSourceV,pathSourceF);

	char pathBumpF[] = PATH_TO_SHADER "/textureBump.frag";
	char pathBumpV[] = PATH_TO_SHADER "/textureBump.vert";
	Shader shaderBump(pathBumpV, pathBumpF);

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


	char pathS[] = PATH_TO_OBJECTS "/sphere_smooth.obj";
	Object sphere1(pathS);
	sphere1.makeObject(shader);

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
	ground.makeObject(shaderGND,true);

	char pathRoom[] = PATH_TO_OBJECTS "/salon_wood.obj";
	Object room(pathRoom);
	room.makeObject(shaderBump);
	
	char pathSapin[] = PATH_TO_OBJECTS "/sapin_maison.obj";
	Object sapin(pathSapin);
	sapin.makeObject(shaderBump);


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


	glm::vec3 light_pos = glm::vec3(2.0, 3.0, 2.0);


	glm::mat4 modelS = glm::mat4(1.0);
	modelS = glm::translate(modelS, glm::vec3(0.0, 4.0, -10.0)); // position of the object
	modelS = glm::scale(modelS, glm::vec3(0.8, 0.8, 0.8));	
	glm::mat4 inverseModelS = glm::transpose(glm::inverse(modelS));


	glm::mat4 modelBunny = glm::mat4(1.0);
	modelBunny = glm::translate(modelBunny, glm::vec3(0.0, 4.0, -5.0)); // position of the object
	modelBunny = glm::scale(modelBunny, glm::vec3(0.8, 0.8, 0.8));
	glm::mat4 inverseModelBunny = glm::transpose(glm::inverse(modelBunny));

	glm::mat4 modelBunnyText = glm::mat4(1.0);
	modelBunnyText = glm::translate(modelBunnyText, glm::vec3(-10.0, 4.0, 0.0)); // position of the object
	modelBunnyText = glm::scale(modelBunnyText, glm::vec3(0.8, 0.8, 0.8));
	glm::mat4 inverseModelBunnyText = glm::transpose(glm::inverse(modelBunnyText));

	glm::mat4 modelBunnyText2 = glm::mat4(1.0);
	modelBunnyText2 = glm::translate(modelBunnyText2, glm::vec3(5.0, 4.0, 5.0)); // position of the object
	modelBunnyText2 = glm::scale(modelBunnyText2, glm::vec3(0.8, 0.8, 0.8));
	glm::mat4 inverseModelBunnyText2 = glm::transpose(glm::inverse(modelBunnyText2));

	
	glm::mat4 modelPlane = glm::mat4(1.0);
	modelPlane = glm::translate(modelPlane, mirrorPos );
	modelPlane = glm::scale(modelPlane, glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 inverseModelPlane = glm::transpose(glm::inverse(modelPlane));


	glm::mat4 modelSol = glm::mat4(1.0);
	modelSol = glm::translate(modelSol, glm::vec3(0.0, 0.0, 0.0));
	modelSol = glm::scale(modelSol, glm::vec3(100, 100, 100));
	glm::mat4 inverseModelSol = glm::transpose(glm::inverse(modelSol));

	glm::mat4 modelRoom = glm::mat4(1.0);
	modelRoom = glm::translate(modelRoom, glm::vec3(0.0, 0.0, 0.0));
	modelRoom = glm::scale(modelRoom, glm::vec3(3.0, 3.0, 3.0));
	glm::mat4 inverseModelRoom = glm::transpose(glm::inverse(modelRoom));

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 perspective = camera.GetProjectionMatrix();

	glm::vec3 mirrorCenter = mirrorPos; 
	glm::mat4 reflection = camera.GetReflectionMatrix(mirrorCenter, mirrorNorm);

	float ambient = 0.2;
	float diffuse = 0.5;
	float specular = 0.8;

	glm::vec3 materialColour = glm::vec3(0.5f, 0.6, 0.8);

	//Rendering
	// /!\ dont .use() another shader before having put all uniforms on this one

	shaderBump.use();
	shaderBump.setFloat("shininess", 32.0f);
	shaderBump.setVector3f("materialColour", materialColour);
	shaderBump.setFloat("light.ambient_strength", ambient);
	shaderBump.setFloat("light.diffuse_strength", diffuse);
	shaderBump.setFloat("light.specular_strength", specular);
	shaderBump.setFloat("light.constant", 1.0);
	shaderBump.setFloat("light.linear", 0.14);
	shaderBump.setFloat("light.quadratic", 0.07);
/*	Refraction indices :
	Air:      1.0	|	Water:    1.33	|
	Ice:      1.309	|	Glass:    1.52	|	Diamond:  2.42*/
	shader.setFloat("refractionIndice", 1.52);



	//Texture objects generation
	char pathim[] = PATH_TO_TEXTURE "/Sand.jpg";
	Texture GNDTex(pathim, GL_TEXTURE_2D);
	
	char pathimG[] = PATH_TO_TEXTURE "/GroundTex.png";
	Texture GNDTexDirt(pathimG, GL_TEXTURE_2D);
	 
	char pathimW[] = PATH_TO_TEXTURE "/wood.png";
	Texture roomTex(pathimW, GL_TEXTURE_2D);
	 
	char pathimSapin[] = PATH_TO_TEXTURE "/sapinrep.jpg";
	Texture sapinTex(pathimSapin, GL_TEXTURE_2D);

	
	//Texture object generation for the ground 
	//char pathimWB[] = PATH_TO_TEXTURE "/woodBump.png";
	//Texture roomNorm(pathimWB, GL_TEXTURE_2D);

	
	
	
	std::string PathCM( PATH_TO_TEXTURE "/cubemaps/yokohama3/sky2_");
	//cube map creation 
	CubeMap skybox(PathCM);
	
	//Frame buffer creation for mirror
	FrameBuffer framebufferMirror(width, height);

	//Framebuffer for cubemap
	FrameBuffer framebufferCube(128, 128);


	//specify how we want the transparency to be computed (here ColorOut= Cfrag * alphaf + Cprev * (1-alphaf)  )
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
	

	glfwSwapInterval(1);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		view = camera.GetViewMatrix();
		glfwPollEvents();
		double now = glfwGetTime();
		//moving light
		auto delta = light_pos + glm::vec3(0.0, 0.0, 2 * std::sin(now));

		//draw the cube


		//bind the frambuffer for the reversed scene
		framebufferMirror.Bind(0);
		glEnable(GL_DEPTH_TEST);
		//clear framebuffer contents
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//First pass: Draw reversed Scene 
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

		

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		skybox.Bind(0);
		shader.setTexUnit("cubemapTexture", 0);
		cubeMapShader.setTexUnit("cubemapTexture", 0);
	
		glDepthFunc(GL_LEQUAL);
		sphere1.draw();


		cubeMapShader.use();
		cubeMapShader.setMatrix4("V", view);
		cubeMapShader.setMatrix4("P", perspective);
		cubeMapShader.setTexUnit("cubemapTexture", 0);

		cubeMap.draw();
		glDepthFunc(GL_LESS);
		

		
		//ground 
		shaderGND.use();

		shaderGND.setMatrix4("M", modelSol);
		shaderGND.setMatrix4("V", view);
		shaderGND.setMatrix4("P", perspective);
		
		// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
		shaderGND.setTexUnit("tex0", 0);
		// Binds texture so that is appears in rendering to the right unit
		GNDTex.Bind(0);

		ground.draw();

		//room with bump mapping
		shaderBump.use();
		shaderBump.setMatrix4("M", modelRoom);
		shaderBump.setMatrix4("itM", inverseModelRoom); //should be modified with regards to R 
		shaderBump.setMatrix4("R", reflection);
		shaderBump.setMatrix4("V", view);
		shaderBump.setMatrix4("P", perspective);
		shaderBump.setVector3f("u_view_pos", camera.Position);
		shaderBump.setVector3f("light.light_pos", delta);

		// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
		shaderBump.setTexUnit("tex0", 0);
		// Binds texture so that is appears in rendering to the right unit
		roomTex.Bind(0);

		room.draw();

		// Binds texture so that is appears in rendering to the right unit
		sapinTex.Bind(0);

		sapin.draw();
		
		
		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		framebufferMirror.Unbind();
		
		// clear all relevant buffers
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
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


		skybox.Bind(0);
		shader.setTexUnit("cubemapTexture", 0);
		cubeMapShader.setTexUnit("cubemapTexture", 0);

		glDepthFunc(GL_LEQUAL);
		sphere1.draw();

		cubeMapShader.use();
		cubeMapShader.setMatrix4("V", view);
		cubeMapShader.setMatrix4("P", perspective);
		cubeMapShader.setTexUnit("cubemapTexture", 0);

		cubeMap.draw();
		glDepthFunc(GL_LESS);

		//ground 
		shaderGND.use();

		shaderGND.setMatrix4("M", modelBunnyText);
		shaderGND.setMatrix4("V", view);
		shaderGND.setMatrix4("P", perspective);

		// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
		shaderGND.setTexUnit("tex0", 0);
		// Binds texture so that is appears in rendering to the right unit
		GNDTex.Bind(0);

		bunnyText.draw();

		shaderGND.setMatrix4("M", modelSol);

		ground.draw();
		
		// Binds texture so that is appears in rendering
		GNDTexDirt.Bind(0);

		shaderGND.setMatrix4("M", modelBunnyText2);

		bunnyText.draw(); //same object with different texture and model uniforms


		//room with bump mapping
		shaderBump.use();
		shaderBump.setMatrix4("M", modelRoom);
		shaderBump.setMatrix4("itM", inverseModelRoom);
		shaderBump.setMatrix4("R", glm::mat4(1.0));
		shaderBump.setMatrix4("V", view);
		shaderBump.setMatrix4("P", perspective);
		shaderBump.setVector3f("u_view_pos", camera.Position);
		shaderBump.setVector3f("light.light_pos", delta);

		// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
		shaderBump.setTexUnit("tex0", 0);
		// Binds texture so that is appears in rendering to the right unit
		roomTex.Bind(0);

		room.draw();

		// Binds texture so that is appears in rendering to the right unit
		sapinTex.Bind(0);

		sapin.draw();

		// now draw the mirror quad with screen texture
	    // --------------------------------------------

		glassShader.use();

		glassShader.setMatrix4("M", modelRoom);
		glassShader.setMatrix4("V", view);
		glassShader.setMatrix4("P", perspective);

		//std::cout << framebufferMirror.unit << std::endl;
		glassShader.setInteger("screenTexture", 0);  // need to check why need of - 1 (depends on previously bounded textures) 

		
		glBindTexture(GL_TEXTURE_2D, framebufferMirror.textureColorbufferID);
		

		//enable transparency 
		glEnable(GL_BLEND);
		mirror.draw();
		glDisable(GL_BLEND);
		

		// Enable the depth buffer
		glEnable(GL_DEPTH_TEST);
		fps(now);
		glfwSwapBuffers(window);
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
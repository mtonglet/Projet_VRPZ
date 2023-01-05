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
#include "headers/camera.h"
#include "headers/CubeMap.h"
#include "headers/FrameBuffer.h"
#include "headers/Emitter.h"
#include "headers/shader.h"
#include "headers/object.h"
#include "headers/Element.h"



#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint currentTextSlot = 0;

const int width = 1024;
const int height = 1024;

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

	char pathS[] = PATH_TO_OBJECTS "/sphere_smooth.obj";
	Object sphere1(pathS);
	sphere1.makeObject(shader);


//	Element elem_moon(pathS, lightShader);
	Object moon(pathS);
	moon.makeObject(classicShader);
	
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
	
	char pathSapin[] = PATH_TO_OBJECTS "/sapin_maison.obj";
	Object sapin(pathSapin);
	sapin.makeObject(lightShader);

	Emitter emitter;

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

	glm::vec3 light_pos = glm::vec3(-7.0, 3.0, 2.0);
	//LIGHTS PARAMETERS
	//must be less than MAX_LIGHTS_NUMBER defined in textureLight.frag
	std::vector<glm::vec3> lights_positions = {
		light_pos,
		glm::vec3(7.5, 2.0, 7.5),
		glm::vec3(0.0,50.0,0.0) //moon
	};
	std::vector<glm::vec3> deltas_lighpos;
	const int lights_number = lights_positions.size();

	float ambient = 0.3;
	float diffuse = 0.5;
	float specular = 0.8;
	const float lights_params[] = { 0.0, diffuse, specular, 1.0, 0.00014, 0.0007 };

	glm::mat4 modelS = glm::mat4(1.0);
//	glm::vec3 mirrorSpherePos = glm::vec3(0.0, 4.0, -15.0);
	glm::vec3 mirrorSpherePos = glm::vec3(5.0, 4.0, 0.0); // position of the sphere
	modelS = glm::translate(modelS, mirrorSpherePos); // position of the sphere
	modelS = glm::scale(modelS, glm::vec3(0.8, 0.8, 0.8));	
	glm::mat4 inverseModelS = glm::transpose(glm::inverse(modelS));

	glm::mat4 modelMoon = glm::mat4(1.0);
	modelMoon = glm::translate(modelMoon, glm::vec3(0.0, 50.0, 0.0));
	modelMoon = glm::scale(modelMoon, glm::vec3(2.0, 2.0, 2.0));
//	elem_moon.initialize(0.0,50.0,0.0,2.0);

	glm::mat4 modelBunny = glm::mat4(1.0);
	modelBunny = glm::translate(modelBunny, glm::vec3(5.0, 4.0, -20.0)); // position of the colored bunny
//	modelBunny = glm::translate(modelBunny, glm::vec3(20.0, 4.0, 5.0)); // position of the colored bunny
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
	modelSol = glm::translate(modelSol, glm::vec3(0.0, 0.0, 0.0));
	modelSol = glm::scale(modelSol, glm::vec3(100, 100, 100));
	glm::mat4 inverseModelSol = glm::transpose(glm::inverse(modelSol));//could be deleted since it is unused

	glm::mat4 modelRoom = glm::mat4(1.0);
	modelRoom = glm::translate(modelRoom, glm::vec3(0.0, 0.0, 0.0));
	modelRoom = glm::scale(modelRoom, glm::vec3(3.0, 3.0, 3.0));
	glm::mat4 inverseModelRoom = glm::transpose(glm::inverse(modelRoom));

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 perspective = camera.GetProjectionMatrixCube(45.0f, 0.01f, 100.0f);

	glm::vec3 mirrorCenter = mirrorPos; 
	glm::mat4 reflection = camera.GetReflectionMatrix(mirrorCenter, mirrorNorm);


	glm::vec3 materialColour = glm::vec3(0.5f, 0.6, 0.8);

	//Rendering
	// /!\ dont .use() another shader before having put all uniforms on this one

	//BUMP
	shaderBump.use();
	shaderBump.setFloat("shininess", 32.0f);
	shaderBump.setVector3f("materialColour", materialColour);
	shaderBump.setFloat("light.ambient_strength", ambient);
	shaderBump.setFloat("light.diffuse_strength", diffuse);
	shaderBump.setFloat("light.specular_strength", specular);
	shaderBump.setFloat("light.constant", 1.0);
	shaderBump.setFloat("light.linear", 0.00014);
	shaderBump.setFloat("light.quadratic", 0.0007);

	/*	Refraction indices :
	Air:      1.0	|	Water:    1.33	|
	Ice:      1.309	|	Glass:    1.52	|	Diamond:  2.42*/
	shader.use();
	shader.setFloat("refractionIndice", 1.52);
	shader.setFloat("shininess", 32.0f);
	shader.setVector3f("materialColour", materialColour);
	shader.setFloat("light.ambient_strength", ambient);
	shader.setFloat("light.diffuse_strength", diffuse);
	shader.setFloat("light.specular_strength", specular);
	shader.setFloat("light.constant", 1.0);
	shader.setFloat("light.linear", 0.14);
	shader.setFloat("light.quadratic", 0.07);

	//LIGHT
	lightShader.use();
	lightShader.setFloat("Ambient", ambient);
	lightShader.setVector3f("Emitted", glm::vec3(0.0));
	lightShader.setLightsParams(10, lights_params);
	lightShader.setLightsPos(lights_number,lights_positions);
	lightShader.setFloat("shininess", 32.0f);
	lightShader.setVector3f("materialColour", materialColour);
	lightShader.setFloat("lights[2].constant", 1.0);
	lightShader.setFloat("lights[2].linear", 0.0);
	lightShader.setFloat("lights[2].quadratic", 0.0);
	lightShader.setFloat("lights[2].ambient_strength", 0.15);
	lightShader.setFloat("lights[2].diffuse_strength", 0.1);
	lightShader.setFloat("lights[2].specular_strength", 0.1);



	//Texture objects generation
	char pathim[] = PATH_TO_TEXTURE "/Sand.jpg";
	Texture GNDTex(pathim, "");
	
	char pathimG[] = PATH_TO_TEXTURE "/GroundTex.png";
	Texture GNDTexDirt(pathimG, "");
	 
	char pathimW[] = PATH_TO_TEXTURE "/wood.png";
	Texture roomTex(pathimW, "");
	 
	char pathimSapin[] = PATH_TO_TEXTURE "/sapinrep.jpg";
	Texture sapinTex(pathimSapin, "");

	char pathNormal[] = PATH_TO_TEXTURE "/woodBump.png";
	Texture normalMap(pathNormal, "normal");
	
	
	std::string PathCM( PATH_TO_TEXTURE "/cubemaps/yokohama3/sky2_");
	//cube map creation 
	CubeMap skybox(PathCM);
	
	//Frame buffer creation for mirror
	FrameBuffer framebufferMirror(width, height);

	//Framebuffer for cubemap
	CubeMap cubeRef = CubeMap(1024, 1024, 1);
	FrameBuffer framebufferCube(1024,1024);

//	Camera cameraCube(glm::vec3(0.0, 4.0, -15.0));
	Camera cameraCube(mirrorSpherePos);
	glm::mat4 projectionCube = cameraCube.GetProjectionMatrixCube(90.0f, 0.01f, 100.0f);

	glm::mat4 viewCube = cameraCube.GetViewCubeMatrix(0);

	//specify how we want the transparency to be computed (here ColorOut= Cfrag * alphaf + Cprev * (1-alphaf)  )
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
	
	bool firstLoop = true;
	glfwSwapInterval(1);

	double init_now = glfwGetTime();
	double moonSpeed = 0.25;
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		view = camera.GetViewMatrix();
		glfwPollEvents();
		double now = glfwGetTime();
		//moving moon
		glm::vec3 newMoonPos = glm::vec3(-50.0 * std::cos(moonSpeed * (now - init_now)) , 50.0 * std::sin(moonSpeed * (now - init_now)), 0.0);
		modelMoon = glm::mat4(1.0);
		modelMoon = glm::translate(modelMoon, newMoonPos);
		modelMoon = glm::scale(modelMoon, glm::vec3(2.0, 2.0, 2.0));
//		lightShader.setFloat("lights[2].ambient_strength", (float) glm::abs(newMoonPos.y));
//		elem_moon.move(newMoonPos,2.0);

		//moving light
		auto delta = light_pos + glm::vec3(0.0, 0.0, 6 * std::sin(now));
		auto deltas = lights_positions;
		deltas[0] += glm::vec3(0.0, 0.0, 6 * std::sin(now));
		deltas[2] = newMoonPos;
		
		float moonAmbientValue = glm::max(0.0, 0.14 * newMoonPos.y / 50.0);


		if (firstLoop) {
////////////////////////////////////////////////////////////////////////////////////////////
// Pass 0 :
			//firstLoop = false; //UNCOMMENT/COMMENT
			//draw the cubeMap 
			framebufferCube.Bind(0);

			for (int loop = 0; loop < 6; ++loop) {
				framebufferCube.attachCubeFace(cubeRef.ID, loop);

				glEnable(GL_DEPTH_TEST);
				//clear framebuffer contents
				glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				viewCube = cameraCube.GetViewCubeMatrix(loop);
				//viewCube = glm::mat4(1.0);
				//draw scene (simplified if possible) 
				classicShader.use();

				classicShader.setMatrix4("M", modelBunny);
				classicShader.setMatrix4("V", viewCube);
				classicShader.setMatrix4("P", projectionCube);
				classicShader.setMatrix4("R", glm::mat4(1.0));

				classicShader.setFloat("alpha", 0.5);
				classicShader.setVector3f("colorRGB", glm::vec3(1.0, 0.6, 0.0));

				bunny.draw();
				
				classicShader.setVector3f("colorRGB", glm::vec3(1.0, 1.0, 0.9));
				classicShader.setMatrix4("M", modelMoon);
				moon.draw();
				
//				elem_moon.display(viewCube,projectionCube);

				glDepthFunc(GL_LEQUAL);

				cubeMapShader.use();
				cubeMapShader.setTexUnit("cubemapSampler", 1);
				cubeMapShader.setMatrix4("V", viewCube);
				cubeMapShader.setMatrix4("P", projectionCube);
				skybox.Bind(1);


				cubeMap.draw();
				glDepthFunc(GL_LESS);

				//ground 
				shaderGND.use();

				shaderGND.setMatrix4("M", modelBunnyText);
				shaderGND.setMatrix4("V", viewCube);
				shaderGND.setMatrix4("P", projectionCube);

				// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
				shaderGND.setTexUnit("tex0", 1);
				// Binds texture so that is appears in rendering to the right unit
				GNDTex.Bind(1);

				bunnyText.draw();

				// Binds texture so that is appears in rendering
				GNDTexDirt.Bind(1);

				shaderGND.setMatrix4("M", modelBunnyText2);

				bunnyText.draw(); //same object with different texture and model uniforms


				//room (for objects without bump mapping)
				lightShader.use();
				lightShader.setFloat("lights[2].ambient_strength", moonAmbientValue);
				lightShader.setMatrix4("M", modelRoom);
				lightShader.setMatrix4("itM", inverseModelRoom); //should be modified with regards to R 
				lightShader.setMatrix4("R", glm::mat4(1.0));
				lightShader.setMatrix4("V", viewCube);
				lightShader.setMatrix4("P", projectionCube);
				lightShader.setVector3f("u_view_pos", cameraCube.Position);
				//lightShader.setVector3f("light.light_pos", delta);

				lightShader.setLightsPos(lights_number, lights_positions);
				lightShader.setTexUnit("tex0", 1);
				
				// Binds texture so that is appears in rendering to the right unit
				sapinTex.Bind(1);
				sapin.draw();

				lightShader.setMatrix4("M", modelSol);
				GNDTex.Bind(1);
				ground.draw();
				
				/*
				lightShader.setVector3f("Emitted", glm::vec3(1.0, 1.0, 0.9));
				lightShader.setMatrix4("M", modelMoon);
				moon.draw();
				*/
				//room with bump mapping
				shaderBump.use();
				shaderBump.setFloat("light.ambient_strength", ambient + moonAmbientValue);
				shaderBump.setMatrix4("M", modelRoom);
				shaderBump.setMatrix4("itM", inverseModelRoom);
				shaderBump.setMatrix4("R", glm::mat4(1.0));
				shaderBump.setMatrix4("V", viewCube);
				shaderBump.setMatrix4("P", projectionCube);
				shaderBump.setVector3f("u_view_pos", cameraCube.Position);
				shaderBump.setVector3f("light.light_pos", delta);

				// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
				shaderBump.setTexUnit("tex0", 1);
				// Binds texture so that is appears in rendering to the right unit
				roomTex.Bind(1);

				// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
				shaderBump.setTexUnit("normal0", 2);
				// Binds texture so that is appears in rendering to the right unit
				normalMap.Bind(2);

				room.draw();
			}

			framebufferCube.Unbind();
		}

		

		//bind the frambuffer for the reversed scene
		framebufferMirror.Bind(0);
		glEnable(GL_DEPTH_TEST);
		//clear framebuffer contents
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

////////////////////////////////////////////////////////////////////////////////////////////
//First pass: Draw reversed Scene 
		// 
		//draw bunny
		classicShader.use();

		classicShader.setMatrix4("M", modelBunny);
		classicShader.setMatrix4("V", view);
		classicShader.setMatrix4("P", perspective);
		classicShader.setMatrix4("R", reflection);

		classicShader.setFloat("alpha", 0.5);
		classicShader.setVector3f("colorRGB", glm::vec3(1.0, 0.6, 0.0));

		bunny.draw();
		
		classicShader.setVector3f("colorRGB", glm::vec3(1.0, 1.0, 0.9));
		classicShader.setMatrix4("M", modelMoon);
		moon.draw();
		
//		elem_moon.display(viewCube, perspective);

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
		shader.setTexUnit("cubemapSampler", 0);
		
	
		glDepthFunc(GL_LEQUAL);
		sphere1.draw();


		cubeMapShader.use();
		cubeMapShader.setMatrix4("V", view);
		cubeMapShader.setMatrix4("P", perspective);
		cubeMapShader.setTexUnit("cubemapSampler", 0);

		cubeMap.draw();
		glDepthFunc(GL_LESS);
		

		//room (with bump mapping)
		shaderBump.use();
		shaderBump.setFloat("light.ambient_strength",ambient + moonAmbientValue);
		shaderBump.setMatrix4("M", modelRoom);
		shaderBump.setMatrix4("itM", inverseModelRoom); //should be modified with regards to R 
		shaderBump.setMatrix4("R", reflection);
		shaderBump.setMatrix4("V", view);
		shaderBump.setMatrix4("P", perspective);
		shaderBump.setVector3f("u_view_pos", camera.Position);
		shaderBump.setVector3f("lightPos", delta);

		// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
		shaderBump.setTexUnit("tex0", 0);
		// Binds texture so that is appears in rendering to the right unit
		roomTex.Bind(0);

		// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
		shaderBump.setTexUnit("normal0", 1);
		// Binds texture so that is appears in rendering to the right unit
		normalMap.Bind(1);

		room.draw();

		//room (for objects without bump mapping)
		lightShader.use();
		lightShader.setFloat("lights[2].ambient_strength", moonAmbientValue);
		lightShader.setMatrix4("M", modelRoom);
		lightShader.setMatrix4("itM", inverseModelRoom); //should be modified with regards to R 
		lightShader.setMatrix4("R", reflection);
		lightShader.setMatrix4("V", view);
		lightShader.setMatrix4("P", perspective);
		lightShader.setVector3f("u_view_pos", camera.Position);
		lightShader.setLightsPos(lights_number, deltas);

		lightShader.setTexUnit("tex0", 0);

		// Binds texture so that is appears in rendering to the right unit
		sapinTex.Bind(0);
		sapin.draw();
		
		lightShader.setMatrix4("M", modelSol);
		GNDTex.Bind(0);
		ground.draw();

		/*
		lightShader.setVector3f("Emitted", glm::vec3(1.0, 1.0, 0.9));
		lightShader.setMatrix4("M", modelMoon);
		moon.draw();
		*/
		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		framebufferMirror.Unbind();
		
		// clear all relevant buffers
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

////////////////////////////////////////////////////////////////////////////////////////////
// Second pass : draw normal scene + mirror with texture from 1st pass
		// 
		//draw bunny
		
		classicShader.use();
		classicShader.setMatrix4("M", modelBunny);
		classicShader.setMatrix4("V", view);
		classicShader.setMatrix4("P", perspective);
		classicShader.setMatrix4("R", glm::mat4(1.0));

		classicShader.setFloat("alpha", 0.5);
		classicShader.setVector3f("colorRGB", glm::vec3(1.0, 0.6, 0.0));

		bunny.draw();
		
		classicShader.setVector3f("colorRGB", glm::vec3(1.0, 1.0, 0.9));
		classicShader.setMatrix4("M", modelMoon);
		moon.draw();
		
//		elem_moon.display(viewCube, projectionCube);

		//refraction sphere
		shader.use();

		shader.setMatrix4("M",modelS);
		shader.setMatrix4("itM", inverseModelS);
		shader.setMatrix4("V", view);
		shader.setMatrix4("P", perspective);
		shader.setMatrix4("R", glm::mat4(1.0));
		shader.setVector3f("u_view_pos", camera.Position);


		cubeRef.Bind(0);
		shader.setTexUnit("cubemapSampler", 0);

		
		glDepthFunc(GL_LEQUAL);
		sphere1.draw();

		skybox.Bind(0);
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

		
		// Binds texture so that is appears in rendering
		GNDTexDirt.Bind(0);

		shaderGND.setMatrix4("M", modelBunnyText2);

		bunnyText.draw(); //same object with different texture and model uniforms


		//room with bump mapping
		shaderBump.use();
		shaderBump.setFloat("light.ambient_strength",ambient + moonAmbientValue);
		shaderBump.setMatrix4("M", modelRoom);
		shaderBump.setMatrix4("itM", inverseModelRoom);
		shaderBump.setMatrix4("R", glm::mat4(1.0));
		shaderBump.setMatrix4("V", view);
		shaderBump.setMatrix4("P", perspective);
		shaderBump.setVector3f("u_view_pos", camera.Position);
		shaderBump.setVector3f("lightPos", delta);

		// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
		shaderBump.setTexUnit("tex0", 0);
		// Binds texture so that is appears in rendering to the right unit
		roomTex.Bind(0);

		// Assigns a value(the unit of the texture) to the uniform; NOTE: Must always be done after activating the Shader Program
		shaderBump.setTexUnit("normal0", 1);
		// Binds texture so that is appears in rendering to the right unit
		normalMap.Bind(1);

		room.draw();

		//room (for objects without bump mapping)
		lightShader.use();
		lightShader.setFloat("lights[2].ambient_strength", moonAmbientValue);
		lightShader.setMatrix4("M", modelRoom);
		lightShader.setMatrix4("itM", inverseModelRoom); //should be modified with regards to R 
		lightShader.setMatrix4("R", glm::mat4(1.0));
		lightShader.setMatrix4("V", view);
		lightShader.setMatrix4("P", perspective);
		lightShader.setVector3f("u_view_pos", camera.Position);
		lightShader.setLightsPos(lights_number, deltas);

		lightShader.setTexUnit("tex0", 0);

		// Binds texture so that is appears in rendering to the right unit
		sapinTex.Bind(0);
		sapin.draw();

		lightShader.setMatrix4("M", modelSol);
		GNDTex.Bind(0);
		ground.draw();

		/*
		lightShader.setVector3f("Emitted", glm::vec3(1.0, 1.0, 0.9));
		lightShader.setMatrix4("M", modelMoon);
		moon.draw();
		*/

		// now draw the mirror quad with screen texture
	    // --------------------------------------------

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
		mirror.draw();
		glDisable(GL_BLEND);

		//EMITTER
		emitter.update(0.01f);

		emitterShader.use();
		emitterShader.setMatrix4("P", perspective);
		emitterShader.setMatrix4("V", view);
		emitterShader.setUniformParticleSize("particleSize", 0.1f);

		emitter.draw(emitterShader);
		

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
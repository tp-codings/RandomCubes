#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader/shader.h>
#include <Camera/camera.h>
#include <ModelLoader/model.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>

#include <imgui/imconfig.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "TextRenderer.h"
#include "ModelHandler.h"
#include "SkyBox.h"

class Simulation
{
public:
	Simulation(GLFWwindow* window, int WINDOW_WIDTH, int WINDOW_HEIGHT);

	void update(float deltaTime, int FPS, Camera camera);
	void render();

	float getCameraSpeed();
	bool getViewMode();

private:
	//Window
	GLFWwindow* window;
	int WINDOW_WIDTH;
	int WINDOW_HEIGHT;

	//Text
	TextRenderer* textRenderer;
	int fontSize;

	//Shader
	Shader screenShader;
	Shader textShader;

	Shader cubeMapShader;
	Shader cubeShader;

	//Matrizen
	glm::mat4 projection;
	glm::mat4 view;

	std::vector<glm::vec3> translations; 
	std::vector<glm::vec3> colors; 

	//TIMING
	float deltaTime;
	float FPS;
	
	int counter;

	int lastDirection;

	//World objects

	ModelHandler* testCube;

	Camera camera;

	//Settings
	float cameraSpeed;

	bool viewMode;
	bool start;

	ImVec4 dirLightColor;
	glm::vec3 dirLightPos;

	bool startKeyPressed;
	bool settingsKeyPressed;

	//Buffer
	unsigned int screenVAO;
	unsigned int screenVBO;
	unsigned int framebuffer;
	unsigned int texColorBuffer;
	unsigned int rbo;

	//Vertices
	float* quadVertices;

	//Skybox
	Skybox *skyBox;
	Skybox *spaceBox;
	int skyBoxChoice;

	std::vector<std::string> sky
	{
			"resources\\textures\\skybox\\right.jpg",
			"resources\\textures\\skybox\\left.jpg",
			"resources\\textures\\skybox\\top.jpg",
			"resources\\textures\\skybox\\bottom.jpg",
			"resources\\textures\\skybox\\front.jpg",
			"resources\\textures\\skybox\\back.jpg"
	};

	std::vector<std::string> space
	{
		"resources\\textures\\skybox\\space_1_right.jpg",
			"resources\\textures\\skybox\\space_1_left.jpg",
			"resources\\textures\\skybox\\space_1_top.jpg",
			"resources\\textures\\skybox\\space_1_bottom.jpg",
			"resources\\textures\\skybox\\space_1_front.jpg",
			"resources\\textures\\skybox\\space_1_back.jpg"
	};



	//Inits------------------------------------------------------------------------------

	void initVertices();
	void initBuffer();
	void initShader();
	void initVariables();
	void initModels();

	//Input------------------------------------------------------------------------------

	void processInput(float deltaTime);

	//Helper------------------------------------------------------------------------------

	int random(int range, int start);


	//Updates------------------------------------------------------------------------------

	void updateCubes();

	//Rendering------------------------------------------------------------------------------

	void DrawScene();
	void DrawSettings();
	void DrawScreen();
	void DrawCube();
	void DrawText();
	void DrawSkyBox();
};


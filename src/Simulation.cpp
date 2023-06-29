#include "Simulation.h"
#include <thread>
#include <random>

// Base Colors
#define RED glm::vec3(1.0f, 0.0f, 0.0f)
#define GREEN glm::vec3(0.0f, 1.0f, 0.0f)
#define BLUE glm::vec3(0.0f, 0.0f, 1.0f)
#define WHITE glm::vec3(1.0f, 1.0f, 1.0f)
#define BLACK glm::vec3(0.0f, 0.0f, 0.0f)

// Custom Colors
#define ORANGE glm::vec3(1.0f, 0.5f, 0.0f)
#define PURPLE glm::vec3(0.5f, 0.0f, 0.5f)
#define YELLOW glm::vec3(1.0f, 1.0f, 0.0f)
#define CYAN glm::vec3(0.0f, 1.0f, 1.0f)
#define MAGENTA glm::vec3(1.0f, 0.0f, 1.0f)


// Pastell Colors
#define PASTEL_PINK glm::vec3(0.86f, 0.63f, 0.69f)
#define PASTEL_YELLOW glm::vec3(0.95f, 0.95f, 0.58f)
#define PASTEL_BLUE glm::vec3(0.62f, 0.77f, 0.87f)
#define PASTEL_GREEN glm::vec3(0.64f, 0.87f, 0.68f)
#define PASTEL_PURPLE glm::vec3(0.73f, 0.62f, 0.81f)

// Metallic Colors
#define GOLD glm::vec3(1.0f, 0.84f, 0.0f)
#define SILVER glm::vec3(0.75f, 0.75f, 0.75f)
#define BRONZE glm::vec3(0.8f, 0.5f, 0.2f)
#define COPPER glm::vec3(0.85f, 0.55f, 0.4f)
#define STEEL glm::vec3(0.6f, 0.6f, 0.67f)

// Rainbow Colors
#define VIOLET glm::vec3(0.5f, 0.0f, 1.0f)
#define INDIGO glm::vec3(0.29f, 0.0f, 0.51f)
#define BLUE_GREEN glm::vec3(0.0f, 0.5f, 0.5f)
#define YELLOW_GREEN glm::vec3(0.6f, 0.8f, 0.2f)
#define YELLOW_ORANGE glm::vec3(1.0f, 0.71f, 0.0f)
#define RED_ORANGE glm::vec3(1.0f, 0.27f, 0.0f)

// Earth Tones
#define BROWN glm::vec3(0.65f, 0.16f, 0.16f)
#define SAND glm::vec3(0.76f, 0.7f, 0.5f)
#define OLIVE glm::vec3(0.5f, 0.5f, 0.0f)
#define MOSS_GREEN glm::vec3(0.55f, 0.64f, 0.45f)
#define SLATE_GRAY glm::vec3(0.44f, 0.5f, 0.56f)

// Neon Colors
#define NEON_PINK glm::vec3(1.0f, 0.07f, 0.55f)
#define NEON_YELLOW glm::vec3(1.0f, 0.95f, 0.0f)
#define NEON_GREEN glm::vec3(0.29f, 0.95f, 0.29f)
#define NEON_BLUE glm::vec3(0.29f, 0.59f, 0.95f)
#define NEON_PURPLE glm::vec3(0.67f, 0.29f, 0.95f)

Simulation::Simulation(GLFWwindow* window, int WINDOW_WIDTH, int WINDOW_HEIGHT)
{
	this->window = window;
	this->WINDOW_WIDTH = WINDOW_WIDTH;
	this->WINDOW_HEIGHT = WINDOW_HEIGHT;

	this->initShader();
	this->initVertices();
	this->initVariables();
	this->initModels();
	this->initBuffer();

	this->skyBox = new Skybox(this->sky);
	this->spaceBox = new Skybox(this->space);
	this->forestBox = new Skybox(this->forest);
	this->cityBox = new Skybox(this->city);

	//ImGUI Setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(this->window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

}

void Simulation::update(float deltaTime, int FPS, Camera camera)
{
	//Update view and projection matrix
	this->projection = glm::perspective(glm::radians(camera.Zoom), (float)this->WINDOW_WIDTH / (float)this->WINDOW_HEIGHT, 0.1f, 10000.0f);
	this->view = camera.GetViewMatrix();

	this->deltaTime = deltaTime;
	this->FPS = FPS;
	this->camera = camera;

	this->processInput(deltaTime);

	if (this->start)
	{
		this->updateCubes();
	}


}

void Simulation::render()
{
	//Draw scene to framebuffer which will be rendered as a texture for post processing purposes
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
	glEnable(GL_DEPTH_TEST);
	//Sets Backgroundcolor to dimmed color of the directional light color
	glClearColor(this->dirLightColor.x* 0.2f, this->dirLightColor.y* 0.2f, this->dirLightColor.z* 0.2f, 0.2f);

	//Bufferclear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Render
	this->DrawScene();

	this->DrawCube();

	this->DrawSkyBox();

	this->DrawScreen();

	this->DrawText();

	this->DrawSettings();

	ImGui::EndFrame();
}

float Simulation::getCameraSpeed()
{
	return this->cameraSpeed;
}

bool Simulation::getViewMode()
{
	return this->viewMode;
}

//Inits------------------------------------------------------------------------------

void Simulation::initVertices()
{
	//Screen quad
	float qVertices[] = {
		// positions // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	this->quadVertices = new float[6 * 4];

	std::memcpy(this->quadVertices, qVertices, sizeof(qVertices));
}

void Simulation::initBuffer()
{
	//Screen Buffer
	glGenVertexArrays(1, &this->screenVAO);
	glGenBuffers(1, &this->screenVBO);
	glBindVertexArray(this->screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->screenVBO);
	//position attribute
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, this->quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Screen Framebuffer
	glGenFramebuffers(1, &this->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
	//create a color attachment texture
	glGenTextures(1, &this->texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, this->texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->WINDOW_WIDTH, this->WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texColorBuffer, 0);

	//create a renderbuffer object for depth and stencil attachment
	glGenRenderbuffers(1, &this->rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Simulation::initShader()
{
	//Create Shader objects for several shader units
	this->screenShader = Shader("Shader/screen.vs", "Shader/screen.fs");
	this->textShader = Shader("Shader/text.vs", "Shader/text.fs");
	this->cubeShader = Shader("Shader/cube.vs", "Shader/cube.fs");
	this->cubeMapShader = Shader("Shader/cubemap.vs", "Shader/cubemap.fs");
}

void Simulation::initVariables()
{
	//Settings
	this->cameraSpeed = 6.0f;
	this->lastDirection = -1;

	//Textrendering
	this->textRenderer = new TextRenderer(10, this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
	this->fontSize = 10;

	//Settingbooleans
	this->start = false;
	this->viewMode = true;
	this->dirLightColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->dirLightPos = glm::vec3(glm::vec3(-0.5, 0.7, -1.0) * 40.f);

	//Projection matrices
	this->projection = glm::mat4(1.0f);
	this->view = glm::mat4(1.0f);

	//Key pressed booleans
	this->startKeyPressed = false;
	this->settingsKeyPressed = false;
	this->shadingKeyPressed = false;

	this->shadingChoice = 0;

	this->translations.push_back(glm::vec3(0.0f));
	this->colors.push_back(BLUE_GREEN);
	this->counter = 0;
	this->skyBoxChoice = 1;

}

void Simulation::initModels()
{
	//Initialize Models
	this->testCube = new ModelHandler("resources\\models\\cube\\cube.obj");
}

//Inputhandling------------------------------------------------------------------------------

void Simulation::processInput(float deltaTime)
{
	//Keyboard shortcuts
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->window, true);
	}
	if (glfwGetKey(this->window, GLFW_KEY_F) == GLFW_PRESS && !this->settingsKeyPressed)
	{
		this->viewMode = !this->viewMode;
		this->settingsKeyPressed = true;
	}

	if (glfwGetKey(this->window, GLFW_KEY_F) == GLFW_RELEASE)
	{
		this->settingsKeyPressed = false;
	}
	if (!this->viewMode) {
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else {
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (glfwGetKey(this->window, GLFW_KEY_ENTER) == GLFW_PRESS && !this->startKeyPressed)
	{
		this->start = !this->start;
		this->startKeyPressed = true;
	}
	if (glfwGetKey(this->window, GLFW_KEY_ENTER) == GLFW_RELEASE)
	{
		this->startKeyPressed = false;
	}
	if (glfwGetKey(this->window, GLFW_KEY_B) == GLFW_PRESS && !this->shadingKeyPressed)
	{
		this->shadingChoice = ++this->shadingChoice%2;
		this->shadingKeyPressed = true;
	}
	if (glfwGetKey(this->window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		this->shadingKeyPressed = false;
	}
	if (glfwGetKey(this->window, GLFW_KEY_4) == GLFW_PRESS)
	{
		this->skyBoxChoice = 4;
	}
	if (glfwGetKey(this->window, GLFW_KEY_3) == GLFW_PRESS)
	{
		this->skyBoxChoice = 3;
	}
	if (glfwGetKey(this->window, GLFW_KEY_2) == GLFW_PRESS)
	{
		this->skyBoxChoice = 2;
	}
	if (glfwGetKey(this->window, GLFW_KEY_1) == GLFW_PRESS)
	{
		this->skyBoxChoice = 1;
	}
}

//Helper------------------------------------------------------------------------------

int Simulation::random(int range, int start)
{
	return rand() % range + start;
}

//Updates------------------------------------------------------------------------------

void Simulation::updateCubes()
{
	this->counter++;
	if (this->counter > 0) {
		int direction = random(6, 1);

		while ((direction * this->lastDirection == 4 || direction * this->lastDirection == 10 || direction * this->lastDirection == 18) && direction != this->lastDirection){
			direction = random(6, 1);
		}

		this->lastDirection = direction;

		constexpr int FLOAT_MIN = 0;
		constexpr int FLOAT_MAX = 1;

		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);

		this->colors.push_back(glm::vec3(distr(eng), distr(eng), distr(eng)));

		switch (direction) {
		case 1:
			this->translations.push_back(glm::vec3(this->translations.back()[0] + 2.0, this->translations.back()[1], this->translations.back()[2]));
			break;
		case 2:
			this->translations.push_back(glm::vec3(this->translations.back()[0], this->translations.back()[1] + 2.0f, this->translations.back()[2]));
			break;
		case 3:
			this->translations.push_back(glm::vec3(this->translations.back()[0], this->translations.back()[1], this->translations.back()[2] + 2.0f));
			break;
		case 4:
			this->translations.push_back(glm::vec3(this->translations.back()[0] - 2.0f, this->translations.back()[1], this->translations.back()[2]));
			break;
		case 5:
			this->translations.push_back(glm::vec3(this->translations.back()[0], this->translations.back()[1] - 2.0f, this->translations.back()[2]));
			break;
		case 6:
			this->translations.push_back(glm::vec3(this->translations.back()[0], this->translations.back()[1], this->translations.back()[2] - 2.0f));
			break;
		}
		this->counter = 0;
	}
}

//Rendering------------------------------------------------------------------------------


void Simulation::DrawScene()
{

	glBindVertexArray(0);
}

void Simulation::DrawSettings()
{
	//Tell ImGUI its a new Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (!viewMode)
	{
		ImGui::Begin("Steuerung", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImVec2 pos(5, 50);
		ImGui::SetWindowPos(pos);

		// Init Settingswindow
		ImVec2 size((float)this->WINDOW_WIDTH / 4, (float)this->WINDOW_HEIGHT * 0.9f);
		ImGui::SetWindowSize(size);

		//Settings
		ImGui::Text("Settings");
		ImGui::SetNextItemWidth((float)this->WINDOW_WIDTH / 5);
		ImGui::SliderFloat("Camspeed", &this->cameraSpeed, 1.0f, 60.0f);

		//Simulation control

		const char* play = "Start";
		if (this->start) {
			play = "Stopp";
		}
		if (ImGui::Button(play))
		{
			this->start = !this->start;
		}		
		const char* shading = "Reflection";
		if (this->shadingChoice == 1) {
			shading = "Colors";
		}
		ImGui::SameLine();
		if (ImGui::Button(shading))
		{
			this->shadingChoice = ++this->shadingChoice % 2;
		}		
		if (ImGui::Button("Ocean"))
		{
			this->skyBoxChoice = 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Space"))
		{
			this->skyBoxChoice = 2;
		}
		ImGui::SameLine();
		if (ImGui::Button("Forest"))
		{
			this->skyBoxChoice = 3;
		}
		ImGui::SameLine();
		if (ImGui::Button("City"))
		{
			this->skyBoxChoice = 4;
		}


		ImGui::ColorPicker3("DirLight", (float*)&this->dirLightColor, ImGuiColorEditFlags_InputRGB);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void Simulation::DrawScreen()
{
	//Draw Scene texture to standard Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	this->screenShader.use();
	this->screenShader.setInt("screenTexture", 0);
	this->screenShader.setInt("choice", 7);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->screenVAO);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, this->texColorBuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Simulation::DrawCube()
{
	float scaleFactor = 0.9f;	
	this->cubeShader.use();

	this->cubeShader.setVec3("dirLightColor", glm::vec3(this->dirLightColor.x*1.5f, this->dirLightColor.y * 1.5f, this->dirLightColor.z * 1.5f));
	this->cubeShader.setVec3("dirLightDir", -this->dirLightPos);
	this->cubeShader.setVec3("lightColor", glm::vec3(BLACK));
	this->cubeShader.setVec3("lightPos", this->dirLightPos);
	this->cubeShader.setVec3("viewPos", camera.Position);
	this->cubeShader.setInt("skybox", 0);
	this->cubeShader.setInt("shadingChoice", this->shadingChoice);

	this->cubeShader.setInt("shininess", 512);

	for (int i = 0; i < this->translations.size(); i++) {
		this->testCube->Translate(this->translations[i]);
		this->testCube->Scale(scaleFactor);
		this->testCube->Draw(&this->cubeShader, this->projection, this->view, this->colors[i]);
	}
}


void Simulation::DrawText()
{
	this->textRenderer->Draw(this->textShader, "FPS: " + std::to_string((int)this->FPS), 0.0f, (float)this->WINDOW_HEIGHT - 1 * (float)this->fontSize, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	this->textRenderer->Draw(this->textShader, "Pos: " + std::to_string(this->camera.Position.x) + ", " + std::to_string(this->camera.Position.y) + ", " + std::to_string(this->camera.Position.z), 0.0f, (float)this->WINDOW_HEIGHT - 2 * (float)this->fontSize, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	this->textRenderer->Draw(this->textShader, "CameraView: " + std::to_string(this->camera.Front.x) + ", " + std::to_string(this->camera.Front.y) + ", " + std::to_string(this->camera.Front.z), 0.0f, (float)this->WINDOW_HEIGHT - 3 * (float)this->fontSize, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	this->textRenderer->Draw(this->textShader, "DeltaTime: " + std::to_string(this->deltaTime), 0.0f, (float)this->WINDOW_HEIGHT - 4 * (float)this->fontSize, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	this->textRenderer->Draw(this->textShader, "Start: " + std::to_string(this->start), this->WINDOW_WIDTH / 2, (float)this->WINDOW_HEIGHT - 1 * (float)this->fontSize, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	std::string shading = "Colors";
	if (this->shadingChoice == 1) {
		shading = "Reflection";
	}
	this->textRenderer->Draw(this->textShader, "Shading: " + shading, this->WINDOW_WIDTH / 2, (float)this->WINDOW_HEIGHT - 2 * (float)this->fontSize, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	
	std::string skyboxes[] = { "Ocean", "Space", "Forest", "City" };
	this->textRenderer->Draw(this->textShader, "Skybox: " + skyboxes[skyBoxChoice-1], this->WINDOW_WIDTH / 2, (float)this->WINDOW_HEIGHT - 3 * (float)this->fontSize, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void Simulation::DrawSkyBox()
{
	switch (this->skyBoxChoice) {
	case 2:
		this->spaceBox->render(this->cubeMapShader, this->camera, this->projection);
		break;	
	case 3:
		this->forestBox->render(this->cubeMapShader, this->camera, this->projection);
		break;	
	case 4:
		this->cityBox->render(this->cubeMapShader, this->camera, this->projection);
		break;
	default:
		this->skyBox->render(this->cubeMapShader, this->camera, this->projection);
	}
}

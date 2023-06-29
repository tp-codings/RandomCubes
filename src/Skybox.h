#pragma once
#include <vector>
#include <string>
#include <Shader/Shader.h>
#include <Camera/camera.h>
#include <stb_image/stb_image.h>


class Skybox
{
public: 
	Skybox(std::vector <std::string> faces);
	void render(Shader s, Camera camera, glm::mat4 projection);

	void setSkyBox(std::vector<std::string>faces);

private:
	//std::vector<std::string> faces;
	
	float* cubemapVertices;

	unsigned int VAO;
	unsigned int VBO;

	unsigned int cubeMapTexture;

	void initVertices();
	void initBuffer();
	unsigned int loadCubeMap(std::vector<std::string> faces);
};


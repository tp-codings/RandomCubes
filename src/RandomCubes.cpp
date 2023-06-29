#include "Engine.h"


int main()
{
	srand(static_cast<unsigned>(time(0)));

	Engine RandomCubes;
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	RandomCubes.run();

	return 0;
}
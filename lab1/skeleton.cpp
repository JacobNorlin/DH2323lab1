// Introduction lab that covers:
// * C++
// * SDL
// * 2D graphics
// * Plotting pixels
// * Video memory
// * Color representation
// * Linear interpolation
// * glm::vec3 and std::vector

#include "SDL.h"
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include "SDLauxiliary.h"
#include "Utils.h"

using namespace std;
using glm::vec3;

// --------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Surface* screen;

// --------------------------------------------------------
// FUNCTION DECLARATIONS

void Draw();

// --------------------------------------------------------
// FUNCTION DEFINITIONS

int main(int argc, char* argv[])
{
	vector<vec3> vec(4);
	vec3 myints[] = { vec3(1.0f, 4.0f, 9.2f), vec3(2.0f, 3.0f, 9.4f), vec3(3.0f, 2.0f, 9.6f), vec3(4.0f, 1.0f, 9.8f) };
	std::vector<vec3> corr(myints, myints + sizeof(myints) / sizeof(vec3));
	vec3 a(1.0f, 4.0f, 9.2f);
	vec3 b(4.0f, 1.0f, 9.8f);
	Interpolate(a, b, vec);
	for (int i = 0; i < corr.size(); i++) {

	}
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	while (NoQuitMessageSDL())
	{
		Draw();
	}
	SDL_SaveBMP(screen, "screenshot.bmp");
	return 0;
}

void Draw()
{

	for (int y = 0; y<SCREEN_HEIGHT; ++y)
	{

		for (int x = 0; x<SCREEN_WIDTH; ++x)
		{
			vec3 color(0, 0, 1);
			PutPixelSDL(screen, x, y, color);
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
}


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
#include <random>
#include "SDLauxiliary.h"
#include "Utils.h"

using namespace std;
using glm::vec3;
using glm::vec2;

// --------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Surface* screen;
vector<vec3> stars(1000);int t;const float V = 0.001;

// --------------------------------------------------------
// FUNCTION DECLARATIONS

void Draw();
void setupStars();
vec3 randomVec3();
float randCoord(float lower, float upper);
void drawStarfield();
void Update();
// --------------------------------------------------------
// FUNCTION DEFINITIONS


vec3 randomVec3() {
	return vec3(float(rand()) / float(RAND_MAX) * 2 - 1, float(rand()) / float(RAND_MAX) * 2 - 1, float(rand()) / float(RAND_MAX));}

vec2 projectPoint(vec3 p)
{
	const float f = SCREEN_HEIGHT / 2;
	float u = f * (p.x / p.z) + (SCREEN_WIDTH / 2);
	float v = f * (p.y / p.z) + (SCREEN_HEIGHT / 2);
	return vec2(u, v);
}


void setupStars() {
	for (int i = 0; i < stars.size(); i++) {
		stars[i] = randomVec3();
	}
}

int main(int argc, char* argv[])
{
	t = SDL_GetTicks();

	setupStars();
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	while (NoQuitMessageSDL())
	{
		Update();
		drawStarfield();
	}
	SDL_SaveBMP(screen, "screenshot.bmp");
	return 0;
}

void Draw()
{
	vec3 topLeft(1, 0, 0);
	vec3 topRight(0, 0, 1);
	vec3 bottomLeft(0, 1, 0);
	vec3 bottomRight(1, 1, 0);

	vector<vec3> leftSide(SCREEN_HEIGHT);
	vector<vec3> rightSide(SCREEN_HEIGHT);
	vector<vec3> colors(SCREEN_WIDTH);
	Interpolate(topLeft, bottomLeft, leftSide);
	Interpolate(topRight, bottomRight, rightSide);

	for (int y = 0; y<SCREEN_HEIGHT; ++y)
	{
		Interpolate(rightSide[y],	leftSide[y], colors);
		for (int x = 0; x<SCREEN_WIDTH; ++x)
		{
			PutPixelSDL(screen, x, y, colors[x]);
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
}


void drawStarfield()
{
	SDL_FillRect(screen, 0, 0);
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);
	for (size_t s = 0; s<stars.size(); ++s)
	{
		vec3 p = stars[s];
		vec2 projP = projectPoint(p);
		PutPixelSDL(screen, projP.x, projP.y, vec3(1, 1, 1));
	}
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, 0, 0);

}

void Update()
{
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	t = t2;

	for (int s = 0; s<stars.size(); ++s)
	{
		// Add code for update of stars
		stars[s].z = stars[s].z - V*dt;
		if (stars[s].z <= 0)
			stars[s].z += 1;
		if (stars[s].z > 1)
			stars[s].z -= 1;
	}
}
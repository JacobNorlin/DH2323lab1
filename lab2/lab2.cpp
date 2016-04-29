#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;

// ----------------------------------------------------------------------------
// DATA STRUCTURES
struct Intersection
{
	vec3 position;
	float distance;
	int triangleIndex;
};

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const float focalLength = SCREEN_HEIGHT / 2;
const vec3 cameraPos(0, 0, -2);
SDL_Surface* screen;
int t;
vector<Triangle> triangles;

Intersection intersection;
// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
bool ClosestIntersection(
	vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection
	);
void planeIntersection(Triangle triangle, vec3 start, vec3 dir, vec3& ret);
bool intersectionInTriangle(vec3 intersectingPoint);

int main( int argc, char* argv[] )
{
	LoadTestModel(triangles);
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.
	Draw();
	while( NoQuitMessageSDL() )
	{
		Update();
		
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
}

void Draw()
{
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		for( int x=0; x<SCREEN_WIDTH; ++x )
		{
			vec3 dir(x - (SCREEN_WIDTH / 2), y - (SCREEN_HEIGHT / 2), focalLength);
			dir = glm::normalize(dir);

			
			if (ClosestIntersection(cameraPos, dir, triangles, intersection)) {
				Triangle t = triangles[intersection.triangleIndex];
				PutPixelSDL(screen, x, y, t.color);
			}
			else {
				PutPixelSDL(screen, x, y, vec3(0,0,0));
			}
			
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
bool ClosestIntersection(
	vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection
	)
{

	float minDistance = std::numeric_limits<float>::max();
	bool foundIntersection = false;

	for (int i = 0; i < triangles.size(); i++) {
		Triangle triangle = triangles[i];
				vec3 intersectingPoint;
		planeIntersection(triangle, start, dir, intersectingPoint);

		if (intersectionInTriangle(intersectingPoint)) {
			float r = abs(intersectingPoint.x);
			if (r < minDistance) {
				minDistance = r;
				closestIntersection.triangleIndex = i;
				foundIntersection = true;
			}
		}
	}
	
	return foundIntersection;
}

bool intersectionInTriangle(vec3 intersectingPoint)
{
	float u = intersectingPoint.y;
	float v = intersectingPoint.z;

	return ((u + v) < 1 && u >= 0 && v >= 0);
		
}

void planeIntersection(Triangle triangle, vec3 start, vec3 dir, vec3& ret) {
	vec3 v0 = triangle.v0;
	vec3 v1 = triangle.v1;
	vec3 v2 = triangle.v2;
	vec3 e1 = v1 - v0;
	vec3 e2 = v2 - v0;
	vec3 b = start - v0;
	mat3 A(-dir, e1, e2);
	ret = glm::inverse(A) * b;
}
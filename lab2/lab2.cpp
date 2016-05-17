#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

# define M_PI           3.14159265358979323846

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

const int SCREEN_WIDTH = 300;
const int SCREEN_HEIGHT = 300;
const float focalLength = SCREEN_HEIGHT;
vec3 cameraPos(0, 0, -3);
SDL_Surface* screen;
int t;
vector<Triangle> triangles;
mat3 R;
float yaw;
float cameraSpeed = 0.001;
vec3 lightPos(0, -0.5, -0.7);
vec3 lightColor = 14.f * vec3(1, 1, 1);
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
void updateRotationMatrix();
vec3 DirectLight(const Intersection& i);
float D(float P, vec3 rv, vec3 nv, float r);


int main( int argc, char* argv[] )
{
	LoadTestModel(triangles);
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.
	while( NoQuitMessageSDL() )
	{
		Update();
		Draw();
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	t = t2;
	if (dt < 20) {
		SDL_Delay(20 - dt);
	}
	cout << "Render time: " << dt << " ms." << endl;
	Uint8* keystate = SDL_GetKeyState(0);

	vec3 forward(R[2][0], R[2][1], R[2][2]);

	if (keystate[SDLK_UP])
	{
		cameraPos += forward*dt*cameraSpeed;
	}
	if (keystate[SDLK_DOWN])
	{
		cameraPos -= forward*dt*cameraSpeed;

	}
	if (keystate[SDLK_LEFT])
	{
		yaw -= 0.01;
		updateRotationMatrix();
	}
	if (keystate[SDLK_RIGHT])
	{
		updateRotationMatrix();
		yaw += 0.01;
	}
	if (keystate[SDLK_w])
		lightPos += vec3(0,0,0.5);
	if (keystate[SDLK_s])
		lightPos -= vec3(0, 0, 0.5);
	if (keystate[SDLK_a])
		lightPos -= vec3(0.5, 0, 0);
	if (keystate[SDLK_d])
		lightPos += vec3(0.5, 0, 0);
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
			dir = R * dir;

			if (ClosestIntersection(cameraPos, dir, triangles, intersection)) {
				
				PutPixelSDL(screen, x, y, DirectLight(intersection));
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
		
		planeIntersection(triangles[i], cameraPos, dir, intersectingPoint);
		
		if (intersectionInTriangle(intersectingPoint)) {
			float r = abs(intersectingPoint.x);
			if (r < minDistance) {
				minDistance = r;
				closestIntersection.triangleIndex = i;
				closestIntersection.distance = r;
				closestIntersection.position = r*dir;
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

void updateRotationMatrix()
{
	vec3 r1(cos(yaw), 0, sin(yaw));
	vec3 r2(0, 1, 0);
	vec3 r3(-sin(yaw), 0, cos(yaw));
	R = mat3(r1, r2, r2);

}

vec3 DirectLight(const Intersection& i) {
	Triangle t = triangles[i.triangleIndex];
	vec3 lighting(0, 0, 0);
	Intersection shadowIntersection;
	vec3 dir = lightPos - i.position;
	float distanceToLight = glm::length(dir);
	dir = glm::normalize(dir);
	if (ClosestIntersection(i.position, dir, triangles, shadowIntersection))
	{
		if (shadowIntersection.distance <= distanceToLight) {
				lighting = D(1, dir, t.normal, 1)*lightColor*t.color;
		}
	}
	return lighting;
}

float D(float P, vec3 rv, vec3 nv, float r) {
	return (P * fmax(glm::dot(rv, nv), 0)) / (4 * M_PI * pow(r,2));
}
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
vec3 indirectLight = 0.1f*vec3(1, 1, 1);
Intersection intersection;

bool SHADOWS = true;
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
		yaw += 0.001*dt;
		updateRotationMatrix();
	}
	if (keystate[SDLK_RIGHT])
	{
		yaw -= 0.001*dt;
		updateRotationMatrix();
	}
	if (keystate[SDLK_w])
		lightPos += vec3(0,0,0.5);
	if (keystate[SDLK_s])
		lightPos -= vec3(0, 0, 0.5);
	if (keystate[SDLK_a])
		lightPos -= vec3(0.5, 0, 0);
	if (keystate[SDLK_d])
		lightPos += vec3(0.5, 0, 0);
	if (keystate[SDLK_q])
		lightPos.y -= 0.5;
	if (keystate[SDLK_e])
		lightPos.y += 0.5;
	if (keystate[SDLK_r]) {
		SHADOWS = !SHADOWS;
		SDL_Delay(200);
	}
	if (keystate[SDLK_f])
		lightColor *= 1.5;
	if (keystate[SDLK_g])
		lightColor /= 1.5;
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
				
				PutPixelSDL(screen, x, y, triangles[intersection.triangleIndex].color*(DirectLight(intersection)+indirectLight));
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
		
		vec3 v0 = triangle.v0;
		vec3 v1 = triangle.v1;
		vec3 v2 = triangle.v2;
		vec3 e1 = v1 - v0;
		vec3 e2 = v2 - v0;
		vec3 b = start - v0;
		mat3 A(-dir, e1, e2);
		vec3 x = glm::inverse(A) * b;

		if (intersectionInTriangle(x)) {
			vec3 intersectingPoint = v0 + (e1 * x.y) + (e2 * x.z);
			float r = glm::distance(start, intersectingPoint);
			if (r < minDistance) {
				minDistance = r;
				closestIntersection.triangleIndex = i;
				closestIntersection.distance = r;
				closestIntersection.position = intersectingPoint;
				foundIntersection = true;
			}
		}
	}
	
	return foundIntersection;
}

bool intersectionInTriangle(vec3 intersectingPoint)
{
	float t = intersectingPoint.x;
	float u = intersectingPoint.y;
	float v = intersectingPoint.z;

	return u >= 0 && v >= 0 && (u + v) <= 1 && t >= 0;
		
}


void updateRotationMatrix()
{
	vec3 r1(cos(yaw), 0, sin(yaw));
	vec3 r2(0, 1, 0);
	vec3 r3(-sin(yaw), 0, cos(yaw));

	R = mat3(r1, r2, r3);
}

vec3 DirectLight(const Intersection& i) {
	Triangle t = triangles[i.triangleIndex];
	Intersection shadowIntersection;
	
	vec3 lightToIntersection = lightPos - i.position;
	float distance = glm::length(lightToIntersection);
	vec3 r = glm::normalize(lightToIntersection);
	vec3 intersectionToLight = glm::normalize(i.position - lightPos);
	if (SHADOWS) {
		if (ClosestIntersection(lightPos, intersectionToLight, triangles, shadowIntersection))
		{
			if (shadowIntersection.distance < distance - 0.000001f) {//Some kind of rounding error that causes black dots, removing a bit fixes it.
				return vec3(0, 0, 0);
			}
		}
	}
	return D(1, r, t.normal, distance)*lightColor;;
}

float D(float P, vec3 rv, vec3 nv, float r) {
	float n = glm::dot(rv, nv);
	float d = 4 * M_PI * r * r;
	return (P * fmax(n, 0)) / d;
}
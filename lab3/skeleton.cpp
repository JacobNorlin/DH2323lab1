#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include "../lab1/Utils.h"

using namespace std;
using glm::vec3;
using glm::ivec2;
using glm::mat3;
using glm::vec2;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;
vector<Triangle> triangles;
vec3 cameraPos(0, 0, -3.001);
mat3 R;
float yaw = 0;
vec3 currentColor;
float f = SCREEN_HEIGHT;

float depthBuffer[SCREEN_WIDTH][SCREEN_HEIGHT];

struct Pixel
{
	int x;
	int y;
	float zinv;
};



// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
void VertexShader(const vec3&, Pixel& p);
void Interpolate(ivec2 a, ivec2 b, vector<ivec2>& result);
void Interpolate(Pixel a, Pixel b, vector<Pixel>& result);
void DrawLineSDL(SDL_Surface* surface, Pixel a, Pixel b, vec3 color);void DrawPolygonEdges(const vector<vec3>& vertices);void updateRotationMatrix();
void DrawRows(const vector<Pixel>& leftPixels, const vector<Pixel>& rightPixels);void DrawPolygon(const vector<vec3>& vertices);void ComputePolygonRows(const vector<Pixel>& vertexPixels, vector<Pixel>& leftPixels, vector<Pixel>& rightPixels);

int main( int argc, char* argv[] )
{
	LoadTestModel( triangles );
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
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;

	Uint8* keystate = SDL_GetKeyState(0);
	vec3 forward(0, 0, 0.1);
	if (keystate[SDLK_UP])
	{
		cameraPos += R*forward;
	}
		
	if (keystate[SDLK_DOWN])
	{
		cameraPos -= R*forward;
	}

	if (keystate[SDLK_RIGHT])
	{
		yaw -= 0.05;
		updateRotationMatrix();
	}
		

	if (keystate[SDLK_LEFT]) {
		yaw += 0.05;
		updateRotationMatrix();
	}
		

	if( keystate[SDLK_RSHIFT] )
		cameraPos.y -= 0.1;

	if( keystate[SDLK_RCTRL] )
		;

	if( keystate[SDLK_w] )
		;

	if( keystate[SDLK_s] )
		;

	if( keystate[SDLK_d] )
		;

	if( keystate[SDLK_a] )
		;

	if( keystate[SDLK_e] )
		;

	if( keystate[SDLK_q] )
		;
}

void resetDepthBuffer()
{
	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			depthBuffer[x][y] = numeric_limits<float>::max();;
		}
	}
}

void Draw()
{
	SDL_FillRect(screen, 0, 0);
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	resetDepthBuffer();

	for (int i = 0; i<triangles.size(); ++i)
	{
		vector<vec3> vertices(3);
		vertices[0] = triangles[i].v0;
		vertices[1] = triangles[i].v1;
		vertices[2] = triangles[i].v2;
		currentColor = triangles[i].color;
		//DrawPolygonEdges(vertices);
		DrawPolygon(vertices);
	}
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void VertexShader(const vec3& v, Pixel& p) {
	vec3 pos = (cameraPos - v) * R;
	p.x = f*(pos.x / pos.z) + (SCREEN_WIDTH / 2);
	p.y = f*(pos.y / pos.z) + (SCREEN_HEIGHT / 2);
	p.zinv = 1 / pos.z;

}

void Interpolate(ivec2 a, ivec2 b, vector<ivec2>& result)
{
	int N = result.size();
	vec2 step = vec2(b - a) / float(max(N - 1, 1));
	vec2 current(a);
	for (int i = 0; i<N; ++i)
	{
		result[i] = current;
		current += step;
	}
}
void Interpolate(Pixel a, Pixel b, vector<Pixel>& result)
{
	int N = result.size();
	vector<ivec2> points(N);
	vector<float> zs(N);
	ivec2 start(a.x, a.y);
	ivec2 end(b.x, b.y);
	//just reuse stuff that i know is working
	Interpolate(start, end, points);
	Interpolate(a.zinv, b.zinv, zs);
	for (int i = 0; i < N; i++) {
		result[i].x = points[i].x;
		result[i].y = points[i].y;
		result[i].zinv = zs[i];
	}


}

void DrawLineSDL(SDL_Surface* surface, Pixel a, Pixel b, vec3 color)
{
	ivec2 aVec = ivec2(a.x, a.y);
	ivec2 bVec = ivec2(b.x, b.y);
	ivec2 delta = glm::abs(aVec - bVec);
	int pixels = glm::max(delta.x, delta.y) + 1;

	vector<Pixel> line(pixels);
	Interpolate(a, b, line);
		
	for (int i = 0; i < line.size(); i++)
	{	
		Pixel p = line[i];
		//Check boundaries
		if (p.x >= 0 && p.y >= 0 && p.x < SCREEN_WIDTH && p.y < SCREEN_WIDTH)
		{
			if (p.zinv <= depthBuffer[p.x][p.y])
			{
				depthBuffer[p.x][p.y] = p.zinv;
				PutPixelSDL(screen, p.x, p.y, color);
			}
		}

	}
}

void DrawPolygonEdges(const vector<vec3>& vertices)
{
	int V = vertices.size();
	// Transform each vertex from 3D world position to 2D image position:
	vector<Pixel> projectedVertices(V);
	for (int i = 0; i<V; ++i)
	{
		VertexShader(vertices[i], projectedVertices[i]);
	}
	// Loop over all vertices and draw the edge from it to the next vertex:
	for (int i = 0; i<V; ++i)
	{
		int j = (i + 1) % V; // The next vertex
		vec3 color(1, 1, 1);
		DrawLineSDL(screen, projectedVertices[i], projectedVertices[j], currentColor);
	}
}

void updateRotationMatrix()
{
	vec3 r1(cos(yaw), 0, sin(yaw));
	vec3 r2(0, 1, 0);
	vec3 r3(-sin(yaw), 0, cos(yaw));

	R = mat3(r1, r2, r3);
}

void ComputePolygonRows(const vector<Pixel>& vertexPixels, vector<Pixel>& leftPixels,
	vector<Pixel>& rightPixels)
{
	int V = vertexPixels.size();

	// 1. Find max and min y-value of the polygon
	// and compute the number of rows it occupies.
	int min = numeric_limits<int>::max();
	int max = numeric_limits<int>::min();

	for (Pixel point : vertexPixels)
	{
		min = std::min(point.y, min);
		max = std::max(point.y, max);
	}
	int rows = max - min + 1;

	// 2. Resize leftPixels and rightPixels
	// so that they have an element for each row.
	leftPixels.resize(rows);
	rightPixels.resize(rows);
	// 3. Initialize the x-coordinates in leftPixels
	// to some really large value and the x-coordinates
	// in rightPixels to some really small value.
	for (int i = 0; i < rows; i++) {
		leftPixels[i].x = SCREEN_WIDTH;
		leftPixels[i].y = min + i;
		rightPixels[i].x = 0;
		rightPixels[i].y = min + i;
	}

	// 4. Loop through all edges of the polygon and use
	// linear interpolation to find the x-coordinate for
	// each row it occupies. Update the corresponding
	// values in rightPixels and leftPixels.
	for (int i = 0; i < V; i++)
	{
		Pixel start = vertexPixels[i];	
		Pixel end = vertexPixels[(i+1) % V];

		int pointsInLine = abs(start.y - end.y) + 1;

		vector<Pixel> line(pointsInLine);
		Interpolate(start, end, line);

		for (Pixel point : line)
		{
			//offset the index
			int j = point.y - min;
			if (point.x < leftPixels[j].x) {
				leftPixels[j].x = point.x;
				leftPixels[j].zinv = point.zinv;
			}
			if (point.x >= rightPixels[j].x)
			{
				rightPixels[j].x = point.x;
				rightPixels[j].zinv = point.zinv;
			}
		}
	}
}

void DrawPolygonRows(const vector<Pixel>& leftPixels, const vector<Pixel>& rightPixels)
{
	for (int i = 0; i < leftPixels.size(); i++)
	{
		DrawLineSDL(screen, leftPixels[i], rightPixels[i], currentColor);
	}
}

void DrawPolygon(const vector<vec3>& vertices)
{
	int V = vertices.size();
	vector<Pixel> vertexPixels(V);
	for (int i = 0; i < V; ++i)
	{
		VertexShader(vertices[i], vertexPixels[i]);
	}
	vector<Pixel> leftPixels;
	vector<Pixel> rightPixels;
	ComputePolygonRows(vertexPixels, leftPixels, rightPixels);
	DrawPolygonRows(leftPixels, rightPixels);
}
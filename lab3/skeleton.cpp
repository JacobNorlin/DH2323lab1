#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::ivec2;
using glm::mat3;
using glm::vec2;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 1024;
SDL_Surface* screen;
int t;
vector<Triangle> triangles;
vec3 cameraPos(0, 0, -3.001);
mat3 R;
float yaw = 0;
vec3 currentColor;
float f = SCREEN_HEIGHT;

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
void VertexShader(const vec3&, ivec2& p);
void Interpolate(ivec2 a, ivec2 b, vector<ivec2>& result);
void DrawLineSDL(SDL_Surface* surface, ivec2 a, ivec2 b, vec3 color);void DrawPolygonEdges(const vector<vec3>& vertices);void rotateVector(vec3& v);void DrawRows(const vector<ivec2>& leftPixels, const vector<ivec2>& rightPixels);void DrawPolygon(const vector<vec3>& vertices);void ComputePolygonRows(const vector<ivec2>& vertexPixels, vector<ivec2>& leftPixels, vector<ivec2>& rightPixels);

int main( int argc, char* argv[] )
{
	LoadTestModel( triangles );
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.
	vector<ivec2> vertexPixels(3);
	vertexPixels[0] = ivec2(10, 5);
	vertexPixels[1] = ivec2(5, 10);
	vertexPixels[2] = ivec2(15, 15);
	vector<ivec2> leftPixels;
	vector<ivec2> rightPixels;
	ComputePolygonRows(vertexPixels, leftPixels, rightPixels);
	for (int row = 0; row<leftPixels.size(); ++row)
	{
		cout << "Start: ("
			<< leftPixels[row].x << ","
			<< leftPixels[row].y << "). "
			<< "End: ("
			<< rightPixels[row].x << ","
			<< rightPixels[row].y << "). " << endl;
	}
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

	if (keystate[SDLK_UP])
	{
		vec3 dt(0, 0, 0.1);
		rotateVector(dt);
		cameraPos += dt;
	}
		
	if (keystate[SDLK_DOWN])
	{
		vec3 dt(0, 0, 0.1);
		rotateVector(dt);
		cameraPos -= dt;
	}

	if (keystate[SDLK_RIGHT])
		yaw += 0.05;

	if (keystate[SDLK_LEFT])
		yaw -= 0.05;

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

void Draw()
{
	SDL_FillRect(screen, 0, 0);
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);
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

void VertexShader(const vec3& v, ivec2& p) {
	vec3 c = (cameraPos - v);
	p.x = f*(c.x / c.z) + (SCREEN_WIDTH / 2);
	p.y = f*(c.y / c.z) + (SCREEN_HEIGHT / 2);

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

void DrawLineSDL(SDL_Surface* surface, ivec2 a, ivec2 b, vec3 color)
{
	ivec2 delta = glm::abs(a - b);
	int pixels = glm::max(delta.x, delta.y) + 1;

	vector<ivec2> line(pixels);
	Interpolate(a, b, line);

	for (int i = 0; i < line.size(); i++)
	{
		PutPixelSDL(screen, line[i].x, line[i].y, color);
	}
}

void DrawPolygonEdges(const vector<vec3>& vertices)
{
	int V = vertices.size();
	// Transform each vertex from 3D world position to 2D image position:
	vector<ivec2> projectedVertices(V);
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

void rotateVector(vec3& v)
{
	vec3 r1(cos(yaw), 0, sin(yaw));
	vec3 r2(0, 1, 0);
	vec3 r3(-sin(yaw), 0, cos(yaw));

	v.x = glm::dot(r1, v);
	v.y = glm::dot(r2, v);
	v.z = glm::dot(r3, v);
}

void ComputePolygonRows(const vector<ivec2>& vertexPixels, vector<ivec2>& leftPixels,
	vector<ivec2>& rightPixels)
{
	int V = vertexPixels.size();

	// 1. Find max and min y-value of the polygon
	// and compute the number of rows it occupies.
	int min = numeric_limits<int>::max();
	int max = numeric_limits<int>::min();

	for (ivec2 point : vertexPixels)
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
		ivec2 start = vertexPixels[i];	
		ivec2 end = vertexPixels[(i+1) % V];

		int pointsInLine = abs(start.y - end.y) + 1;

		vector<ivec2> line(pointsInLine);
		Interpolate(start, end, line);

		for (ivec2 point : line)
		{
			//offset the index
			int j = point.y - min;
			if (point.x < leftPixels[j].x) {
				leftPixels[j].x = point.x;
			}
			if (point.x >= rightPixels[j].x)
			{
				rightPixels[j].x = point.x;
			}
		}
	}
}

void DrawPolygonRows(const vector<ivec2>& leftPixels, const vector<ivec2>& rightPixels)
{
	for (int i = 0; i < leftPixels.size(); i++)
	{
		DrawLineSDL(screen, leftPixels[i], rightPixels[i], currentColor);
	}
}

void DrawPolygon(const vector<vec3>& vertices)
{
	int V = vertices.size();
	vector<ivec2> vertexPixels(V);
	for (int i = 0; i < V; ++i)
	{
		VertexShader(vertices[i], vertexPixels[i]);
	}
	vector<ivec2> leftPixels;
	vector<ivec2> rightPixels;
	ComputePolygonRows(vertexPixels, leftPixels, rightPixels);
	DrawPolygonRows(leftPixels, rightPixels);
}
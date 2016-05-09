#include "Utils.h"
#include <iostream>
#include <consoleapi.h>


void Interpolate(float a, float b, vector<float>& result) 
{
	for (int i = 0; i < result.size(); i++) {
		result[i] = a + step(a, b, result.size())*i;
	}

}

void Interpolate(vec3 a, vec3 b, vector<vec3>& result) {
	for (int i = 0; i < result.size(); i++) {
		float f = result.size() - 1;	
		result[i].x = a.x + step(a.x, b.x, f)*i;
		result[i].y = a.y + step(a.y, b.y, f)*i;
		result[i].z = a.z + step(a.z, b.z, f)*i;
	}
}

float step(float a, float b, int f)
{
	return (b - a) / f;
}

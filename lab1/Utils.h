#include <vector>
#include <glm\glm.hpp>
using glm::vec3;
using namespace std;

void Interpolate(float a, float b, vector<float>& result);
void Interpolate(vec3 a, vec3 b, vector<vec3>& results);
float step(float a, float b, int f);
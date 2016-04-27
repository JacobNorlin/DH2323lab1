#include "stdafx.h"
#include "CppUnitTest.h"
#include <vector>
#include <iostream>
#include "../lab1/Utils.h"
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace lab1tests
{
	template<typename T>
	void AssertVectorEqual(vector<T> a, vector<T> b) {
		for (int i = 0; i < a.size(); i++) {
			Assert::AreEqual(b[i], a[i], L"Message");
		}
	}

	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(floatInterpolateTenInteger)
		{
			vector<float> vec(10);
			float myints[] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
			std::vector<float> corr(myints, myints + sizeof(myints) / sizeof(float));
			float a = 5;
			float b = 15;
			Interpolate(a, b, vec);
			AssertVectorEqual(vec, corr);

			
		}
		TEST_METHOD(floatInterpolateEmptyList)
		{
			vector<float> empty(0);
			vector<float> corr(0);
			float a = 0;
			float b = 0;
			Interpolate(a, b, empty);
			AssertVectorEqual(empty, corr);
		}
		TEST_METHOD(floatInterpolateFiveFloat)
		{
			vector<float> vec(5);
			float myints[] = { 1, 1.2, 1.4, 1.6, 1.8 };
			std::vector<float> corr(myints, myints + sizeof(myints) / sizeof(float));
			float a = 1;
			float b = 2;
			Interpolate(a, b, vec);
			AssertVectorEqual(vec, corr);
		}
		TEST_METHOD(floatInterpolateFlat)
		{
			vector<float> vec(5);
			float myInts[] = { 1,1,1,1,1 };
			vector<float> corr(myInts, myInts + sizeof(myInts) / sizeof(float));
			float a = 1;
			float b = 1;
			Interpolate(a, b, vec);
			AssertVectorEqual(vec, corr);
		}
		TEST_METHOD(vec3InterpolateFour)
		{
			vector<vec3> vec(4);
			vec3 myints[] = { vec3(1.0f, 4.0f, 9.2f), vec3(2.0f, 3.0f, 9.4f), vec3(3.0f, 2.0f, 9.6f), vec3(4.0f, 1.0f, 9.8f) };
			std::vector<vec3> corr(myints, myints + sizeof(myints) / sizeof(vec3));
			vec3 a(1.0f, 4.0f, 9.2f);
			vec3 b(4.0f, 1.0f, 9.8f);
			Interpolate(a, b, vec);
			for (int i = 0; i < corr.size(); i++) {
				Assert::AreEqual(corr[i].x, vec[i].x, L"Message");
			}
		}
		TEST_METHOD(vec3InterpolateEmpty)
		{
			vector<vec3> vec(0);
			vector<vec3> corr(0);
			vec3 a(0, 0, 0);
			vec3 b(0, 0, 0);
			Interpolate(a, b, vec);
			for (int i = 0; i < corr.size(); i++) {
				Assert::AreEqual(corr[i].x, vec[i].x, L"Message");
			}

		}
		TEST_METHOD(vec3InterpolateFlat) {
			vector<vec3> vec(4);
			vec3 myints[] = { vec3(1, 2, 3), vec3(1, 2, 3), vec3(1, 2, 3), vec3(1, 2, 3) };
			std::vector<vec3> corr(myints, myints + sizeof(myints) / sizeof(vec3));
			vec3 a(1, 2, 3);
			vec3 b(1, 2, 3);
			Interpolate(a, b, vec);
			for (int i = 0; i < corr.size(); i++) {
				Assert::AreEqual(corr[i].x, vec[i].x, L"Message");
			}
		}
	};

	
}
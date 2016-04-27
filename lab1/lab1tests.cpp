#include "stdafx.h"
#include "CppUnitTest.h"
#include <vector>
#include "Utils.h"
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace MyTest
{
	TEST_CLASS(MyTests)
	{
	public:
		TEST_METHOD(MyTestMethod)
		{
			vector<float> ten(10);
			float myints[] = { 1, 2, 3, 4, 5, 6, 7, 8 ,9, 10 };
			std::vector<float> correct(myints, myints + sizeof(myints) / sizeof(int));
			float a = 0;
			float b = 10;
			Interpolate(a, b, ten);
			for (int i = 0; i < ten.size(); i++) {
				Assert::AreEqual(ten[i], correct[i], L"Message");
			}
		}
	};
}
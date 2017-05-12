// ArraySmartPointer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ArraySmartPointer.h"

#include <iostream>

ClassArray<int> ClassArray<int>::Singleton;

ArraySmartPointer<int> TestingReturnFromFunction(int input)
{
	ArraySmartPointer<int> out;
	out = input;
	return out;
}

int main()
{
#if true
	{
	int TestNum = 3;
	ClassArray<int>::Reserve(50);
	ArraySmartPointer<int> Test;
	Test = TestingReturnFromFunction(TestNum);
	ArraySmartPointer<int> UnTest;
	UnTest = TestNum;



	ArraySmartPointer<int> ShouldBe3 = UnTest;

	ArraySmartPointer<int> BadTestA = 37;

	ArraySmartPointer<int> BadTestB = 99;

	std::cout << *(ShouldBe3.operator->()) << "\n";

	std::cout << *(BadTestA.operator->()) << "\n";

	std::cout << *(BadTestB.operator->()) << "\n";

}
	system("pause");
#endif

	return 0;
}


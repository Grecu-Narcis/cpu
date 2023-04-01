#include <iostream>

#include "Memory.h"
#include "CPU.h"

int main()
{
	/*Memory test("memory.txt");
	test.loadMemoryFromFile();*/

	CPU testCpu;

	/*vector<int> v = testCpu.decode();

	for (int i = 0; i < 5; i++)
		cout << v[i] << endl;*/
	testCpu.execute();
	testCpu.execute();

	testCpu.getMemory().printMemory();

	//test.printMemory();

	system("pause");

	return 0;
}
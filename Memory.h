#pragma once

#include <map>
#include <string>

using namespace std;

class Memory
{
	map<int, int> instructionsAdresses;
	string fileName;
	int currentAddress;

	int stringToInt(string stringToConvert);

public:
	Memory();
	Memory(string fileName);

	void setFileName(string fileName);

	void loadMemoryFromFile();
	int getValueFromAddress(int address);
	void setMemoryAddress(int address, int value);

	void printMemory();
};


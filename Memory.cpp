#include "Memory.h"

#include <fstream>
#include <iostream>

int Memory::stringToInt(string stringToConvert)
{
	for (int i = 0; i < stringToConvert.size(); i++)
		if (stringToConvert[i] >= 'A' && stringToConvert[i] <= 'Z')
			stringToConvert[i] = stringToConvert[i] - 'A' + 'a';

	int value = 0;

	for (int i = stringToConvert.size() - 1, power = 1; i >= 0; i--, power *= 16)
	{
		if (stringToConvert[i] >= '0' && stringToConvert[i] <= '9')
			value = value + (stringToConvert[i] - '0') * power;

		else
			value = value + (stringToConvert[i] - 'a' + 10) * power;
	}

	return value;
}

Memory::Memory()
{
	this->currentAddress = 0;
	this->fileName = "memory.txt";
}

Memory::Memory(string fileName)
{
	this->fileName = fileName;
	this->currentAddress = 0;
}

void Memory::setFileName(string fileName)
{
	this->fileName = fileName;
}

void Memory::loadMemoryFromFile()
{
	ifstream inputFile(this->fileName);

	if (!inputFile.good())
		throw std::exception("Memory not loaded!");

	string currentLine;

	while (!inputFile.eof())
	{
		getline(inputFile, currentLine);

		if (currentLine == "")
			continue;

		if (currentLine[0] == '#')
		{
			this->currentAddress = this->stringToInt(currentLine.substr(1));
		}

		else
		{
			this->instructionsAdresses[this->currentAddress] = this->stringToInt(currentLine);
			this->currentAddress += 2;
		}
	}
}

int Memory::getValueFromAddress(int address)
{
	if (this->instructionsAdresses.find(address) != this->instructionsAdresses.end())
		return this->instructionsAdresses[address];

	return 0;
}

void Memory::setMemoryAddress(int address, int value)
{
	this->instructionsAdresses[address] = value;
}

void Memory::printMemory()
{
	cout << "Printing memory:\n\n";
	for (auto it = this->instructionsAdresses.begin(); it != this->instructionsAdresses.end(); it++)
		cout << it->first << " " << it->second << "\n";
}

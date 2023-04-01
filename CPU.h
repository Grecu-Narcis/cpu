#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <map>
#include <vector>

#include "Memory.h"

class CPU
{
	uint16_t R[8], IP, SP, stack_base, stack_size;
	uint16_t FLAGS[3];

	Memory systemMemory;
	vector<int> cpuStack;

	map<int, string> instruction;

	void add(vector<int> instructionArgs);
	void sub(vector<int> instructionArgs);
	void mov(vector<int> instructionArgs);
	void mul(vector<int> instructionArgs);
	void div(vector<int> instructionArgs);
	void cmp(vector<int> instructionArgs);
	void jmp(vector<int> instructionArgs);
	void je(vector<int> instructionArgs);
	void jl(vector<int> instructionArgs);
	void jg(vector<int> instructionArgs);
	void jz(vector<int> instructionArgs);
	void call(vector<int> instructionArgs);
	void ret(vector<int> instructionArgs);



public:
	CPU();

	Memory& getMemory();

	void reset();

	int fetch();

	vector<int> decode();

	void execute();

	void load(int addressInMemory, int& registerToLoad);
	void store(int addressInMemory, int registerValue);
};


#include "CPU.h"
#include <iostream>
#include <stdio.h>
#include <Windows.h>

CPU::CPU()
{
	this->reset();

	this->instruction[1] = "add";
	this->instruction[2] = "sub";
	this->instruction[3] = "mov";
	this->instruction[4] = "mul";
	this->instruction[5] = "div";
	this->instruction[6] = "cmp";
	this->instruction[7] = "jmp";
	this->instruction[9] = "je";
	this->instruction[10] = "jl";
	this->instruction[11] = "jg";
	this->instruction[12] = "jz";
	this->instruction[13] = "call";
	this->instruction[14] = "ret";
	this->instruction[15] = "end_sim";
	this->instruction[16] = "push";
	this->instruction[17] = "pop";

	this->systemMemory.loadMemoryFromFile();
}

Memory& CPU::getMemory()
{
	return this->systemMemory;
}


void CPU::reset()
{
	for (int i = 0; i < 8; i++)
		this->R[i] = 0;

	this->FLAGS[ZERO_FLAG] = this->FLAGS[EQUAL_FLAG] = this->FLAGS[GREATER_FLAG] = 0;
	this->stack_base = 10000;
	this->stack_size = 1000;
	this->SP = this->stack_base + 2;

	this->IP = 0xfff0;
}


int CPU::fetch()
{
	int currentInstruction = this->systemMemory.getValueFromAddress(this->IP);
	
	this->IP += 2;

	return currentInstruction;
}


vector<int> CPU::decode()
{
	vector<int> cmdArgs(5);

	int instruction = this->fetch();

	int opCode = instruction >> 10;
	int src1 = (instruction >> 5);
	src1 = src1 & 0x1F;
	int src2 = instruction & 0x1F;

	cmdArgs[0] = opCode;
	cmdArgs[1] = src1;
	cmdArgs[2] = src2;

	if (src1 >= 16)
	{
		int src1Value = this->fetch();
		cmdArgs[3] = src1Value;
	}

	else
	{
		cmdArgs[3] = 0;
	}

	if (src2 >= 16)
	{
		int src2Value = this->fetch();
		cmdArgs[4] = src2Value;
	}

	else
	{
		cmdArgs[4] = 0;
	}

	if (src1 == 0x11 && cmdArgs[3] % 2 == 1)
		throw std::exception("Misalign access!");

	if (src1 == 0x12 && this->R[cmdArgs[3]] % 2 == 1)
		throw std::exception("Misalign access!");

	if (src2 == 0x11 && cmdArgs[4] % 2 == 1)
		throw std::exception("Misalign access!");

	if (src2 == 0x12 && this->R[cmdArgs[4]] % 2 == 1)
		throw std::exception("Misalign access!");

	if (src1 == 0x11 && src2 != 0x0)
	{
		this->changeConsoleColor(FOREGROUND_GREEN);
		printf("Write in memory at address %xh\n\n", cmdArgs[3]);
		this->changeConsoleColor(15);
	}

	else if (src1 == 0x12 && src2 != 0x0)
	{
		this->changeConsoleColor(FOREGROUND_GREEN);
		printf("Write in memory at address %xh\n\n", this->R[cmdArgs[3] - 1]);
		this->changeConsoleColor(15);
	}

	if (src2 == 0x11)
	{
		this->changeConsoleColor(FOREGROUND_GREEN);
		printf("Read from memory at address %xh\n\n", cmdArgs[4]);
		this->changeConsoleColor(15);
	}

	if (src2 == 0x12)
	{
		this->changeConsoleColor(FOREGROUND_GREEN);
		printf("Read from memory at address %xh\n\n", this->R[cmdArgs[4] - 1]);
		this->changeConsoleColor(15);
	}

	return cmdArgs;
}

void CPU::execute()
{
	vector<int> decodedInstruction = this->decode();

	string instructionName = this->instruction[decodedInstruction[0]];

	if (instructionName == "add")
	{
		this->add(decodedInstruction);
	}

	if (instructionName == "sub")
	{
		this->sub(decodedInstruction);
	}

	if (instructionName == "mov")
	{
		this->mov(decodedInstruction);
	}

	if (instructionName == "mul")
	{
		this->mul(decodedInstruction);
	}

	if (instructionName == "div")
	{
		this->div(decodedInstruction);
	}
	
	if (instructionName == "cmp")
	{
		this->cmp(decodedInstruction);
	}
	
	if (instructionName == "jmp")
	{
		this->jmp(decodedInstruction);
	}
	
	if (instructionName == "je")
	{
		this->cmp(decodedInstruction);
	}
	
	if (instructionName == "jl")
	{
		this->jl(decodedInstruction);
	}

	if (instructionName == "jg")
	{
		this->jg(decodedInstruction);
	}

	if (instructionName == "jz")
	{
		this->jz(decodedInstruction);
	}

	if (instructionName == "call")
	{
		this->call(decodedInstruction);
	}

	if (instructionName == "ret")
	{
		this->ret(decodedInstruction);
	}

	if (instructionName == "end_sim")
	{
		this->end_sim();
	}

	if (instructionName == "push")
	{
		this->push(decodedInstruction);
	}

	if (instructionName == "pop")
	{
		this->pop();
	}
}


void CPU::load(int addressInMemory, uint16_t& registerToLoad)
{
	registerToLoad = this->systemMemory.getValueFromAddress(addressInMemory);
}


void CPU::store(int addressInMemory, int registerValue)
{
	this->systemMemory.setMemoryAddress(addressInMemory, registerValue);
}

void CPU::run()
{
	while (true)
	{
		try
		{
			CPU cpuBeforeExecution(*this);

			this->execute();
			this->printCPUStatus(cpuBeforeExecution);
			
			Sleep(50);
		}
		catch (std::exception& e)
		{
			cout << e.what() << endl;
			system("pause");
		}
	}
}


void CPU::add(vector<int> instructionArgs)
{
	int src1 = instructionArgs[1];
	int src2 = instructionArgs[2];
	int src1Value = instructionArgs[3];
	int src2Value = instructionArgs[4];

	if (src1 == 0x10)
		throw std::exception("In add instruction src1 can't be immediate!");

	if (src1 < 0x10 && src2 < 0x10)
	{
		this->R[src1 - 1] += this->R[src2 - 1];

		this->FLAGS[ZERO_FLAG] = this->R[src1 - 1] == 0;
	} 

	else if (src1 < 0x10)
	{
		if (src2 == 0x10)
			this->R[src1 - 1] += src2Value;

		else if (src2 == 0x11)
		{
			int valueToAdd = this->systemMemory.getValueFromAddress(src2Value);

			this->R[src1 - 1] += valueToAdd;
		}

		else if (src2 == 0x12)
		{
			int valueToAdd = this->systemMemory.getValueFromAddress(this->R[src2Value - 1]);
			this->R[src1 - 1] += valueToAdd;
		}

		this->FLAGS[ZERO_FLAG] = this->R[src1 - 1] == 0;
	}

	else
	{
		int addressFromMemory;

		if (src1 == 0x11)
			addressFromMemory = src1Value;
		else
			addressFromMemory = this->R[src1Value - 1];

		if (src2 < 0x10)
		{
			int newValue = this->systemMemory.getValueFromAddress(addressFromMemory) + this->R[src2Value - 1];
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}

		else if (src2 == 0x10)
		{
			int newValue = this->systemMemory.getValueFromAddress(addressFromMemory) + src2Value;
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}

		if (src2 == 0x11)
		{
			int newValue = this->systemMemory.getValueFromAddress(addressFromMemory) + this->systemMemory.getValueFromAddress(src2Value);
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}

		if (src2 == 0x12)
		{
			int newValue = this->systemMemory.getValueFromAddress(addressFromMemory) + this->systemMemory.getValueFromAddress(this->R[src2Value - 1]);
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}

		this->FLAGS[ZERO_FLAG] = this->systemMemory.getValueFromAddress(addressFromMemory) == 0;
	}
}


void CPU::sub(vector<int> instructionArgs)
{
	int src1 = instructionArgs[1];
	int src2 = instructionArgs[2];
	int src1Value = instructionArgs[3];
	int src2Value = instructionArgs[4];

	if (src1 == 0x10)
		throw std::exception("In sub instruction src1 can't be immediate!");

	if (src1 < 0x10 && src2 < 0x10)
	{
		this->R[src1 - 1] -= this->R[src2 - 1];

		this->FLAGS[ZERO_FLAG] = this->R[src1 - 1] == 0;
	}

	else if (src1 < 0x10)
	{
		if (src2 == 0x10)
			this->R[src1 - 1] -= src2Value;

		else if (src2 == 0x11)
		{
			int valueToAdd = this->systemMemory.getValueFromAddress(src2Value);

			this->R[src1 - 1] -= valueToAdd;
		}

		else if (src2 == 0x12)
		{
			int valueToAdd = this->systemMemory.getValueFromAddress(this->R[src2Value - 1]);
			this->R[src1 - 1] -= valueToAdd;
		}

		this->FLAGS[ZERO_FLAG] = this->R[src1 - 1] == 0;
	}

	else
	{
		int addressFromMemory;

		if (src1 == 0x11)
			addressFromMemory = src1Value;
		else
			addressFromMemory = this->R[src1Value - 1];

		if (src2 < 0x10)
		{
			int newValue = this->systemMemory.getValueFromAddress(addressFromMemory) - this->R[src2Value - 1];
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}

		else if (src2 == 0x10)
		{
			int newValue = this->systemMemory.getValueFromAddress(addressFromMemory) - src2Value;
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}

		if (src2 == 0x11)
		{
			int newValue = this->systemMemory.getValueFromAddress(addressFromMemory) - this->systemMemory.getValueFromAddress(src2Value);
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}

		if (src2 == 0x12)
		{
			int newValue = this->systemMemory.getValueFromAddress(addressFromMemory) - this->systemMemory.getValueFromAddress(this->R[src2Value - 1]);
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}

		this->FLAGS[ZERO_FLAG] = this->systemMemory.getValueFromAddress(addressFromMemory) == 0;
	}
}


void CPU::mov(vector<int> instructionArgs)
{
	int src1 = instructionArgs[1];
	int src2 = instructionArgs[2];
	int src1Value = instructionArgs[3];
	int src2Value = instructionArgs[4];

	if (src1 == 0x10)
		throw std::exception("dest can't be immediate!");

	if (src1 < 0x10 && src2 < 0x10)
		this->R[src1 - 1] = this->R[src2 - 1];

	else if (src1 < 0x10)
	{
		if (src2 == 0x10)
			this->R[src1 - 1] = src2Value;

		else if (src2 == 0x11)
		{
			int valueToAdd = this->systemMemory.getValueFromAddress(src2Value);

			this->R[src1 - 1] = valueToAdd;
		}

		else if (src2 == 0x12)
		{
			int valueToAdd = this->systemMemory.getValueFromAddress(this->R[src2Value - 1]);
			this->R[src1 - 1] = valueToAdd;
		}
	}

	else
	{
		int addressFromMemory;

		if (src1 == 0x11)
			addressFromMemory = src1Value;
		else
			addressFromMemory = this->R[src1Value - 1];

		if (src2 < 0x10)
		{
			int newValue = this->R[src2Value - 1];
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}

		else if (src2 == 0x10)
		{
			int newValue = src2Value;
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}

		else if (src2 == 0x11)
		{
			int newValue = this->systemMemory.getValueFromAddress(src2Value);
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}

		else if (src2 == 0x12)
		{
			int newValue = this->systemMemory.getValueFromAddress(this->R[src2Value - 1]);
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}
	}
}


void CPU::mul(vector<int> instructionArgs)
{
	int src1 = instructionArgs[1];
	int src2 = instructionArgs[2];
	int src1Value = instructionArgs[3];
	int src2Value = instructionArgs[4];

	int result = 0;

	if (src1 < 0x10 && src2 < 0x10)
		result = this->R[src1 - 1] * this->R[src2 - 1];

	else if (src1 == 0x10 && src2 == 0x10)
		result = src1Value * src2Value;

	else if (src1 < 0x10)
	{
		if (src2 == 0x10)
			result = this->R[src1 - 1] * src2Value;

		else if (src2 == 0x11)
		{
			int valueToAdd = this->systemMemory.getValueFromAddress(src2Value);

			result = this->R[src1 - 1] * valueToAdd;
		}

		else if (src2 == 0x12)
		{
			int valueToAdd = this->systemMemory.getValueFromAddress(this->R[src2Value - 1]);
			result = this->R[src1 - 1] * valueToAdd;
		}
	}

	else
	{
		int addressFromMemory;

		if (src1 == 0x11)
			addressFromMemory = src1Value;
		else
			addressFromMemory = this->R[src1Value - 1];

		if (src2 < 0x10)
			result = this->systemMemory.getValueFromAddress(addressFromMemory) * this->R[src2Value - 1];

		else if (src2 == 0x10)
			result = this->systemMemory.getValueFromAddress(addressFromMemory) * src2Value;

		if (src2 == 0x11)
			result = this->systemMemory.getValueFromAddress(addressFromMemory) * this->systemMemory.getValueFromAddress(src2Value);

		if (src2 == 0x12)
			result = this->systemMemory.getValueFromAddress(addressFromMemory) * this->systemMemory.getValueFromAddress(this->R[src2Value - 1]);
	}

	this->FLAGS[ZERO_FLAG] = result == 0;

	this->R[1] = result & 0xFFFF;
	this->R[0] = (result >> 16) & 0xFFFF;
}


void CPU::div(vector<int> instructionArgs)
{
	int src1 = instructionArgs[1];
	int src2 = instructionArgs[2];
	int src1Value = instructionArgs[3];
	int src2Value = instructionArgs[4];

	int quotient = 0, remainder = 0;

	if (src2 < 0x10 && this->R[src2 - 1] == 0)
		throw std::exception("Divide by 0!");

	if (src2 == 0x10 && src2Value == 0)
		throw std::exception("Divide by 0!");

	if (src2 == 0x11 && this->systemMemory.getValueFromAddress(src2Value) == 0)
		throw std::exception("Divide by 0!");

	if (src2 == 0x11 && this->systemMemory.getValueFromAddress(src2Value) == 0)
		throw std::exception("Divide by 0!");

	if (src2 == 0x12 && this->systemMemory.getValueFromAddress(this->R[src2Value - 1]) == 0)
		throw std::exception("Divide by 0!");

	if (src1 < 0x10 && src2 < 0x10)
	{
		quotient = this->R[src1 - 1] / this->R[src2 - 1];
		remainder = this->R[src1 - 1] % this->R[src2 - 1];
	}

	else if (src1 == 0x10 && src2 == 0x10)
	{
		quotient = src1Value / src2Value;
		remainder = src1Value % src2Value;
	}

	else if (src1 < 0x10)
	{
		if (src2 == 0x10)
		{
			quotient = this->R[src1 - 1] / src2Value;
			remainder = this->R[src1 - 1] % src2Value;
		}

		else if (src2 == 0x11)
		{
			int valueToAdd = this->systemMemory.getValueFromAddress(src2Value);

			quotient = this->R[src1 - 1] / valueToAdd;
			remainder = this->R[src1 - 1] % valueToAdd;
		}

		else if (src2 == 0x12)
		{
			int valueToAdd = this->systemMemory.getValueFromAddress(this->R[src2Value - 1]);
			quotient = this->R[src1 - 1] / valueToAdd;
			remainder = this->R[src1 - 1] % valueToAdd;
		}
	}

	else
	{
		int addressFromMemory;

		if (src1 == 0x11)
			addressFromMemory = src1Value;
		else
			addressFromMemory = this->R[src1Value - 1];

		if (src2 < 0x10)
		{
			quotient = this->systemMemory.getValueFromAddress(addressFromMemory) / this->R[src2Value - 1];
			remainder = this->systemMemory.getValueFromAddress(addressFromMemory) % this->R[src2Value - 1];
		}

		else if (src2 == 0x10)
		{
			quotient = this->systemMemory.getValueFromAddress(addressFromMemory) / src2Value;
			remainder = this->systemMemory.getValueFromAddress(addressFromMemory) % src2Value;

		}

		if (src2 == 0x11)
		{
			quotient = this->systemMemory.getValueFromAddress(addressFromMemory) / this->systemMemory.getValueFromAddress(src2Value);
			remainder = this->systemMemory.getValueFromAddress(addressFromMemory) % this->systemMemory.getValueFromAddress(src2Value);
		}

		if (src2 == 0x12)
		{
			quotient = this->systemMemory.getValueFromAddress(addressFromMemory) / this->systemMemory.getValueFromAddress(this->R[src2Value - 1]);
			remainder = this->systemMemory.getValueFromAddress(addressFromMemory) % this->systemMemory.getValueFromAddress(this->R[src2Value - 1]);
		}
	}

	this->FLAGS[ZERO_FLAG] = quotient == 0;

	this->R[1] = remainder;
	this->R[0] = quotient;
}


void CPU::cmp(vector<int> instructionArgs)
{
	int src1 = instructionArgs[1];
	int src2 = instructionArgs[2];
	int src1Value = instructionArgs[3];
	int src2Value = instructionArgs[4];

	int firstArg = 0, secondArg = 0;

	if (src1 < 0x10)
		firstArg = this->R[src1 - 1];

	else if (src1 == 0x10)
		firstArg = src1Value;

	else if (src1 == 0x11)
		firstArg = this->systemMemory.getValueFromAddress(src1Value);

	else
		firstArg = this->systemMemory.getValueFromAddress(this->R[src1Value - 1]);


	if (src2 < 0x10)
		secondArg = this->R[src2 - 1];

	else if (src2 == 0x10)
		secondArg = src2Value;

	else if (src2 == 0x11)
		secondArg = this->systemMemory.getValueFromAddress(src2Value);

	else
		secondArg = this->systemMemory.getValueFromAddress(this->R[src2Value - 1]);


	if (firstArg == secondArg)
		this->FLAGS[EQUAL_FLAG] = 1;
	else
		this->FLAGS[EQUAL_FLAG] = 0;

	if (firstArg == 0 && secondArg == firstArg)
		this->FLAGS[ZERO_FLAG] = 1;
	else
		this->FLAGS[ZERO_FLAG] = 0;

	if (firstArg > secondArg)
		this->FLAGS[GREATER_FLAG] = 1;
	else
		this->FLAGS[GREATER_FLAG] = 0;
}


void CPU::jmp(vector<int> instructionArgs)
{
	int src1 = instructionArgs[1];
	int src1Value = instructionArgs[3];

	int newIP = this->getValueOfArgument(src1, src1Value);

	this->IP = newIP;
}


void CPU::je(vector<int> instructionArgs)
{
	if (this->FLAGS[EQUAL_FLAG] == 0)
		return;

	this->jmp(instructionArgs);
}


void CPU::jl(vector<int> instructionArgs)
{
	if (this->FLAGS[EQUAL_FLAG] == 1 || this->FLAGS[GREATER_FLAG] == 1)
		return;

	this->jmp(instructionArgs);
}


void CPU::jg(vector<int> instructionArgs)
{
	if (this->FLAGS[GREATER_FLAG] == 0)
		return;

	this->jmp(instructionArgs);
}


void CPU::jz(vector<int> instructionArgs)
{
	if (this->FLAGS[ZERO_FLAG] == 0)
		return;

	this->jmp(instructionArgs);
}


void CPU::call(vector<int> instructionArgs)
{
	this->push(this->IP);
	
	for (int i = 0; i < 3; i++)
		this->push(this->FLAGS[i]);

	for (int i = 0; i < 8; i++)
		this->push(this->R[i] + 2);

	this->jmp(instructionArgs);
}


void CPU::ret(vector<int> instructionArgs)
{
	vector<int> argsForMove(5);

	for (int i = 7; i >= 0; i--)
	{
		this->pop();
		
		this->R[i] = this->R[2];
	}

	for (int i = 2; i >= 0; i--)
	{
		this->pop();
		this->FLAGS[i] = this->R[2];
	}

	this->pop();
	this->IP = this->R[2]; // equivalent to jmp
}


void CPU::push(vector<int> instructionArgs)
{
	int valueToPush = this->getValueOfArgument(instructionArgs[1], instructionArgs[3]);

	this->push(valueToPush);
}

void CPU::push(int valueToPush)
{
	this->SP -= 2;

	this->store(this->SP, valueToPush);

	this->changeConsoleColor(FOREGROUND_GREEN);
	printf("Pushed to stack the value %xh\nNew stack pointer have value: %xh\n\n", valueToPush, this->SP);
	this->changeConsoleColor(15);
}


void CPU::pop()
{
	this->load(this->SP, this->R[2]);
	this->SP += 2;

	this->changeConsoleColor(FOREGROUND_GREEN);
	printf("Poped from stack the value %xh\nNew stack pointer have value: %xh\n\n", this->R[2], this->SP);
	this->changeConsoleColor(15);
}


void CPU::end_sim()
{
	system("pause");
	exit(0);
}

int CPU::getValueOfArgument(int src, int srcValue)
{
	if (src < 0x10)
		return this->R[src - 1];

	else if (src == 0x10)
		return srcValue;

	else if (src == 0x11)
		return this->systemMemory.getValueFromAddress(srcValue);

	else
		return this->systemMemory.getValueFromAddress(this->R[srcValue - 1]);
}


void CPU::changeConsoleColor(int color)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, color);
}


void CPU::printCPUStatus(CPU cpuBeforeExecution)
{
	printf("Registers:\n");
	for (int i = 0; i < 8; i++)
	{
		if (this->R[i] != cpuBeforeExecution.R[i])
			changeConsoleColor(FOREGROUND_GREEN);

		printf("Register %d: %xh\n", i, this->R[i]);
	
		changeConsoleColor(15);
	}

	if (this->IP != cpuBeforeExecution.IP)
	{
		changeConsoleColor(FOREGROUND_GREEN);
		printf("Register IP: %xh\n", this->IP);
		changeConsoleColor(15);
	}

	else
		printf("Register IP: %xh\n", this->IP);


	if (this->SP != cpuBeforeExecution.SP)
	{
		changeConsoleColor(FOREGROUND_GREEN);
		printf("Register SP: %xh\n", this->SP);
		changeConsoleColor(15);
	}

	else
		printf("Register SP: %xh\n", this->SP);


	printf("\nFlags:\n");
	printf("Zero Flag: %d\n", this->FLAGS[ZERO_FLAG]);
	printf("Equal Flag: %d\n", this->FLAGS[EQUAL_FLAG]);
	printf("Greater Flag: %d\n", this->FLAGS[GREATER_FLAG]);
	printf("\n");
}


#include "CPU.h"
#include <iostream>

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

	this->SP = 0;
	this->FLAGS[0] = this->FLAGS[1] = this->FLAGS[2] = 0;
	this->stack_base = 0;
	this->stack_size = 0;

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
	}

	if (instructionName == "push")
	{
	}

	if (instructionName == "pop")
	{
	}
}


void CPU::load(int addressInMemory, int& registerToLoad)
{
	registerToLoad = this->systemMemory.getValueFromAddress(addressInMemory);
}


void CPU::store(int addressInMemory, int registerValue)
{
	this->systemMemory.setMemoryAddress(addressInMemory, registerValue);
}


void CPU::add(vector<int> instructionArgs)
{
	int src1 = instructionArgs[1];
	int src2 = instructionArgs[2];
	int src1Value = instructionArgs[3];
	int src2Value = instructionArgs[4];

	if (src1 == 0x10)
		throw std::exception("src1 can't be immediate!");

	if (src1 < 0x10 && src2 < 0x10)
	{
		this->R[src1 - 1] += this->R[src2 - 1];

		this->FLAGS[0] = this->R[src1 - 1] == 0;
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

		this->FLAGS[0] = this->R[src1 - 1] == 0;
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

		this->FLAGS[0] = this->systemMemory.getValueFromAddress(addressFromMemory) == 0;
	}
}


void CPU::sub(vector<int> instructionArgs)
{
	int src1 = instructionArgs[1];
	int src2 = instructionArgs[2];
	int src1Value = instructionArgs[3];
	int src2Value = instructionArgs[4];

	if (src1 == 0x10)
		throw std::exception("src1 can't be immediate!");

	if (src1 < 0x10 && src2 < 0x10)
	{
		this->R[src1 - 1] -= this->R[src2 - 1];

		this->FLAGS[0] = this->R[src1 - 1] == 0;
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

		this->FLAGS[0] = this->R[src1 - 1] == 0;
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

		this->FLAGS[0] = this->systemMemory.getValueFromAddress(addressFromMemory) == 0;
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

		if (src2 == 0x11)
		{
			int newValue = this->systemMemory.getValueFromAddress(src2Value);
			this->systemMemory.setMemoryAddress(addressFromMemory, newValue);
		}

		if (src2 == 0x12)
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

	this->FLAGS[0] = result == 0;

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
		throw std::exception("Src2 == 0 not allowed!");

	if (src2 == 0x10 && src2Value == 0)
		throw std::exception("Src2 == 0 not allowed!");

	if (src2 == 0x11 && this->systemMemory.getValueFromAddress(src2Value) == 0)
		throw std::exception("Src2 == 0 not allowed!");

	if (src2 == 0x11 && this->systemMemory.getValueFromAddress(src2Value) == 0)
		throw std::exception("Src2 == 0 not allowed!");

	if (src2 == 0x12 && this->systemMemory.getValueFromAddress(this->R[src2Value - 1]) == 0)
		throw std::exception("Src2 == 0 not allowed!");

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

	this->FLAGS[0] = quotient == 0;

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
		this->FLAGS[1] = 1;
	else
		this->FLAGS[1] = 0;

	if (firstArg == 0 && secondArg == firstArg)
		this->FLAGS[0] = 1;
	else
		this->FLAGS[0] = 0;

	if (firstArg > secondArg)
		this->FLAGS[2] = 1;
	else
		this->FLAGS[2] = 0;
}


void CPU::jmp(vector<int> instructionArgs)
{
	int src1 = instructionArgs[1];
	int src1Value = instructionArgs[3];

	//if (src < 0x10)
}


void CPU::je(vector<int> instructionArgs)
{
}


void CPU::jl(vector<int> instructionArgs)
{
}


void CPU::jg(vector<int> instructionArgs)
{
}


void CPU::jz(vector<int> instructionArgs)
{
}


void CPU::call(vector<int> instructionArgs)
{
}


void CPU::ret(vector<int> instructionArgs)
{
}


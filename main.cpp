#include <iostream>

#include "Memory.h"
#include "CPU.h"

int main()
{
	CPU testCpu;

	//testCpu.run();

	/*cout << "\n\nSecond CPU starts running...\n\n";*/

	CPU secondTestCpu("demo_errors.txt");
	secondTestCpu.run();

	system("pause");

	return 0;
}

/*
#fff0
1e00
0500
#ff48
1610 - div 2 0 => divide by 0
0002
0000
0c71 - mov r2, [16]
0010
0610 - add [1] [2]
0001
0002
0c70 - mov r2 12h
0012
0a10 - sub 1 4
0001
0004
0c70 - mov r2 14h 
0014
0631 - add [561] [291]
0231
0123
3c00
#500
0c50
0001
0c70
0002
0450
000a
0d10
0003
0cb0
000c
0cd0
0001
08a6
0c90
0000
1090
000a
04d0
0004
4040
4060
4080
40a0
40c0
1e00
1000
#1000
0c50
0010
0c62
0c90
0001
1e00
0400
#400
0864
3200
0100
1e00
0400
#100
0d10
0300
0503
0ce8
0cb2
0007
0910
0002
0cd2
0008
18a6
2e00
0ab0
1e00
1200
#ab0
0c25
0ca6
0cc1
0e45
0007
0e46
0008
0c50
0002
1e00
1200
#1200
0850
0001
3200
0900
1e00
1200
#900
1843
3600
0010
#10
1e00
ff48
*/
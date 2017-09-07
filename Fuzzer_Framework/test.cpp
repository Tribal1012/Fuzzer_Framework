#include <cstdio>
#include <iostream>
#include "fuzzer.h"

using namespace std;
#define TEST_PROCESS "C:\\WINDOWS\\system32\\notepad.exe"
#define TEST_CMDLINE NULL
#define TEST_PID 8076

int main(int argc, char* argv[]) {
	/*
	if(argc < 2) {
		fprintf(stderr, "Usage : %s [program]", argv[0]);
		return(-1);
	}*/
	char str[2048] = { 0, };
	memset(str, 0x42, 10);
	memset(str + 10, 0x43, 1000);
	//Fuzzer file_fuzz;
	//Fuzzer network_fuzz;
	Mutator mutator;

	//std::cout << "Before : " << str << std::endl;
	mutator.CreateMutatedFile("aaa");
	//std::cout << "After : " << str << std::endl;
	//file_fuzz.File_Fuzzer();
	//network_fuzz.Network_Fuzzer(TEST_PID);


	return(0);
}

#include <cstdio>
#include "fuzzer.h"

using namespace std;
#define TEST_PROCESS "C:\\WINDOWS\\system32\\notepad.exe"
#define TEST_CMDLINE NULL
#define TEST_PID 11111

int main(int argc, char* argv[]) {
	/*
	if(argc < 2) {
		fprintf(stderr, "Usage : %s [program]", argv[0]);
		return(-1);
	}*/

	Fuzzer file_fuzz;
	//Fuzzer network_fuzz;

	file_fuzz.File_Fuzzer(TEST_PROCESS, TEST_CMDLINE);
	//network_fuzz.Network_Fuzzer(TEST_PID);

	return(0);
}

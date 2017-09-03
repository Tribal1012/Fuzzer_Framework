#include <cstdio>
#include "fuzzer.h"

using namespace std;

bool Fuzzer::File_Fuzzer(char* FileName, char* arg) {
	Open_Process(FileName, arg);

	return(true);
}

bool Fuzzer::File_Fuzzer(unsigned int pid) {
	Attach_Process(pid);
	return(true);
}

bool Fuzzer::Network_Fuzzer() {
	return(true);
}

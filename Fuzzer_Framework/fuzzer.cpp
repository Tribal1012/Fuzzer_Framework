#include <cstdio>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <regex>
#include "fuzzer.h"

using namespace std;

Fuzzer::Fuzzer() {
	dword fsize = 0;
	ifstream ifs("./test.yaml");
	string fdata;
	if(ifs.is_open()) {
		ifs >> fdata;
		ifs.close();

		regex reg("^():()");
		smatch m;

		memset(file_ext, 0, _MAX_EXT);
		memset(orig_path, 0, MAX_PATH);
		memset(mutated_path, 0, MAX_PATH);
		memset(result_path, 0, MAX_PATH);
		crash = 0;
		timeout = 5000;
	}
}

bool Fuzzer::Mutater() {
	return(true);
}

bool Fuzzer::File_Fuzzer() {
	bool isstop = false;

	while(!isstop) {
		Mutater();
		//Open_Process(target_program, mutated_path);
		while(!DebugStart());
		//CloseProcess();
		break;
	}

	return(true);
}

bool Fuzzer::Network_Fuzzer(dword pid) {
	bool isstop = false;

	while(!isstop) {
		if(!Attach_Process(pid)) {
			fprintf(stderr, "[-] Program Attach failed.\n");
			break;
		}
		while(!DebugStart());
		CloseProcess();
		break;
	}

	return(true);
}

/*
 *	DebugStart()
 *	실행된 또는 Attached Process Debug 수행 
 *	Error 반환
 */
#define Err_Wait 0xfffffff
#define Sucess_Debug 0x0
#define Access_Violation 0x1
#define Stack_Overflow 0x2
dword Fuzzer::DebugStart() {
	ContinueDebugEvent(
		DebugEvent.dwProcessId, 
		DebugEvent.dwThreadId, 
		DBG_CONTINUE);

	if(WaitForDebugEvent(&DebugEvent, 3000) == NULL)
		return Err_Wait;
	
	//printf("%d\n", DebugEvent.dwDebugEventCode);
	switch(DebugEvent.dwDebugEventCode) {
	case EXCEPTION_DEBUG_EVENT:
		switch(DebugEvent.u.Exception.ExceptionRecord.ExceptionCode) {
		case EXCEPTION_ACCESS_VIOLATION :
			GetThreadContext(TargetProcessInfo.hThread, &context);
			return Access_Violation;
		case EXCEPTION_STACK_OVERFLOW:
			GetThreadContext(TargetProcessInfo.hThread, &context);
			return Stack_Overflow;
		case EXCEPTION_INT_OVERFLOW:
			GetThreadContext(TargetProcessInfo.hThread, &context);
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			GetThreadContext(TargetProcessInfo.hThread, &context);
			break;
		}
	}

	return Sucess_Debug;
}


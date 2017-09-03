#include <cstdio>
#include "fuzzer.h"

using namespace std;

bool Fuzzer::Mutater() {
	return(true);
}

bool Fuzzer::File_Fuzzer(char* FileName, char* arg) {
	bool isstop = false;

	while(!isstop) {
		Mutater();
		Open_Process(FileName, arg);
		do {
		}while(!DebugStart());
		CloseProcess();
		break;
	}

	return(true);
}

bool Fuzzer::Network_Fuzzer(unsigned int pid) {
	bool isstop = false;

	while(!isstop) {
		Attach_Process(pid);
		do {
		}while(!DebugStart());
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
unsigned int Fuzzer::DebugStart() {
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
			break;
		case EXCEPTION_INT_OVERFLOW:
			GetThreadContext(TargetProcessInfo.hThread, &context);
			break;
		case EXCEPTION_STACK_OVERFLOW:
			GetThreadContext(TargetProcessInfo.hThread, &context);
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			GetThreadContext(TargetProcessInfo.hThread, &context);
			break;
		}
	}

	return Sucess_Debug;
}


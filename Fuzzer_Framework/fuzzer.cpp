#include <cstdio>
#include <cstring>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <regex>
#include "fuzzer.h"

using namespace std;

#define BUF_SIZE 512

/*
 *	Fuzzer::Fuzzer()
 *	설정 파일을 읽어들여, 초기화 수행
 */
Fuzzer::Fuzzer() {
	ifstream ifs("./test.yaml");
	char fdata[BUF_SIZE];
	if(ifs.is_open()) {
		while (!ifs.eof()) {
			memset(fdata, 0, BUF_SIZE);
			ifs.getline(fdata, BUF_SIZE);

			regex reg("^(\\w+?): ([\\w:\\\\ ()]+)");
			string fdata_str = fdata;
			smatch m;
			
			bool ismatched = regex_search(fdata_str, m, reg);

			if (ismatched) {
				char** dummy = NULL;
				if(!strcmp(m[1].str().c_str(), "result_path")) strncpy_s(result_path, m[2].str().c_str(), MAX_PATH-1);
				else if(!strcmp(m[1].str().c_str(), "crash")) crash = strtoul(m[2].str().c_str(), dummy, 10);
				else if(!strcmp(m[1].str().c_str(), "timeout")) timeout = (dword)(strtof(m[2].str().c_str(), dummy) * 1000);
			}
		}

		ifs.close();
	}
}

/*
 *	File_Fuzzer()
 *	파일 퍼징 수행
 *	정상 종료 : true, 비정상 종료 : false
 */
bool Fuzzer::File_Fuzzer() {
	bool isstop = false;

	while(!isstop) {
		//Mutater();
		//Open_Process(target_program, mutated_path);
		while(!DebugStart());
		//CloseProcess();
		break;
	}

	return(true);
}

/*
 *	Check_Server_Crash()
 *	서버의 크래시 체크
 *	정상 종료 : true, 비정상 종료 : false
 */
bool Fuzzer::Check_Server_Crash() {
	

	return(true);
}

/*
 *	Network_Fuzzer()
 *	네트워크 패킷 퍼저
 *	정상 종료 : true, 비정상 종료 : false
 */
bool Fuzzer::Network_Fuzzer() {
	

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


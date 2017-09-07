#include <cstdio>
#include <cstring>
#include <Windows.h>
#include <TlHelp32.h>
#include "debugger.h"

using namespace std;

/*
 *	Debugger::Debugger()
 *	�ʱ�ȭ ����
 */
Debugger::Debugger() {
	memset(&ProcessWindowInfo, NULL, sizeof(STARTUPINFO));
	ProcessWindowInfo.cb = sizeof(STARTUPINFO);
	memset(&TargetProcessInfo, NULL, sizeof(PROCESS_INFORMATION));
	memset(&DebugEvent, NULL, sizeof(DEBUG_EVENT));
	memset(&context, NULL, sizeof(CONTEXT));
	#ifdef _WIN32
		#ifdef _WIN64
			isWow64Process = false;
		#endif
	#else
	#endif
	isattached = false;
}

/*
 *	Debugger::~Debugger()
 *	
 */
Debugger::~Debugger() {
	CloseProcess();
}

/*
 *	Open_Process()
 *	Debug ���� �� ���μ��� ����
 *	���� �� true, ���� �� false ��ȯ
 */
bool Debugger::Open_Process(
	const LPCSTR ApplicationName, 
	const LPSTR CmdLine) 
{
	if(CreateProcess(
		ApplicationName,
		CmdLine, 
		NULL, 
		NULL, 
		FALSE, 
		CREATE_NEW_CONSOLE | DEBUG_ONLY_THIS_PROCESS | PROCESS_TERMINATE, 
		NULL, 
		NULL, 
		&ProcessWindowInfo, 
		&TargetProcessInfo)) 
	{
		printf("[+] cmd> %s %s\n", ApplicationName, CmdLine);
	}
	else {
		puts("[-] CreateProcess() Error\n");
		return(false);
	}

	isattached = false;

	return(true);
}

/*
 *	ProcessView()
 *	���� ���� ���μ��� Ȯ��
 */
void Debugger::ProcessView() {
	HANDLE hSnapShot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 ProcessEntry;

	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	hSnapShot = CreateToolhelp32Snapshot(
							TH32CS_SNAPALL, 
							NULL);
	Process32First(hSnapShot, &ProcessEntry);
	puts("[+] Process List");
	puts("������������������������������������������������");
	do {
		printf("��%u. %s\n", ProcessEntry.th32ProcessID, ProcessEntry.szExeFile);
	} while(Process32Next(hSnapShot, &ProcessEntry));
	puts("������������������������������������������������\n");
	CloseHandle(hSnapShot);
}

/*
 *	Attach_Process()
 *	�̹� ����� ���μ����� attach
 *	���� �� true, ���� �� false ��ȯ
 */
bool Debugger::Attach_Process(const dword pid) {
	HANDLE hSnapShot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 ProcessEntry;
	HANDLE hProcess;
	dword dwsize = 0;

	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	hSnapShot = CreateToolhelp32Snapshot(
							TH32CS_SNAPALL, 
							NULL);
	Process32First(hSnapShot, &ProcessEntry);
	do {
		if(ProcessEntry.th32ProcessID == pid) {
			printf("[+] Attached Process : %s\n", ProcessEntry.szExeFile);
			TargetProcessInfo.dwProcessId = ProcessEntry.th32ProcessID;
			break;
		}
	} while(Process32Next(hSnapShot, &ProcessEntry));		// ���ڿ� ������ Process Ž��
	CloseHandle(hSnapShot);

	if(TargetProcessInfo.dwProcessId == 0) {
		fprintf(stderr, "[-] This Process Not Found\n");
		return(false);
	}
	
	hProcess = OpenProcess(
		PROCESS_ALL_ACCESS, 
		FALSE, 
		TargetProcessInfo.dwProcessId);
	if(hProcess == NULL) {
		fprintf(stderr, "[-] OpenProcess Error...\n");
		return(false);
	}
	ZeroMemory(target_program, MAX_PATH);
	dwsize = MAX_PATH;
	QueryFullProcessImageName(hProcess, 0, (LPSTR)target_program, (PDWORD)&dwsize);

	TargetProcessInfo.hProcess = hProcess;
	DebugActiveProcess(TargetProcessInfo.dwProcessId);
	isattached = true;

	return(true);
}

/*
 *	CloseProcess()
 *	Debugger�� ����� Process ���� 
 */
bool Debugger::CloseProcess() {
	if(isattached) 
		DebugActiveProcessStop(TargetProcessInfo.dwProcessId);
	else
		TerminateProcess(TargetProcessInfo.hProcess, 0);

	CloseHandle(TargetProcessInfo.hProcess);

	return(true);
}

/*
 *	SetSingleStep()
 *	SingleStep�� ���� SingleStep ���� 
 */
#define TrapBit 0x100
bool Debugger::SetSingleStep() {
	context.EFlags |= TrapBit;
	if(!SetThreadContext(TargetProcessInfo.hThread, &context)) {
		context.EFlags ^= TrapBit;
		return false;
	}
	return true;
}

/*
 *	DelSingleStep()
 *	SingleStep�� ���� SingleStep ���� 
 */
bool Debugger::DelSingleStep() {
	context.EFlags ^= TrapBit;
	if(!SetThreadContext(TargetProcessInfo.hThread, &context)) {
		context.EFlags |= TrapBit;
		return false;
	}
	return true;
}

/*
 *	DebugStart()
 *	����� �Ǵ� Attached Process Debug ���� 
 *	Error ��ȯ
 */
#define Err_Wait 0xfffffff
#define Sucess_Debug 0x0
dword Debugger::DebugStart() {
	ContinueDebugEvent(
		DebugEvent.dwProcessId, 
		DebugEvent.dwThreadId, 
		DBG_CONTINUE);

	if(WaitForDebugEvent(&DebugEvent, 3000) == NULL)
		return Err_Wait;

	//test code
    switch(DebugEvent.dwDebugEventCode)
    {
    //Create Process
    case CREATE_PROCESS_DEBUG_EVENT:        //3
        //Store Process Status
		puts("CREATE_PROCESS_DEBUG_EVENT");
        break;
    //Exit Process
    case EXIT_PROCESS_DEBUG_EVENT:        //5
        puts("EXIT_PROCESS_DEBUG_EVENT");
		break;
    //Occur Debug Exception
    case EXCEPTION_DEBUG_EVENT:            //1
        switch(DebugEvent.u.Exception.ExceptionRecord.ExceptionCode)
        {
        //Single Step Execute
        case EXCEPTION_SINGLE_STEP:
			puts("EXCEPTION_SINGLE_STEP");
            break;
        //BreakPonint
        case EXCEPTION_BREAKPOINT:
			puts("EXCEPTION_BREAKPOINT");
            break;
        default:
            break;
        }
	}
	/*
	switch(DebugEvent.dwDebugEventCode) {
	case EXCEPTION_DEBUG_EVENT:
		switch(DebugEvent.u.Exception.ExceptionRecord.ExceptionCode) {
		case EXCEPTION_ACCESS_VIOLATION :
			GetThreadContext(TargetProcessInfo.hThread, &context);
			break;
		}
	}*/

	return Sucess_Debug;
}

#ifndef __FUZZER_H__
#define __FUZZER_H__
#include <Windows.h>

typedef unsigned int dword;

class Debugger {
private:
	#ifdef _WIN32
		#ifdef _WIN64
			BOOL isWow64Process;
		#endif
	#else
	#endif
	BOOL isattached;

protected:
	STARTUPINFO ProcessWindowInfo;
	PROCESS_INFORMATION TargetProcessInfo;
	DEBUG_EVENT DebugEvent;
	CONTEXT context;
	char target_program[MAX_PATH];

protected:
	bool Open_Process(
		const LPCSTR ApplicationName, 
		const LPSTR CmdLine);
	virtual void ProcessView();
	bool Attach_Process(const dword pi);
	bool CloseProcess();
	bool SetSingleStep();
	bool DelSingleStep();
	virtual dword DebugStart(void);

public:
	Debugger();
	~Debugger();
};

class Fuzzer: public Debugger
{
private:
	char file_ext[_MAX_EXT];
	char orig_path[MAX_PATH];
	char mutated_path[MAX_PATH];
	char result_path[MAX_PATH];
	dword crash;
	dword timeout;		// ms

protected:
	virtual dword DebugStart(void);
	bool Mutater();

public:
	Fuzzer();
	bool File_Fuzzer();
	bool Network_Fuzzer(dword pid);
};

#endif

#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#include <Windows.h>
typedef unsigned long dword;

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
#endif

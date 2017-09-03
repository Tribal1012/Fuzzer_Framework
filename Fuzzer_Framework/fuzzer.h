#ifndef __FUZZER_H__
#define __FUZZER_H__
#include <Windows.h>

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

protected:
	bool Open_Process(
		const LPCSTR ApplicationName, 
		const LPSTR CmdLine);
	virtual void ProcessView();
	bool Attach_Process(const unsigned int pi);
	bool CloseProcess();
	bool SetSingleStep();
	bool DelSingleStep();
	virtual unsigned int DebugStart(void);

public:
	Debugger();
	~Debugger();
};

class Fuzzer: public Debugger
{
private:
	bool Mutater();
protected:
	virtual unsigned int DebugStart(void);

public:
	bool File_Fuzzer(char* FileName, char* arg);
	bool Network_Fuzzer(unsigned int pid);
};

#endif

#ifndef __FUZZER_H__
#define __FUZZER_H__
#include <Windows.h>

class Debugger {
private:
	STARTUPINFO ProcessWindowInfo;
	PROCESS_INFORMATION TargetProcessInfo;
	DEBUG_EVENT DebugEvent;
	CONTEXT context;
	#ifdef _WIN32
		#ifdef _WIN64
			BOOL isWow64Process;
		#endif
	#else
	#endif
	BOOL isattached;

protected:
	bool Open_Process(
		const LPCSTR ApplicationName, 
		const LPSTR CmdLine);
	virtual void ProcessView();
	bool Attach_Process(const unsigned int pi);
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

protected:
	
public:
	bool File_Fuzzer(char* FileName, char* arg);
	bool File_Fuzzer(unsigned int pid);
	bool Network_Fuzzer();
};

#endif

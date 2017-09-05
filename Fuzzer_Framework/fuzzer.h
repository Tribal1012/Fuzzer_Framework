#ifndef __FUZZER_H__
#define __FUZZER_H__
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

class Mutator
{
private:

protected:
	char	orig_path[MAX_PATH];
	char**	orig_file_list;
	char	mutated_path[MAX_PATH];
	char	file_name[_MAX_FNAME];
	char	file_ext[_MAX_EXT];

public:
	Mutator();
	dword GetFileList();
	unsigned int GenRandomValue();
	bool Mutation(char* data, dword dsize);
	~Mutator();
}

class Fuzzer: public Debugger, Mutator
{
private:
	char result_path[MAX_PATH];
	dword crash;
	dword timeout;		// ms
	bool isconfigured;

protected:
	virtual dword DebugStart(void);

public:
	Fuzzer();
	bool File_Fuzzer();
	bool Check_Server_Crash();
	bool Network_Fuzzer();
};

#endif

#ifndef __FUZZER_H__
#define __FUZZER_H__
#include <Windows.h>
#include "debugger.h"
#include "mutator.h"


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

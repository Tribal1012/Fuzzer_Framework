#ifndef __MUTATOR_H__
#define __MUTATOR_H__

#include <Windows.h>

typedef unsigned long dword;

class Mutator
{
private:

protected:
	char	orig_path[MAX_PATH];
	char*	orig_file_list;
	char*	mutated_file_list;
	char	mutated_path[MAX_PATH];
	char	file_name[_MAX_FNAME];
	char	file_ext[_MAX_EXT];

public:
	Mutator();
	dword GetFileList();
	dword GenRandomValue(dword dsize);
	dword Mutation(char* data, const dword dsize);
	bool CreateMutatedFile(char* FileName);
	~Mutator();
};

void InitWELLRNG512a(unsigned int *init);
double WELLRNG512a();

#endif

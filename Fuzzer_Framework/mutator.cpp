#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>

#pragma comment(lib, "crypt32.lib")
#include <Wincrypt.h>
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
#include "fuzzer.h"

using namespace std;

#define BUF_SIZE 512

/*
 *	Mutator::Mutator()
 *	생성자, 설정 파일을 읽어들임
 */
Mutator::Mutator() {
	ifstream ifs("./test.yaml");
	char fdata[BUF_SIZE];
	if(ifs.is_open()) {
		while (!ifs.eof()) {
			memset(fdata, 0, BUF_SIZE);
			ifs.getline(fdata, BUF_SIZE);

			regex reg("^(\\w+?): (\\w+)");
			string fdata_str = fdata;
			smatch m;

			bool ismatched = regex_search(fdata_str, m, reg);

			if (ismatched) {
				if(!strcmp(m[1], "orig_path")) strncpy(orig_path, m[2], MAX_PATH-1);
				else if(!strcmp(m[1], "mutated_path")) strncpy(mutated_path, m[2], MAX_PATH-1);
			}
		}

		orig_file_list = NULL;
		cout << "Original_Path" << orig_path<< endl;
		cout << "Mutated_Path" << mutated_path<< endl;
		//cout << "" << << endl;
		//cout << "" << << endl;
		
		ifs.close();
	}
}

/*
 *	Mutator::~Mutator()
 *	소멸자
 */
Mutator::~Mutator() {
	if(orig_file_list != NULL)
		delete orig_file_list;
	orig_file_list = NULL;
}

/*
 *	GetFileList()
 *	Original Path에서 Seed 파일의 목록을 읽어들임
 *	읽어들인 파일의 갯수를 반환
 */
#define EXIST		1
#define NOT_EXIST	-1
dword Mutator::GetFileList() {
	dword fcount = 0;
	_finddata_t fd;
	long fd_handle;
	int isexisted = EXIST;

	fd_handle = _findfirst(".\\*.*", & fd);
	if(fd_handle == -1) {
		fprintf(stderr, "Not Found...\n");
		goto ret_count;
	}

	while(isexisted != NOT_EXIST) fcount++;
	orig_file_list = new char[fcount][MAX_PATH];
	isexisted = EXIST;
	fcount = 0;
	while(isexisted != NOT_EXIST) {
		strncpy(orig_file_list[fcount], fd.name, MAX_PATH);
		fcount++;
	}

ret_count:
	return(fcount);
}

/*
 *	GenRandomValue()
 *	랜덤 값 생성
 *	4바이트 랜덤 값 생성
 */
#define CVTVALUE(x) (x) % 0xffffffff
unsigned int Mutator::GenRandomValue() {
	HCRYPTPROV   hCryptProv;
	unsigned int pbData;

	if(CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0)) {
		fprintf(stderr, "CryptAcquireContext succeeded. \n");
	} else {
		fprintf(stderr, "Error during CryptAcquireContext!\n");
	}

	if(CryptGenRandom(hCryptProv, 4, &pbData)) {
        fprintf(stderr, "Random number is: %d.\n", CVTVALUE(pbData));
	} else {
		fprintf(stderr, "Error during CryptGenRandom.\n");
		return(0);
	}

	if(hCryptProv) {
		if (!CryptReleaseContext(hCryptProv, 0)) {
			fprintf(stderr, "Failed CryptReleaseContext\n");
		}
	}

	return CVTVALUE(pbData);
}


/*
 *	Mutation()
 *	Mutation을 수행후, 저장
 *	Mutation 성공 여부 반환
 */
bool Mutator::Mutation(char* data, dword dsize) {
	dword offset = GenRandomValue();

	return(true);
}


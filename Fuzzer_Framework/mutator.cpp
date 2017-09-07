#include <cstdio>
#include <cstring>
#include <iostream>
#include <regex>
#include <fstream>
#include <io.h>
#include <conio.h>
#include <ctime>
#include <random>
#include <functional>
#include "mutator.h"
#include "cryptohash.h"

using namespace std;

#define BUF_SIZE 512

/*
 *	Mutator::Mutator()
 *	생성자, 설정 파일을 읽어들임
 */
Mutator::Mutator() {
	memset(orig_path, 0, MAX_PATH);
	memset(mutated_path, 0, MAX_PATH);
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
				if(!strcmp(m[1].str().c_str(), "orig_path")) strncpy_s(orig_path, m[2].str().c_str(), MAX_PATH-1);
				else if(!strcmp(m[1].str().c_str(), "mutated_path")) strncpy_s(mutated_path, m[2].str().c_str(), MAX_PATH-1);
			}
		}

		orig_file_list = NULL;
		cout << "[+] Original_Path : " << orig_path << endl;
		cout << "[+] Mutated_Path : " << mutated_path << endl;
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
		delete[] orig_file_list;
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

	char* orig_file_path = new char[MAX_PATH];
	memset(orig_file_path, 0, MAX_PATH);
	strcpy(orig_file_path, orig_path);
	strcat(orig_file_path, "\\*.*");
	fd_handle = _findfirst(orig_file_path, & fd);
	if(fd_handle == -1) {
		fprintf(stderr, "Not Found...\n");
		goto ret_count;
	}

	while (isexisted != NOT_EXIST) {
		//cout << "File Name : " << fd.name << endl;
		isexisted = _findnext(fd_handle, &fd);
		fcount++;
	}
	_findclose(fd_handle);

	orig_file_list = new char[(fcount-2) * MAX_PATH];
	mutated_file_list = new char[(fcount-2) * MAX_PATH];
	
	fd_handle = _findfirst(orig_file_path, &fd);
	isexisted = _findnext(fd_handle, &fd);
	isexisted = _findnext(fd_handle, &fd);
	isexisted = EXIST;
	for (fcount = 0; isexisted != NOT_EXIST; fcount++) {
		strncpy_s(orig_file_list + fcount*MAX_PATH, MAX_PATH, fd.name, MAX_PATH - 1);
		isexisted = _findnext(fd_handle, &fd);
	}
	_findclose(fd_handle);
ret_count:
	delete[] orig_file_path;
	return(fcount);
}

/*
 *	GenRandomValue()
 *	랜덤 값 생성
 */
#define WELLRANDOMLONG(x) (dword)((double)WELLRNG512a() * 1000000000) % dsize
dword Mutator::GenRandomValue(dword dsize) {
	random_device rd;
	mt19937 engine(rd());
	uniform_int_distribution<int> distribution(0, dsize);
	auto generator = bind(distribution, engine);
	unsigned int init[16];
	for (int i = 0; i < 16; i++)
		init[i] = generator();
	InitWELLRNG512a(init);

	return WELLRANDOMLONG(dsize);
}

/*
 *	Mutation()
 *	Mutation을 수행후, 저장
 *	Mutation된 data 길이 반환
 */
dword Mutator::Mutation(char* data, const dword dsize) {
	dword offset = GenRandomValue(dsize);
	BYTE  reverse_case = (BYTE)GenRandomValue(0x100);
	data[offset] ^= reverse_case;

	offset = GenRandomValue(dsize);
	dword dummy_len = GenRandomValue(1000);
	char* tmp_mem = new char[dsize - offset];
	char* dummy = new char[dummy_len];
	memset(dummy, 0x41, dummy_len);
	memcpy(tmp_mem, &data[offset], dsize - offset);
	memcpy(&data[offset], dummy, dummy_len);
	memcpy(&data[offset + dummy_len], tmp_mem, dsize - offset);

	delete tmp_mem;
	delete dummy;

	return(dsize + dummy_len);
}

/*
*	CreateMutatedFile()
*	Original File List로 부터 Mutation을 통해 MutatedFile 생성
*	파일 생성 여부 확인
*/
bool Mutator::CreateMutatedFile(char* FileName) {
	char orig_full_path[MAX_PATH];
	char mutated_full_path[MAX_PATH];

	dword file_count = GetFileList();
	if (file_count == 0) return(false);
	for (int i = 0; i < file_count; i++) {
		memset(orig_full_path, 0, MAX_PATH);
		memset(mutated_full_path, 0, MAX_PATH);
		if (strlen(&orig_file_list[i*MAX_PATH]) + strlen(orig_path) < MAX_PATH) {
			sprintf_s(orig_full_path, "%s\\%s", orig_path, &orig_file_list[i*MAX_PATH]);
			//cout << "[+] Original Full Path[i] : " << orig_full_path << endl;
		} else {
			fprintf(stderr, "[-] File Path Error...\n");
			return(false);
		}

		crypto::sha1_helper_t hash_helper;
		strncpy_s(&mutated_file_list[i*MAX_PATH], MAX_PATH, hash_helper.hexdigesttext(&orig_file_list[i*MAX_PATH], TRUE).c_str(), MAX_PATH-1);
		if (strlen(&mutated_file_list[i*MAX_PATH]) + strlen(mutated_path) < MAX_PATH) {
			sprintf_s(mutated_full_path, "%s\\%s", mutated_path, &mutated_file_list[i*MAX_PATH]);
			//cout << "[+] Mutated Full Path[i] : " << mutated_full_path << endl;
		} else {
			fprintf(stderr, "File Path Error...\n");
			return(false);
		}

		ifstream ifs(orig_full_path, ifstream::binary | ifstream::in);
		unsigned int fsize = 0;
		char* fdata = NULL;
		if (ifs.is_open()) {
			ifs.seekg(0, ios::end);
			fsize = (unsigned int)ifs.tellg();
			ifs.seekg(0, ios::beg);

			fdata = new char[fsize + 1024];
			memset(fdata, 0, fsize + 1024);
			ifs.read(fdata, fsize);
			ifs.close();

			Mutation(fdata, fsize);
			ofstream ofs(mutated_full_path, ofstream::binary | ofstream::out | ofstream::trunc);
			if (ofs.is_open()) {
				//while(!ofs.eof())
				ofs.write(fdata, fsize);
				ofs.close();
			}
			delete[] fdata;
		}
	}

	return(true);
}

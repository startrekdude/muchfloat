#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

#include "gmp/gmp.h"

#define DURATION 10000
#define SAMPLE_SIZE 1000

// GMP needs this
FILE _iob[] = { 0, 0, 0 };
uint32_t initIob = 0;
FILE * __cdecl __iob_func(void) {
	if (!initIob) {
		_iob[0] = *stdin;
		_iob[1] = *stdout;
		_iob[2] = *stderr;
		initIob = 1;
	}
	return _iob;
}

typedef struct _THREAD_RESULT {
	uint64_t operationCount;
	char* result;
} THREAD_RESULT;

THREAD_RESULT *results;

DWORD WINAPI DoWork(uint32_t index) {
	uint32_t start = GetTickCount();
	uint64_t ops = 0;
	mpq_t acc;
	mpq_t unit;
	mpq_set_str(acc, "1/10", 10);
	mpq_set(unit, acc);
	
	while (GetTickCount() - start < DURATION) {
		for (uint32_t i = 0; i < SAMPLE_SIZE; i++) {
			mpq_add(acc, acc, unit);
		}
		ops += SAMPLE_SIZE;
	}
	results[index].operationCount = ops;
	
	uint32_t len = mpz_sizeinbase(mpq_numref(acc), 10);
	len += mpz_sizeinbase(mpq_denref(acc), 10) + 3;
	
	results[index].result = (char*)malloc(len);
	mpq_get_str(results[index].result, 10, acc);
	
	return 0;
}

int wmain(int agrc, wchar_t **argv) {
	_putws(L"MuchGMP v0.9");
	
	SYSTEM_INFO sys;
	GetSystemInfo(&sys);
	DWORD procCnt = sys.dwNumberOfProcessors;
	wprintf(L"Number of processors: %u\n", procCnt);
	
	results = malloc(sizeof(THREAD_RESULT)*procCnt);
	
	for(uint32_t i = 0; i < procCnt; i++) {
		CreateThread(NULL, 0, DoWork, i, 0, NULL);
	}
	
	Sleep(DURATION + 1000);
	
	uint64_t sum = 0;
	for(uint32_t i = 0; i < procCnt; i++) {
		//wprintf(L"Thread %u: %Iu operations, result was %f\n",
		//	i, results[i].operationCount, results[i].result);
		wprintf(L"Thread: %u: %Iu operations, result was ", i, results[i].operationCount);
		puts(results[i].result);
		sum += results[i].operationCount;
		free(results[i].result);
	}
	wprintf(L"Total: %Iu operations\n", sum);
	
	free(results);
}
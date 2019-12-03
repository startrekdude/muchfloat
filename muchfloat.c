#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <immintrin.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

#define DURATION 10000
#define SAMPLE_SIZE 1000

typedef struct _THREAD_RESULT {
	uint64_t operationCount;
	double result;
} THREAD_RESULT;

THREAD_RESULT *results;

DWORD WINAPI DoWork(uint32_t index) {
	uint32_t start = GetTickCount();
	
	__m256d acc = _mm256_set_pd(0.1f, 0.1f, 0.1f, 0.1f);
	__m256d unit = _mm256_set_pd(0.1, 0.1f, 0.1f, 0.1f);
	uint64_t ops = 0;
	
	while (GetTickCount() - start < DURATION) {
		for (uint32_t i = 0; i < SAMPLE_SIZE; i++) {
			acc = _mm256_add_pd(acc, unit);
		}
		ops += SAMPLE_SIZE*4;
	}
	
	double accs[] = {0.1f, 0.1f, 0.1f, 0.1f};
	_mm256_store_pd(accs, acc);
	double sum = accs[0] + accs[1] + accs[2] + accs[3];
	
	results[index].operationCount = ops;
	results[index].result = sum;
	
	return 0;
}

int wmain(int agrc, wchar_t **argv) {
	_putws(L"MuchFloat v0.9");
	
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
		wprintf(L"Thread %u: %Iu operations, result was %f\n",
			i, results[i].operationCount, results[i].result);
		sum += results[i].operationCount;
	}
	wprintf(L"Total: %Iu operations\n", sum);
	
	free(results);
}
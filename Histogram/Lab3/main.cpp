#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <iostream>
#include <random>
#include <vector>

#include "DS_definitions.h"
#include "DS_timer.h"

void publishRandomNumber(std::vector<float>&data, int dataSize);
void showResult(std::vector<int>&bins, std::string s);
void serialAlgorithm(std::vector<float>&data, int bin_size);
void parallelAlgorithm_v1(std::vector<float>&data, int thread_num, int bin_size);
void parallelAlgorithm_v2(std::vector<float>&data, int thread_num, int bin_size);
void parallelAlgorithm_v3(std::vector<float>&data, int thread_num, int bin_size);

int main(int argc, char const *argv[]){
    DS_timer timer(4, 1);

	timer.setTimerName(0, (char*)"Serial Algorithm");
	timer.setTimerName(1, (char*)"Parallel Algorithm version1");
	timer.setTimerName(2, (char*)"Parallel Algorithm version2");
	timer.setTimerName(3, (char*)"Parallel Algorithm version3");

	if (argc < 4) {
		printf("It requires one arguments\n");
		printf("Usage: Extuction_file, data_size, thread_num, bin_size\n");
		return -1;
	}

	int bin_size = atoi(argv[3]);
	int thread_num = atoi(argv[2]);
    int dataSize = atoi(argv[1]);
    std::vector<float> data(dataSize);
	publishRandomNumber(data, dataSize);

	// Serial algorithm
	timer.onTimer(0);
	serialAlgorithm(data, bin_size);
	timer.offTimer(0);

	// Pallael algorithm version1
	timer.onTimer(1);
	parallelAlgorithm_v1(data, thread_num, bin_size);
	timer.offTimer(1);

	// Pallael algorithm version2
	timer.onTimer(2);
	parallelAlgorithm_v2(data, thread_num, bin_size);
	timer.offTimer(2);
	
	// Pallael algorithm version3
	timer.onTimer(3);
	parallelAlgorithm_v3(data, thread_num, bin_size);
	timer.offTimer(3);
	
	// Performance Comparison
    timer.printTimer();
	EXIT_WIHT_KEYPRESS;
    return 0;
}

/*
* 0~10 사이의 부동소수를 랜덤하게 입력받은 크기만큼 생성
* 생성된 값은 data에 저장
*/
void publishRandomNumber(std::vector<float>&data, int dataSize){
	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, 10);

	#pragma omp parallel for num_threads(4)
	for(int i = 0; i < dataSize; ++i)
		data[i] = dist(gen);
}


// 분류한 데이터를 출력 
void showResult(std::vector<int>&bins, std::string s){
	std::cout << s;
	for(int i = 0; i < bins.size(); ++i)
		std::cout << bins[i] << ", ";
	std::cout << "\n";
}

//직렬 데이터 분류
void serialAlgorithm(std::vector<float>&data, int bin_size){
	std::vector<int>bins(bin_size);

	for(int i = 0; i < data.size(); ++i)
		++bins[int(data[i] / 10 * bin_size)];

	showResult(bins, "<Serial_res>\n");
}

/*
* version 1
* 병렬연산을 통한 데이터 분류
* critical section에 대하여 atomic을 이용하여 Synchronization
*/
void parallelAlgorithm_v1(std::vector<float>&data, int thread_num, int bin_size){
	std::vector<int>bins(bin_size);

	#pragma omp parallel for num_threads(thread_num)
	for(int i = 0; i < data.size(); ++i){
		#pragma omp atomic
		++bins[int(data[i] / 10 * bin_size)];
	}

	showResult(bins, "<Parallel_v1_res>\n");
}

/*
* version 2
* 병렬연산을 통한 데이터 분류
* 스레드 로컬에 공간을 할당 후 각 구간 분류
* 베리어 없이 끝나는 대로 순차적 데이터 병합
*/
void parallelAlgorithm_v2(std::vector<float>&data, int thread_num, int bin_size){
	std::vector<int>bins(bin_size);

	omp_lock_t sumLock;
	omp_init_lock(&sumLock);

	#pragma omp parallel num_threads(thread_num)
	{
		int*my_space = new int[bin_size];

		for(int i = 0; i < bin_size; ++i)
			my_space[i] = 0;

		#pragma omp for nowait
		for(int i = 0; i < data.size(); ++i)
			++my_space[int(data[i] / 10 * bin_size)];

		omp_set_lock(&sumLock);
		for(int c = 0; c < bin_size; ++c)
			bins[c] += my_space[c];
		omp_unset_lock(&sumLock);
	}
	omp_destroy_lock(&sumLock);
	
	showResult(bins, "<Parallel_v2_res>\n");
}

/*
* version 3
* 병렬연산을 통한 데이터 분류
* 스레드 로컬에 공간을 할당 후 각 구간 분류
* 위의 작업이 다 끝나면 다시 스레드 별로 특정 구간 값에 대하여 병렬 합산
*/
void parallelAlgorithm_v3(std::vector<float>&data, int thread_num, int bin_size){
	std::vector<int>bins(bin_size);
	int**each_thread = new int*[thread_num];

	#pragma omp parallel num_threads(thread_num)
	{
		int*my_space = new int[bin_size];
		each_thread[omp_get_thread_num()] = my_space;

		for(int i = 0; i < bin_size; ++i)
			my_space[i] = 0;

		#pragma omp for
		for(int i = 0; i < data.size(); ++i)
			++my_space[int(data[i] / 10 * bin_size)];
		
		#pragma omp for
		for(int c = 0; c < bin_size; ++c)
			for(int r = 0; r < thread_num; ++r)
				bins[c] += each_thread[r][c];
	}

	showResult(bins, "<Parallel_v3_res>\n");
}
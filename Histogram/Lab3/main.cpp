#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <iostream>
#include <random>
#include <vector>

#include "DS_definitions.h"
#include "DS_timer.h"

void publishRandomNumber(std::vector<float>&data, int dataSize);
void serialAlgorithm(std::vector<float>&data, std::vector<int>&bins);
void parallelAlgorithm_v1(std::vector<float>&data, std::vector<int>&bins, int thread_num);
void parallelAlgorithm_v2(std::vector<float>&data, std::vector<int>&bins, int thread_num);
void parallelAlgorithm_v3(std::vector<float>&data, std::vector<int>&bins, int thread_num);

int main(int argc, char const *argv[]){
    DS_timer timer(4, 1);

	timer.setTimerName(0, "Serial Algorithm");
	timer.setTimerName(1, "Parallel Algorithm version1");
	timer.setTimerName(2, "Parallel Algorithm version2");
	timer.setTimerName(3, "Parallel Algorithm version3");

	if (argc < 1) {
		printf("It requires five arguments\n");
		printf("Usage: data_size\n");
		return -1;
	}

	int thread_num = 4;
    int dataSize = atoi(argv[1]);
    std::vector<float> data(dataSize);
	publishRandomNumber(data, dataSize);

    std::vector<int> serial_bins(10);
	std::vector<int> parallel_bins_v1(10);
	std::vector<int> parallel_bins_v2(10);
	std::vector<int> parallel_bins_v3(10);

	// Serial algorithm
	timer.onTimer(0);
	serialAlgorithm(data, serial_bins);
	timer.offTimer(0);

	// Pallael algorithm version1
	timer.onTimer(1);
	parallelAlgorithm_v1(data, parallel_bins_v1, thread_num);
	timer.offTimer(1);

	// Pallael algorithm version2
	timer.onTimer(2);
	parallelAlgorithm_v2(data, parallel_bins_v2, thread_num);
	timer.offTimer(2);

	// Pallael algorithm version3
	timer.onTimer(3);
	parallelAlgorithm_v3(data, parallel_bins_v3, thread_num);
	timer.offTimer(3);

	// Performance Comparison
	std::cout << "<Serial_res>\n";
	for(int i = 0; i < 10; ++i)
		std::cout << serial_bins[i] << ", ";

	std::cout << "\n";

    std::cout << "<Parallel_v1_res>\n";
	for(int i = 0; i < 10; ++i)
		std::cout << parallel_bins_v1[i] << ", ";	

	std::cout << "\n";

    std::cout << "<Parallel_v2_res>\n";
	for(int i = 0; i < 10; ++i)
		std::cout << parallel_bins_v2[i] << ", ";	

	std::cout << "\n";

    std::cout << "<Parallel_v3_res>\n";
	for(int i = 0; i < 10; ++i)
		std::cout << parallel_bins_v3[i] << ", ";	

    timer.printTimer();
	EXIT_WIHT_KEYPRESS;
    return 0;
}

void publishRandomNumber(std::vector<float>&data, int dataSize){
	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, 10);

	for(int i = 0; i < dataSize; ++i){
		data[i] = dist(gen);
		//std::cout << data[i] << ", ";
	}

	//std::cout << "\n";
}

void serialAlgorithm(std::vector<float>&data, std::vector<int>&bins){
	for(int i = 0; i < data.size(); ++i)
		++bins[floor(data[i])];
}

void parallelAlgorithm_v1(std::vector<float>&data, std::vector<int>&bins, int thread_num){
	omp_lock_t sumLock;
	omp_init_lock(&sumLock);

	#pragma omp parallel for num_threads(thread_num)
	for(int i = 0; i < data.size(); ++i){
		omp_set_lock(&sumLock);
		++bins[floor(data[i])];
		omp_unset_lock(&sumLock);
	}
	omp_destroy_lock(&sumLock);
}

void parallelAlgorithm_v2(std::vector<float>&data, std::vector<int>&bins, int thread_num){
	std::vector<std::vector<int>> sum(thread_num, std::vector<int>(10));

	#pragma omp parallel num_threads(thread_num)
	{
		std::vector<int>&my_space = sum[omp_get_thread_num()];
		#pragma omp for
		for(int i = 0; i < data.size(); ++i){
			++my_space[floor(data[i])];
		}
	}

	for(int r = 0; r < thread_num; ++r)
		for(int c = 0; c < 10; ++c)
			bins[c] += sum[r][c];
}

void parallelAlgorithm_v3(std::vector<float>&data, std::vector<int>&bins, int thread_num){
	std::vector<std::vector<int>> sum(thread_num, std::vector<int>(10));

	#pragma omp parallel num_threads(thread_num)
	{
		std::vector<int>&my_space = sum[omp_get_thread_num()];
		#pragma omp for
		for(int i = 0; i < data.size(); ++i){
			++my_space[floor(data[i])];
		}
	}

	#pragma omp parallel for num_threads(thread_num)
	for(int r = 0; r < thread_num; ++r)
		for(int c = 0; c < 10; ++c)
			bins[c] += sum[r][c];
}
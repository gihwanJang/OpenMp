#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "DS_timer.h"
#include "DS_definitions.h"

// Set the size of matrix and vector
// matrix A = m by n
// vector b = n by 1
#define m (10000)
#define n (10000)

#define GenFloat (rand() % 100 + ((float)(rand() % 100) / 100.0))
void genRandomInput();

float A[m][n];
float X[n];
float Y_serial[m];
float Y_parallel[m];

int main(int argc, char** argv)
{
	DS_timer timer(2);
	timer.setTimerName(0, (char*)"Serial");
	timer.setTimerName(1, (char*)"Parallel");

	genRandomInput();
	/*
	std::cout << "A matrix" << "\n";
	for(int r = 0; r < 10; ++r){
		for(int c = 0; c < 10; ++c)
			std::cout << A[r][c] << " ";
		std::cout << "\n";
	}

	std::cout << "X matrix" << "\n";s
	for(int c = 0; c < 10; ++c)
		std::cout << X[c] << "\n";
	*/

	//** 1. Serial code **//
	timer.onTimer(0);


	//** HERE
	//** Write your code implementing the serial algorithm here
	for(int r = 0; r < m; ++r)
		for(int c = 0; c < n; ++c)
			Y_serial[r] += A[r][c] * X[c];
	/*
	std::cout << "serial" << "\n";
	for(int c = 0; c < 10; ++c)
		std::cout << Y_serial[c] << "\n";
	*/
	timer.offTimer(0);

	//** 2. Parallel code **//
	timer.onTimer(1);


	//** HERE
	//** Write your code implementing the parallel algorithm here
	#pragma omp parallel num_threads(16)
	{
		#pragma omp for
		for(int r = 0; r < m; ++r)
			for(int c = 0; c < n; ++c)
				Y_parallel[r] += A[r][c] * X[c];
	}
	/*
	std::cout << "parallel" << "\n";
	for(int c = 0; c < 10; ++c)
		std::cout << Y_parallel[c] << "\n";
	*/
	timer.offTimer(1);



	//** 3. Result checking code **//
	bool isCorrect = true;

	//** HERE
	//** Wriet your code that compares results of serial and parallel algorithm
	// Set the flag 'isCorrect' to true when they are matched
	for(int r = 0; r < m; ++r)
		if(abs(Y_serial[r] - Y_parallel[r]) > 0.00000001){
			std::cout << Y_serial[r] << ":" << Y_parallel[r] << "\n";
			isCorrect = false;
			break;
		}
	
	if (!isCorrect)
		printf("Results are not matched :(\n");
	else
		printf("Results are matched! :)\n");

	timer.printTimer();
	EXIT_WIHT_KEYPRESS;
}

void genRandomInput(void) {
	// A matrix
	LOOP_INDEX(row, m) {
		LOOP_INDEX(col, n) {
			A[row][col] = GenFloat;
		}
	}

	LOOP_I(n)
		X[i] = GenFloat;

	memset(Y_serial, 0, sizeof(float) * m);
	memset(Y_parallel, 0, sizeof(float) * m);
}
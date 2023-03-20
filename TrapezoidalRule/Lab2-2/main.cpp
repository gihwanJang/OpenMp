#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <iostream>
#include <vector>
#include <cmath>

#include "DS_timer.h"
#include "DS_definitions.h"

double function(double x){
    return x * x;
}

double Trapezoidal(double a, double b, double h){
    return (function(a) + function(b)) * h / 2;
}

int main(int argc, char const *argv[]){
    // input
    int n = 1000000;
    double a = -2;
    double b = 2;
    double h = (b - a) / n;
    // output
    double serial_res = 0;
    double parallel_res = 0;
    // thread
    int thread_num = 8;
    std::vector<double> space(thread_num);
    // timer
    DS_timer timer(2);
    timer.setTimerName(0, (char*)"Serial");
	timer.setTimerName(1, (char*)"Parallel");

    // Serial algorithm
    timer.onTimer(0);

    for(int i = 0; i < n; ++i)
            serial_res += Trapezoidal(a + i*h, a + (i+1)*h, h);

    timer.offTimer(0);

    //Parallel algorithm
    timer.onTimer(1);

    #pragma omp parallel num_threads(thread_num)
    {   
        #pragma omp for
        for(int i = 0; i < n; ++i)
            space[omp_get_thread_num()] += Trapezoidal(a + i*h, a + (i+1)*h, h);
    }

    for(int i = 0; i < thread_num; ++i)
        parallel_res += space[i];
        
    timer.offTimer(1);

    // Performance Comparison
    std::cout << "Serial_res : " << serial_res << "\n";
    std::cout << "Parallel_res : " << parallel_res << "\n";

    timer.printTimer();
	EXIT_WIHT_KEYPRESS;
    return 0;
}

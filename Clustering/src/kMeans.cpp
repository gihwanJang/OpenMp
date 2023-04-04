#include <limits>
#include <ctime> 
#include <iostream>
#include <vector>

#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

struct Point {
    double x, y;
    int cluster;
    double minDistance;

    Point() :
        x(0.0),
        y(0.0),
        cluster(-1),
        minDistance(std::numeric_limits<double>::max()) {}

    double distance(Point p) {
        return (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y);
    }
};

Point data[100000];
Point centroids[5];

unsigned short pseudo_rand() {
    static unsigned long long seed = 5;
    return (seed = seed * 25214903917ULL + 11ULL) >> 16;
}

void build() {
    int alpha = pseudo_rand();
    int beta = pseudo_rand();

    int loop = 0;
    while (loop < 100000) {
        int tmp = 0;
        for (int i = 0; i < 3; i++) tmp += pseudo_rand() % 100;

        data[loop].x = (double)((tmp / 3 + alpha) % 100) + (pseudo_rand() % 1000000) / 1000000.;
        data[loop].y = (double)((tmp / 3 + beta) % 100) + (pseudo_rand() % 1000000) / 1000000.;
        loop++;
    }
}

void kMeansClustering(int epoch) {
    /* Init cluster */
    for (int i = 0; i < 5; i++)
        centroids[i] = data[pseudo_rand() % 100000];

    while (epoch--) {
        /* Assign Points to a cluster */
        for (int i = 0; i < 5; i++) {
            for (register int idx = 0; idx < 100000; idx++) {
                double dist = centroids[i].distance(data[idx]);
                if (dist < data[idx].minDistance) {
                    data[idx].minDistance = dist;
                    data[idx].cluster = i;
                }
            }
        }

        /* Computing new centroids */
        int point_cnt[5] = { 0, };
        double sum_x[5] = { 0, };
        double sum_y[5] = { 0, };

        for (register int i = 0; i < 100000; i++) {
            int cluster_id = data[i].cluster;
            point_cnt[cluster_id] += 1;
            sum_x[cluster_id] += data[i].x;
            sum_y[cluster_id] += data[i].y;

            data[i].minDistance = std::numeric_limits<double>::max();
        }

        for (int i = 0; i < 5; i++) {
            centroids[i].x = sum_x[i] / point_cnt[i];
            centroids[i].y = sum_y[i] / point_cnt[i];
        }
    }
}

int main(int argc, char const *argv[]){
	plt::plot({1,2,3,4}, "*");
	plt::show();
	return 0;
}

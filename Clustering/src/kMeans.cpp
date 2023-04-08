#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <limits>
#include <ctime>
#include <cmath>

struct Point {
    int x, y;
    int cluster;
    int minDistance;

    Point() :x(0),y(0),cluster(-1), minDistance(std::numeric_limits<int>::max()){}

    double distance(Point p) {
        return (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y);
    }
};

void dataGenerate(std::vector<Point>&data, int size, int centroid) {
    srand(time(NULL));
    Point center;
    int total = 0, x_rand, y_rand;

    while(centroid--){
        int cluster_size = centroid == 0 ? size : rand() % size;

        center.x = rand() % 100000;
        center.y = rand() % 100000;

        for(int i = 0; i < cluster_size; ++i){
            x_rand = rand() % 5000+1;
            y_rand = rand() % 5000+1;
            data[total+i].x = center.x + (rand() % x_rand) * (x_rand%2 == 0 ? 1 : -1);
            data[total+i].y = center.y + (rand() % y_rand) * (y_rand%2 == 0 ? 1 : -1);
        }

        size -= cluster_size;
        total += cluster_size;
    }
}

void kMeansClustering(std::vector<Point>&data, int k) {
    std::vector<Point> centroids(k);
    std::vector<Point> prev(k);

    int loop = 100000;

    for (int i = 0; i < k; ++i)
        centroids[i] = data[rand() % data.size()];

    while(loop--) {
        for(int i = 0; i < k; ++i){
            prev[i].x = centroids[i].x;
            prev[i].y = centroids[i].y;
        }

        for (int i = 0; i < k; ++i)
            for (int idx = 0; idx < data.size(); ++idx) {
                int dist = centroids[i].distance(data[idx]);
                if (dist < data[idx].minDistance) {
                    data[idx].minDistance = dist;
                    data[idx].cluster = i;
                }
            }

        int point_cnt[k] = { 0, };
        int sum_x[k] = { 0, };
        int sum_y[k] = { 0, };

        for (int i = 0; i < data.size(); ++i) {
            int cluster_id = data[i].cluster;
            point_cnt[cluster_id] += 1;
            sum_x[cluster_id] += data[i].x;
            sum_y[cluster_id] += data[i].y;

            data[i].minDistance = std::numeric_limits<int>::max();
        }

        for (int i = 0; i < k; ++i) 
            if(point_cnt[i]){
                if(sum_x[i] > 0)
                    centroids[i].x = sum_x[i] / point_cnt[i];
                if(sum_y[i] > 0)
                    centroids[i].y = sum_y[i] / point_cnt[i];
            }
    }
}

int main(int argc, char const *argv[]){
    int dataSize = 200;
    int k = 3;

    std::cout<< "start generate" << "\n";
    std::vector<Point> data(dataSize);
	dataGenerate(data, dataSize, 3);
    std::cout<< "end generate" << "\n";
    kMeansClustering(data,k);

    std::ofstream myfile;
    myfile.open("k-means_output.csv");
    myfile << "x,y,c" << "\n";

    for (int i = 0; i < dataSize; ++i) {
        myfile << data[i].x << "," << data[i].y << "," << data[i].cluster << "\n";
    }
    myfile.close();
	return 0;
}

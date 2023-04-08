#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <limits>
#include <ctime>
#include <cmath>
#include <algorithm>

struct Point{
    int x, y;
    int cluster;
    int minDistance;

    Point() : x(0), y(0), cluster(-1), minDistance(std::numeric_limits<int>::max()) {}

    double distance(Point p){
        return (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y);
    }
};

void dataGenerate(std::vector<Point> &data, int size, int centroid){
    srand(time(NULL));
    Point center;
    int total = 0, x_rand, y_rand;

    while (centroid--){
        int cluster_size = centroid == 0 ? size : rand() % size;

        center.x = rand() % 100000;
        center.y = rand() % 100000;

        for (int i = 0; i < cluster_size; ++i){
            x_rand = rand() % 5000 + 1;
            y_rand = rand() % 5000 + 1;
            data[total + i].x = center.x + (rand() % x_rand) * (x_rand % 2 == 0 ? 1 : -1);
            data[total + i].y = center.y + (rand() % y_rand) * (y_rand % 2 == 0 ? 1 : -1);
        }

        size -= cluster_size;
        total += cluster_size;
    }
}

void kMeansClustering(std::vector<Point> &data, int k, int iterations, std::vector<Point> &centroids){
    std::vector<Point> prev(k);

    while (iterations--){
        for (int i = 0; i < k; ++i){
            prev[i].x = centroids[i].x;
            prev[i].y = centroids[i].y;
        }

        for (int i = 0; i < k; ++i)
            for (int idx = 0; idx < data.size(); ++idx){
                int dist = centroids[i].distance(data[idx]);
                if (dist < data[idx].minDistance){
                    data[idx].minDistance = dist;
                    data[idx].cluster = i;
                }
            }

        int point_cnt[k] = {0,};
        int sum_x[k] = {0,};
        int sum_y[k] = {0,};

        for (int i = 0; i < data.size(); ++i){
            int cluster_id = data[i].cluster;
            point_cnt[cluster_id] += 1;
            sum_x[cluster_id] += data[i].x;
            sum_y[cluster_id] += data[i].y;

            data[i].minDistance = std::numeric_limits<int>::max();
        }

        for (int i = 0; i < k; ++i)
            if (point_cnt[i]){
                if(sum_x[i] > 0)
                    centroids[i].x = sum_x[i] / point_cnt[i];
                if(sum_y[i] > 0)
                    centroids[i].y = sum_y[i] / point_cnt[i];
            }
    }
}

void clara(std::vector<Point> &data, int k, int sample_size, int iterations){
    std::vector<Point> sample_data(sample_size);
    std::vector<Point> sample_centroids(k);
    std::vector<Point> best_centroids(k);
    double min_cost = std::numeric_limits<double>::max();

    for (int i = 0; i < iterations; ++i){
        std::random_shuffle(data.begin(), data.end());
        std::copy(data.begin(), data.begin() + sample_size, sample_data.begin());

        kMeansClustering(sample_data, k, 1000, sample_centroids);

        double cost = 0;
        for (int idx = 0; idx < data.size(); ++idx){
            double min_distance = std::numeric_limits<double>::max();
            
            for (int j = 0; j < k; ++j){
                double distance = data[idx].distance(sample_centroids[j]);
                if (distance < min_distance)
                    min_distance = distance;
            }
            cost += min_distance;
        }

        if (cost < min_cost){
            min_cost = cost;
            std::copy(sample_centroids.begin(), sample_centroids.end(), best_centroids.begin());
        }
    }

    kMeansClustering(data, k, 1000, best_centroids);
}

int main(int argc, char const *argv[]){
    int dataSize = 200;
    int k = 3;
    int sampleSize = 40;
    int iterations = 5;

    std::vector<Point> data(dataSize);
    dataGenerate(data, dataSize, 3);
    clara(data, k, sampleSize, iterations);

    std::ofstream myfile;
    myfile.open("clara_output.csv");
    myfile << "x,y,c" << "\n";

    for (int i = 0; i < dataSize; ++i)
        myfile << data[i].x << "," << data[i].y << "," << data[i].cluster << "\n";

    myfile.close();
    return 0;
}
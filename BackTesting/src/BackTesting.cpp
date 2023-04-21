#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cmath>
#include <omp.h>

#include "nlohmann/json.hpp"
#include "curl/curl.h"
#include "timer/DS_definitions.h"
#include "timer/DS_timer.h"
// public
int numThreads = 1;
void get_numTreads(int argc, char *argv[]);
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
std::string send_http_request(const std::string &url);
char *conversionTimestamp(long timestamp);
void viewData(nlohmann::json&j);
void makeFile(nlohmann::json&j, std::unordered_map<long,int>&history, int n, std::string const title);
// serial
void serialAlgorithm(nlohmann::json&j, double*close);
std::vector<double> calculateRSI(double*close, int n, int period);
std::vector<double> calculateBollingerBands(double*close, int n, int period, int k);
double backtesting(nlohmann::json&j, std::unordered_map<long,int>&history, std::vector<double> rsi_values, int p1, int p2, int p3, double*close);
double* getClose(nlohmann::json&j);
//parallel
void parallelAlgorithm(nlohmann::json&j, double*close);
std::vector<double> calculateRSI_parallel(double*close, int n, int period);
std::vector<double> calculateBollingerBands_parallel(double*close, int n, int period, int k);
double backtesting_parallel(nlohmann::json&j, std::unordered_map<long,int>&history, std::vector<double> rsi_values, int p1, int p2, int p3, double*close);
double* getClose_parallel(nlohmann::json&j);

int main(int argc, char *argv[]){
    omp_set_nested(1);
    get_numTreads(argc, argv);

    double* close;
    std::string url = "https://api.bithumb.com/public/candlestick/btc_krw/24h";
    std::string response = send_http_request(url);
    nlohmann::json j = nlohmann::json::parse(response);
    DS_timer timer(4, 1);

    timer.setTimerName(0, (char*)"Serial Algorithm");
    timer.setTimerName(1, (char*)"Parallel Algorithm");

    timer.onTimer(0);
    close = getClose(j);
    serialAlgorithm(j, close);
    timer.offTimer(0);

    timer.onTimer(1);
    close = getClose_parallel(j);
    parallelAlgorithm(j, close);
    timer.offTimer(1);

    timer.printTimer();
    return 0;
}

void get_numTreads(int argc, char *argv[]){
    if (argc < 2) {
      printf("It requires one arguments\n");
      printf("Usage: Extuction_file numThreads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
}

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata){
    ((std::string *)userdata)->append(ptr, size * nmemb);
    return size * nmemb;
}

std::string send_http_request(const std::string &url){
    CURL *curl = curl_easy_init();
    std::string response;

    if (curl){
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode result = curl_easy_perform(curl);
        if (result != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(result) << std::endl;

        curl_easy_cleanup(curl);
    }

    return response;
}

char *conversionTimestamp(long timestamp){
    time_t t = (time_t)(timestamp / 1000);
    struct tm *timeinfo = localtime(&t);
    char *buffer = new char[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M", timeinfo);

    return buffer;
}

void viewData(nlohmann::json&j){
    for (auto &element : j["data"]){
        std::cout << "timestamp: " << conversionTimestamp(element[0]) << ", ";
        std::cout << "open: " << element[1] << ", ";
        std::cout << "high: " << element[2] << ", ";
        std::cout << "low: " << element[3] << ", ";
        std::cout << "close: " << element[4] << ", ";
        std::cout << "volume: " << element[5] << "\n";
    }
}

void makeFile(nlohmann::json&j, std::unordered_map<long,int>&history, int n, std::string const title){
    std::ofstream myfile;
    
    std::string subtitle = title;
    subtitle.append(std::to_string(n));
    subtitle.append(".csv");
    myfile.open(subtitle);
    myfile << "timestamp,open,high,low,close,action" << "\n";

    for (size_t i = 0; i < j["data"].size(); ++i) {
        myfile << j["data"][i][0] << "," << j["data"][i][1] << "," << j["data"][i][2] << "," << j["data"][i][3] << "," << j["data"][i][4] << "," << (history.count(j["data"][i][0]) ? history.at(j["data"][i][0]) : 0) << "\n";
    }
    myfile.close();
}

std::vector<double> calculateRSI(double*close, int n, int period){
    std::vector<double> RSI_values(n - period + 1, 0);
    for (int index = 0; index <= n - period; ++index) {
        double sumGain = 0;
        double sumLoss = 0;

        for (int i = index; i < index + period - 1; i++) {
            if (close[i + 1] > close[i])
                sumGain += close[i + 1] - close[i];
            else
                sumLoss += close[i] - close[i + 1];
        }

        double RS = (sumGain / period) / (sumLoss / period);
        RSI_values[index] = 100 - (100 / (1 + RS));
    }

    return RSI_values;
}

std::vector<double> calculateBollingerBands(double*close, int n, int period, int k) {
    std::vector<double> BB_position_values(n - period + 1, 0);
    for (int index = 0; index <= n - period; ++index) {
        double sum = 0;
        for (int i = index; i < index + period; i++)
            sum += close[i];
        double MA = sum / period;

        double SD = 0;
        for (int i = index; i < index + period; i++)
            SD += pow(close[i] - MA, 2);
        SD = sqrt(SD / period);

        double UB = MA + k * SD;
        double LB = MA - k * SD;

        BB_position_values[index] = (close[index + period - 1] - LB) / (UB - LB);
    }

    return BB_position_values;
}

double backtesting(nlohmann::json&j, std::unordered_map<long,int>&history, std::vector<double> rsi_values, int p1, int p2, int p3, double*close) {
    int maxParam = std::max(std::max(p1, p2), p3);
    
    double rateOfReturn = 0.0;
    double entryPrice = 0.0;
    
    std::vector<double> bb_values = calculateBollingerBands(close, j["data"].size(), p2, p3);

    for (size_t i = maxParam; i < j["data"].size(); i++) {
        
        double rsi = rsi_values[i - p1];
        double bb = bb_values[i - p2];

        bool buyCondition = rsi < 30 && bb < 0.0;
        bool sellCondition = rsi > 70 && bb > 1.0;

        double closePrice = close[i];
        long timestamp = j["data"][i][0];

        if (buyCondition && entryPrice == 0.0) {
            entryPrice = closePrice;
            history.insert({timestamp,1});
        } else if (sellCondition && entryPrice != 0.0) {
            rateOfReturn += 100.0 * closePrice / entryPrice - 100.0;
            entryPrice = 0.0;
            history.insert({timestamp,2});
        }
    }

    return rateOfReturn;

}

double* getClose(nlohmann::json&j){
    double* close = new double[j["data"].size()];

    for(size_t i = 0; i < j["data"].size(); ++i){
        std::stringstream ssDouble((std::string)j["data"][i][4]);
        ssDouble >> close[i];
    }

    return close;
}

void serialAlgorithm(nlohmann::json&j, double*close){
    double bestReturn = 0.0;
    int n = 1, curr = 1;

    for (int p1 = 10; p1 <= 60; p1++){
        std::vector<double> rsi = calculateRSI(close, j["data"].size(), p1);

        for (int p2 = 10; p2 <= 60; p2++)
            for (int p3 = 5; p3 <= 30; p3++){
                std::unordered_map<long,int> history;

                double rateOfReturn = backtesting(j, history, rsi, p1, p2, p3 / 10.0, close);

                std::cout << "process :" << curr++ << "/" << (51*51*26) << "\n";
                if (bestReturn < rateOfReturn) {
                    bestReturn = rateOfReturn;

                    // std::cout << rateOfReturn << "\n";
                    makeFile(j, history, n++, "./out/serialTest");
                }

                history.clear();
            }
    }
}

void parallelAlgorithm(nlohmann::json&j, double*close){
    double bestReturn = 0.0;
    int n = 1, curr = 1;

    #pragma omp parallel for num_threads(numThreads)
    for (int p1 = 10; p1 <= 60; p1++) {
        std::vector<double> rsi = calculateRSI_parallel(close, j["data"].size(), p1);

        #pragma omp parallel for collapse(2) num_threads(numThreads)
        for (int p2 = 10; p2 <= 60; p2++) {
            for (int p3 = 5; p3 <= 30; p3++) {
                std::unordered_map<long,int> history;
                double rateOfReturn = backtesting_parallel(j, history, rsi, p1, p2, p3 / 10.0, close);

                #pragma omp critical
                {   
                    std::cout << "process :" << curr++ << "/" << (51*51*26) << "\n";
                    if (bestReturn < rateOfReturn) {
                        bestReturn = rateOfReturn;

                        makeFile(j, history, n++, "./out/parallelTest");
                    }
                }
                history.clear();
            }
        }
    }
}

std::vector<double> calculateRSI_parallel(double*close, int n, int period){
    std::vector<double> RSI_values(n - period + 1, 0);

    #pragma omp parallel for num_threads(numThreads)
    for (int index = 0; index <= n - period; ++index) {
        double sumGain = 0;
        double sumLoss = 0;

        for (int i = index; i < index + period - 1; i++) {
            if (close[i + 1] > close[i])
                sumGain += close[i + 1] - close[i];
            else
                sumLoss += close[i] - close[i + 1];
        }

        double RS = (sumGain / period) / (sumLoss / period);
        RSI_values[index] = 100 - (100 / (1 + RS));
    }

    return RSI_values;
}

std::vector<double> calculateBollingerBands_parallel(double*close, int n, int period, int k) {
    std::vector<double> BB_position_values(n - period + 1, 0);

    #pragma omp parallel for num_threads(numThreads)
    for (int index = 0; index <= n - period; ++index) {
        double sum = 0;
        for (int i = index; i < index + period; i++)
            sum += close[i];
        double MA = sum / period;

        double SD = 0;
        for (int i = index; i < index + period; i++)
            SD += pow(close[i] - MA, 2);
        SD = sqrt(SD / period);

        double UB = MA + k * SD;
        double LB = MA - k * SD;

        BB_position_values[index] = (close[index + period - 1] - LB) / (UB - LB);
    }

    return BB_position_values;
}

double backtesting_parallel(nlohmann::json&j, std::unordered_map<long,int>&history, std::vector<double> rsi_values, int p1, int p2, int p3, double*close) {
    int maxParam = std::max(std::max(p1, p2), p3);
    double rateOfReturn = 0.0;
    double entryPrice = 0.0;
    std::vector<double> bb_values = calculateBollingerBands_parallel(close, j["data"].size(), p2, p3);

    for (size_t i = maxParam; i < j["data"].size(); i++) {
        double rsi = rsi_values[i - p1];
        double bb = bb_values[i - p2];

        bool buyCondition = rsi < 30 && bb < 0.0;
        bool sellCondition = rsi > 70 && bb > 1.0;

        double closePrice = close[i];
        long timestamp = j["data"][i][0];

        if (buyCondition && entryPrice == 0.0) {
            entryPrice = closePrice;
            history.insert({timestamp, 1});
        }
        else if (sellCondition && entryPrice != 0.0) {
            rateOfReturn += 100.0 * closePrice / entryPrice - 100.0;
            entryPrice = 0.0;
            history.insert({timestamp, 2});
        }
    }

    return rateOfReturn;
}

double* getClose_parallel(nlohmann::json&j){
    double* close = new double[j["data"].size()];

    #pragma omp parallel for
    for(size_t i = 0; i < j["data"].size(); ++i){
        std::stringstream ssDouble((std::string)j["data"][i][4]);
        ssDouble >> close[i];
    }

    return close;
}
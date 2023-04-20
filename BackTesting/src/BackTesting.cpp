#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cmath>

#include "nlohmann/json.hpp"
#include "curl/curl.h"
#include "timer/DS_definitions.h"
#include "timer/DS_timer.h"

void makeFile(nlohmann::json&j, std::unordered_map<long,int>&history, int n);
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
std::string send_http_request(const std::string &url);
char *conversionTimestamp(long timestamp);
void viewData(nlohmann::json&j);
std::vector<double> calculateRSI(double*close, int n, int period);
std::vector<double> calculateBollingerBands(double*close, int n, int period, int k);
double backtesting(nlohmann::json&j, std::unordered_map<long,int>&history, int p1, int p2, int p3);
double* getClose(nlohmann::json&j);

int main(int argc, char const *argv[]){
    std::string url = "https://api.bithumb.com/public/candlestick/btc_krw/24h";
    std::string response = send_http_request(url);
    nlohmann::json j = nlohmann::json::parse(response);
    DS_timer timer(4, 1);

    timer.setTimerName(0, (char*)"Serial Algorithm");
	timer.setTimerName(1, (char*)"Parallel Algorithm");

    viewData(j);

    double bestReturn = 0.0;
    int best1 = 1, best2 = 1, best3 = 1, n = 1;

    timer.onTimer(0);
    for (int p1 = 10; p1 <= 60; p1++)
        for (int p2 = 10; p2 <= 60; p2++)
            for (int p3 = 5; p3 <= 30; p3++){
                std::unordered_map<long,int> history;

                double rateOfReturn = backtesting(j, history, p1, p2, p3 / 10.0);

                if (bestReturn < rateOfReturn) {
                    bestReturn = rateOfReturn;

                    best1 = p1;
                    best2 = p2;
                    best3 = p3;

                    std::cout << rateOfReturn << "\n";
                    makeFile(j, history, n++);
                }

                history.clear();
            }
    timer.offTimer(0);

    timer.printTimer();
    return 0;
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

double backtesting(nlohmann::json&j, std::unordered_map<long,int>&history, int p1, int p2, int p3) {
    int maxParam = std::max(std::max(p1, p2), p3);
    
    double rateOfReturn = 0.0;
    double entryPrice = 0.0;

    double* close = getClose(j);
    
    std::vector<double> rsi_values = calculateRSI(close, j["data"].size(), p1);
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

    for(int i = 0; i < j["data"].size(); ++i){
        std::stringstream ssDouble((std::string)j["data"][i][4]);
        ssDouble >> close[i];
    }

    return close;
}

void makeFile(nlohmann::json&j, std::unordered_map<long,int>&history, int n){
    std::ofstream myfile;
    std::string title = "./out/bascTest";
    
    title.append(std::to_string(n));
    title.append(".csv");
    myfile.open(title);
    myfile << "timestamp,open,high,low,close,action" << "\n";

    for (int i = 0; i < j["data"].size(); ++i) {
        myfile << j["data"][i][0] << "," << j["data"][i][1] << "," << j["data"][i][2] << "," << j["data"][i][3] << "," << j["data"][i][4] << "," << (history.count(j["data"][i][0]) ? history.at(j["data"][i][0]) : 0) << "\n";
    }
    myfile.close();
}
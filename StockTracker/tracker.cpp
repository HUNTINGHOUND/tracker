#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <cstdlib>
#include "include/DataFeeder.hpp"
#include "include/Settings.hpp"
#include "include/JSONProcessor.hpp"
#include "include/RSITool.hpp"
#include "include/OHLC.hpp"
#include "include/HistoryAnalyzer.hpp"
#include "include/SnapshotAnalyzer.hpp"
#include "include/Oauth.hpp"

int main() {
    
    char* tz = "TZ=America/New_York";
    putenv(tz);

    std::unordered_map<std::string, RSITool> rsi;

    //http requests

    Settings setting;
    setting.ask();
    setting.makeFieldsAndHeaders();

    if (setting.service.compare("/snapshot") == 0) {

        std::unordered_map<std::string, std::vector<std::pair<double, long>>> stocks;

        DataFeeder feeder(STOCKTRACKER_HOST, setting);

        feeder.buildRequest(setting.service, setting.fields.back(), setting.headers);

        SnapshotAnalyzer analyzer;
        analyzer.analyze(stocks, rsi, feeder, setting.analyticalPattern, setting.upBound, setting.lowBound, setting.interval, setting.email);
    }
    else {

        std::unordered_map<std::string, std::vector<OHLC>> stocks;

        DataFeeder feeder(RAPIDAPI_HOST, setting);

        for (auto& field : setting.fields) {
            feeder.buildRequest(setting.service, field, setting.headers);

            std::string res = feeder.getData();
            JSONProcessor processor(res);

            processor.processYahoo(stocks);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        HistoryAnalyzer analyzer;
        analyzer.analyze(stocks, rsi, setting.upBound, setting.lowBound, setting.analyticalPattern);
    }


    return 0;
}



#include "../include/SnapshotAnalyzer.hpp"
#include "../include/Pattern.hpp"
#include "../include/JSONProcessor.hpp"
#include "../include/Email.hpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <iomanip>

void SnapshotAnalyzer::analyze(std::unordered_map<std::string, std::vector<std::pair<double, long>>>& stocks,
                               std::unordered_map<std::string, RSITool>& rsi, DataFeeder& feeder, int pattern, int upBound, int lowBound, int interval, std::string& email) {

    this->interval = interval;
    this->lowBound = lowBound;
    this->upBound = upBound;
    this->email = email;
    
    if (pattern == Pattern::BOUND) {
        bound(stocks, rsi, feeder);
    }
    else if (pattern == Pattern::RISING) {
        rising(stocks, rsi, feeder);
    }

}

void SnapshotAnalyzer::rising(std::unordered_map<std::string, std::vector<std::pair<double, long>>>& stocks, std::unordered_map<std::string, RSITool>& rsi,
                              DataFeeder& feeder) {

    std::unordered_map<std::string, std::pair<double, double>> peaks;
    std::unordered_map<std::string, std::pair<double, double>> sells;



    while (true) {

        try {

        std::string res = feeder.getData();
        std::cout << res << std::endl;

        JSONProcessor processor(res);

        processor.processStockTracker(stocks);


        for (auto it = stocks.begin(); it != stocks.end(); it++) {

            if (peaks.find(it->first) == peaks.end()) {
                peaks[it->first] = std::pair<double, double>(-1, -1);
                sells[it->first] = std::pair<double, double>(-1, -1);
            }

            if (it->second.size() == 15) {
                rsi[it->first].createPeriod14(it->second);
                std::cout << it->first + ":" + " ";
                std::cout << rsi[it->first].rsi.back() << std::endl;
                rsi[it->first].checkWithEmail(it->first, upBound, lowBound, interval);

                if (peaks[it->first].first < 0 && rsi[it->first].rsi.back() > upBound) {
                    peaks[it->first].first = it->second.back().first;
                    peaks[it->first].second = rsi[it->first].rsi.back();
                }
            }
            else if (it->second.size() > 15) {

                rsi[it->first].update(it->second.back().first);
                std::cout << it->first + ":" + " ";
                std::cout << rsi[it->first].rsi.back() << std::endl;
                rsi[it->first].checkWithEmail(it->first, upBound, lowBound, interval);

                if (peaks[it->first].first < 0 && rsi[it->first].rsi.back() > upBound) {
                    peaks[it->first].first = it->second.back().first;
                    peaks[it->first].second = rsi[it->first].rsi.back();
                }

                if (it->second.back().first >= peaks[it->first].first && peaks[it->first].first > 0) {
                    if (rsi[it->first].rsi.back() >= peaks[it->first].second) {
                        peaks[it->first].first = stocks[it->first].back().first;
                        peaks[it->first].second = rsi[it->first].rsi.back();
                    }
                    else {
                        if (sells[it->first].first == -1 || sells[it->first].first > it->second.back().first) {
                            sells[it->first] = std::pair<double, double>(it->second.back().first, rsi[it->first].rsi.back());
                        }
                        else {

                            std::string host = "smtp.office365.com";
                            std::string username = "STOCKALERT_NO_REPLY@outlook.com";
                            std::string password = "830633xhrwyxxz";
                            Email mail(host, 587, username, password);

                            std::string rec = email;
                            mail.setSender(username);
                            mail.setRecipient(rec);

                            peaks[it->first] = std::pair<double, double>(-1, -1);
                            sells[it->first] = std::pair<double, double>(-1, -1);

                            std::string subject = it->first + "SELL SIGNAL";
                            mail.setSubject(subject);

                            std::stringstream stream1;
                            stream1 << std::fixed << std::setprecision(2) << it->second.back().first;

                            std::time_t time = it->second.back().second;
                            std::stringstream stream2;
                            stream2 << std::put_time(std::localtime(&time), "%c %Z");

                            std::string body = it->first + "Sell Sigal." + "Current RSI: " +
                            std::to_string(rsi[it->first].rsi.back()) + ". Current price: " + stream1.str() +
                            ". Current time: " + stream2.str();
                            mail.setBody(body);

                            mail.send();
                        }
                    }
                }
            }
        }

        } catch(...) {
            std::cout << "snapshot error, skipping" << std::endl;
        }

        std::cout << "waiting\n";
        std::this_thread::sleep_for(std::chrono::seconds(interval));

    }
}


void SnapshotAnalyzer::bound(std::unordered_map<std::string, std::vector<std::pair<double, long>>>& stocks, std::unordered_map<std::string, RSITool>& rsi,
                             DataFeeder& feeder) {
    while (true) {

        try {

            std::string res = feeder.getData();
            std::cout << res << std::endl;
            
            if(res == "") {
                std::cerr << "Data failed, skipping" << std::endl;
                continue;
            }
            
            time_t epochStamp = std::time(nullptr);
            std::tm* time = std::localtime(&epochStamp);
            std::stringstream ss;
            ss << std::put_time(time, "%a, %d %b %Y %T %z");
            std::cout << ss.str() << std::endl;

            JSONProcessor processor(res);

            processor.processStockTracker(stocks);

            for (auto it = stocks.begin(); it != stocks.end(); it++) {
                if (it->second.size() == 15) {
                    rsi[it->first].createPeriod14(it->second);
                    std::cout << it->first + ":" + " ";
                    std::cout << rsi[it->first].rsi.back() << std::endl;
                    rsi[it->first].checkWithEmail(it->first, upBound, lowBound, interval);
                }
                else if (it->second.size() > 15) {
                    rsi[it->first].update(it->second.back().first);
                    std::cout << it->first + ":" + " ";
                    std::cout << rsi[it->first].rsi.back() << std::endl;
                    rsi[it->first].checkWithEmail(it->first, upBound, lowBound, interval);
                }
            }
        } catch(...) {
            std::cout << "Snapshot exception, skipping" << std::endl;
        }



        std::cout << "waiting\n";
        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
}


#include "../include/HistoryAnalyzer.hpp"
#include <sstream>
#include <iomanip>

void HistoryAnalyzer::analyze(std::unordered_map<std::string, std::vector<OHLC>>& prices, std::unordered_map<std::string, RSITool>& rsis, int upperBound, int lowerBound, int pattern) {
    for (const auto& x : prices) {
        CSVFile file(x.first);
        file.write(std::string("Time,Open,High,Low,Close,RSI"));
        rsis[x.first].createPeriod14(prices[x.first]);

        if (pattern == Pattern::RISING) {
            rising(x.first, x.second, rsis, upperBound, lowerBound, file);
        }
        else if (pattern == Pattern::BOUND) {
            bound(x.first, x.second, rsis, upperBound, lowerBound, file);
        }
        
        file.writeFile();
    }
}

void HistoryAnalyzer::bound(const std::string& name, const std::vector<OHLC>& x, std::unordered_map<std::string, RSITool>& rsis, int upperBound, int lowerBound, CSVFile& file) {
    for (size_t i = 0; i < x.size(); i++) {
        const OHLC& price = x[i];

        if (i >= 14) {
            rsis[name].update(price.close);
        }


        file.addRow();
        std::stringstream stream;

        std::time_t time = price.time;
        stream << std::put_time(std::localtime(&time), "%c %Z");
        file.write(stream.str());
        stream.str(std::string());

        stream << std::fixed << std::setprecision(2) << price.open;
        file.write(stream.str());
        stream.str(std::string());

        stream << std::fixed << std::setprecision(2) << price.high;
        file.write(stream.str());
        stream.str(std::string());

        stream << std::fixed << std::setprecision(2) << price.low;
        file.write(stream.str());
        stream.str(std::string());

        stream << std::fixed << std::setprecision(2) << price.close;
        file.write(stream.str());
        stream.str(std::string());

        stream << std::fixed << std::setprecision(2) << rsis[name].rsi.back();
        file.write(stream.str());
        stream.str(std::string());

        file.write(rsis[name].checkWithoutEmail(upperBound, lowerBound));

    }
}

void HistoryAnalyzer::rising(const std::string& name, const std::vector<OHLC>& x, std::unordered_map<std::string, RSITool>& rsis, int upperBound, int lowerBound, CSVFile& file) {
    double peakPrice = -1;
    int peakRSI = -1;

    for (size_t i = 0; i < x.size(); i++) {
        const OHLC& price = x[i];

        if (i >= 14) {
            rsis[name].update(price.close);
        }

        bool sellsignal = false;
        bool peak = false;

        if (peakPrice < 0 && rsis[name].rsi.back() > upperBound) {
            peakPrice = price.close;
            peakRSI = rsis[name].rsi.back();
        }
        else if (price.close >= peakPrice && peakPrice > 0) {
            if (rsis[name].rsi.back() >= peakRSI) {
                peakPrice = price.close;
                peakRSI = rsis[name].rsi.back();
                peak = true;
            }
            else {
                sellsignal = true;
            }
        }

        file.addRow();
        std::stringstream stream;

        std::time_t time = price.time;
        stream << std::put_time(std::localtime(&time), "%c %Z");
        file.write(stream.str());
        stream.str(std::string());

        stream << std::fixed << std::setprecision(2) << price.open;
        file.write(stream.str());
        stream.str(std::string());

        stream << std::fixed << std::setprecision(2) << price.high;
        file.write(stream.str());
        stream.str(std::string());

        stream << std::fixed << std::setprecision(2) << price.low;
        file.write(stream.str());
        stream.str(std::string());

        stream << std::fixed << std::setprecision(2) << price.close;
        file.write(stream.str());
        stream.str(std::string());

        stream << std::fixed << std::setprecision(2) << rsis[name].rsi.back();
        file.write(stream.str());
        stream.str(std::string());

        if (sellsignal) {
            file.write(std::string("SELL SIGNAL"));
        }

        if (peak) {
            file.write(std::string("PEAK"));
        }

    }
}

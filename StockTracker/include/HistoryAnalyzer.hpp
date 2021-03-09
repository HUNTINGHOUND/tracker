#ifndef HistoryAnalyzer_hpp
#define HistoryAnalyzer_hpp

#include <unordered_map>
#include "OHLC.hpp"
#include "RSITool.hpp"
#include "CSVFile.hpp"
#include "Pattern.hpp"

class HistoryAnalyzer {

    void rising(const std::string&, const std::vector<OHLC>&, std::unordered_map<std::string, RSITool>&, int, int, CSVFile&);
    void bound(const std::string&, const std::vector<OHLC>&, std::unordered_map<std::string, RSITool>&, int, int, CSVFile&);

public:
    void analyze(std::unordered_map<std::string, std::vector<OHLC>>&, std::unordered_map<std::string, RSITool>&, int, int, int);
};

#endif /* HistoryAnalyzer_hpp */

#ifndef JSONProcessor_hpp
#define JSONProcessor_hpp

#include <rapidjson/document.h>
#include "OHLC.hpp"
#include <unordered_map>
#include <vector>

class JSONProcessor {
    rapidjson::Document body;
public:

    JSONProcessor(std::string&);
    void processStockTracker(std::unordered_map<std::string, std::vector<std::pair<double, long>>>&);
    void processYahoo(std::unordered_map<std::string, std::vector<OHLC>>&);
};


#endif /* JSONProcessor_hpp */

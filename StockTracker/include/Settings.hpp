#ifndef Settings_hpp
#define Settings_hpp

#include "Pattern.hpp"
#include <string>
#include <vector>
#include <unordered_map>

#define STOCKTRACKER_HOST "http://api.stockstracker.com"
#define RAPIDAPI_HOST "https://apidojo-yahoo-finance-v1.p.rapidapi.com"

class Settings {
public:
    
    std::string stockstracker_apikey;
    std::string yahoo_finance_apikey;
    
    std::string email;

    std::string service;
    std::string syms;
    std::vector<std::vector<std::pair<std::string, std::string>>> fields;
    std::unordered_map<std::string, std::string> headers;

    int upBound;
    int lowBound;

    std::string resolution;
    bool useRange;
    std::string range;

    time_t period1;
    time_t period2;
    
    int interval;

    int analyticalPattern;
    
    void ask();

    void makeFieldsAndHeaders();

};

#endif /* Settings_hpp */

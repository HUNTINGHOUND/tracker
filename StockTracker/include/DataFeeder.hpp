#ifndef DataFeeder_hpp
#define DataFeeder_hpp

#include "Settings.hpp"
#include <curl/curl.h>
#include <string>
#include <unordered_map>
#include <vector>

#define STOCKTRACKER_HOST "http://api.stockstracker.com"

class DataFeeder {
    CURL* curl = curl_easy_init();
    std::string host;
    std::string response_string;
    std::string request;
    curl_slist* headers = nullptr;

    bool hIsEmpty;;

public:
    DataFeeder(const char* host, Settings&);
    ~DataFeeder();

    void buildRequest(std::string& type, std::vector<std::pair<std::string, std::string>>& fields, std::unordered_map<std::string, std::string>& headers);

    std::string getData();
};


#endif /* DataFeeder_hpp */

#include "../include/DataFeeder.hpp"
#include "../include/Settings.hpp"
#include <curl/easy.h>

size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}


DataFeeder::DataFeeder(const char* host, Settings& s) {
    this->host = host;
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, s.service.compare("/snapshot") == 0 ? 5L : 60L);
}

DataFeeder::~DataFeeder() {
    curl_easy_cleanup(curl);
    curl_slist_free_all(this->headers);
}

void DataFeeder::buildRequest(std::string& type,
    std::vector<std::pair<std::string, std::string>>& fields, std::unordered_map<std::string, std::string>& headers) {
    request = "";
    request += host + type + "?";

    std::string query;
    for (auto const& x : fields) {
        query += x.first + "=" + x.second + "&";
    }
    query.pop_back();

    request += query;

    hIsEmpty = headers.empty();
    curl_slist_free_all(this->headers);
    this->headers = nullptr;
    for (auto const& x : headers) {
        std::string head = x.first + ": " + x.second;
        this->headers = curl_slist_append(this->headers, head.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, request.c_str());
}

std::string DataFeeder::getData() {
    response_string = "";

    if (!hIsEmpty) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    curl_easy_perform(curl);


    return response_string;
}


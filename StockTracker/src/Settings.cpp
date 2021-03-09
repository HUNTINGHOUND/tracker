#include "../include/Settings.hpp"
#include <iostream>
#include <chrono>
#include <sstream>
#include <locale>
#include <iomanip>
#include <chrono>
#include <fstream>


time_t toEpoch(std::string& date) {
    std::tm tm = {};
    std::stringstream ss(date);
    ss >> std::get_time(&tm, "%m/%d/%Y/%H:%M:%S");
    tm.tm_isdst = -1;
    auto t = mktime(&tm);
    return t;
}

bool searchVector(std::string& s, std::vector<std::string>& arr) {
    for (const std::string& x : arr) {
        if (x.compare(s)) {
            return true;
        }
    }
    return false;
}


void Settings::ask() {
    bool invalid = true;
    while (invalid) {
        std::cout << "Choose function: use numbers beside each function" << std::endl;
        std::cout << "1. realtime" << std::endl;
        std::cout << "2. history" << std::endl;
        int function;
        std::cin >> function;

        switch (function) {
        case 1:
            service = "/snapshot";
            invalid = false;
            break;
        case 2:
            service = "/stock/v2/get-chart";
            invalid = false;
            break;
        default:
            std::cout << "Invalid input, try again" << std::endl;
        }
    }

    std::cout << "Input stock symbols seperated with space:";

    std::string input;
    std::cin.ignore();
    getline(std::cin, input);

    size_t pos = 0;
    std::string token;
    while ((pos = input.find(" ")) != std::string::npos) {
        token = input.substr(0, pos);
        syms += token + ",";
        input.erase(0, pos + 1);
    }
    syms += input;
    std::cout << std::endl;
    
    if (service.compare("/snapshot") == 0) {
        std::cout << "Input interval in seconds (must be larger than or equal to 10):";
        std::cin >> this->interval;
        std::cout << std::endl;
        
        std::cout << "Input email you want to recieve alert:";
        std::cin >> this->email;
        std::cout << std::endl;
        
        std::cout << "Input your stockstracker api key (you can get one for free on stockstracker.com or input 1 to use one stored on system):";
        std::cin >> this->stockstracker_apikey;
        std::cout << std::endl;
        
        if(this->stockstracker_apikey.compare("1") == 0) {
            std::ifstream file;
            file.open("stocks_key.txt");
            
            if(!file.is_open()) {
                std::cerr << "No previous key found, terminating";
                exit(1);
            }
            
            file >> this->stockstracker_apikey;
            file.close();
        } else {
            std::ofstream file;
            file.open("stocks_key.txt", std::ios::trunc);
            
            file << this->stockstracker_apikey;
            file.close();
        }
    }

    std::cout << "Set upper rsi bound (must be an integer):";
    std::cin >> upBound;
    std::cout << std::endl;

    std::cout << "Set lower rsi bound (must be an integer):";
    std::cin >> lowBound;
    std::cout << std::endl;

    std::cout << "Input analytical pattern: use number beside each function" << std::endl;
    std::cout << "1.Bound" << std::endl;
    std::cout << "2.Rising" << std::endl;
    std::cin >> analyticalPattern;

    if (service.compare("/stock/v2/get-chart") == 0) {
        
        std::cout << "Input your yahoo finance api key (you can get one for free on https://rapidapi.com/apidojo/api/yahoo-finance1 or input 1 to use one stored on system):";
        std::cin >> this->yahoo_finance_apikey;
        std::cout << std::endl;
        
        if(this->yahoo_finance_apikey.compare("1") == 0) {
            std::ifstream file;
            file.open("yahoo_key.txt");
            
            if(!file.is_open()) {
                std::cerr << "No previous key found, terminating";
                exit(1);
            }
            
            file >> this->yahoo_finance_apikey;
            file.close();
        } else {
            std::ofstream file;
            file.open("yahoo_key.txt", std::ios::trunc);
            
            file << this->yahoo_finance_apikey;
            file.close();
        }
        
        invalid = true;
        while (invalid) {
            std::vector<std::string> arr = { "1m", "2m", "5m", "15m", "60m", "1d" };
            std::cout << "Choose unit for resolution (1m|2m|5m|15m|60m|1d):";
            std::string reso;
            std::cin >> reso;
            std::cout << std::endl;
            if (!searchVector(reso, arr)) {
                std::cout << "Invalid input, try again." << std::endl;
            }
            else {
                resolution = reso;
                invalid = false;
            }
        }

        char isRange;
    inputErr:
        std::cout << "Do you want to specify range?  [y/n]: ";
        std::cin >> isRange;
        std::cout << std::endl;
        if (isRange != 'y' && isRange != 'n') {
            std::cout << "Invalid input, try again." << std::endl;
            goto inputErr;
        }

        useRange = isRange == 'y' ? true : false;
        std::cout << std::endl;

        if (useRange) {
            std::cout << "==============================IMPORTANT==========================================" << std::endl;
            std::cout << "Please follow the given way of inputing dates. The result will" << std::endl
                << "be calculated from the start of starting date and end of the " << std::endl
                << "ending day. Also note that data from certain resolution may no longer" << std::endl
                << "be available. For example, 1 minute data is only available for the last" << std::endl
                << "7 day. All times will also be in New York timezone." << std::endl
                << "Enter date in the follow order: mm/dd/yyyy" << std::endl;
            std::cout << "=================================================================================" << std::endl;

            std::cout << "Enter starting date: ";
            std::string starting_date;
            std::cin >> starting_date;
            std::cout << std::endl;
            std::cout << "Enter ending date: ";
            std::string ending_date;
            std::cin >> ending_date;
            std::cout << std::endl;

            starting_date += "/00:00:00";
            ending_date += "/23:59:59";

            period1 = toEpoch(starting_date);
            period2 = toEpoch(ending_date);
        }
        else {
            invalid = true;
            while (invalid) {
                std::cout << "Input range (1d|5d|1mo|3mo|6mo|1y|2y|5y|10y): ";
                std::vector<std::string> arr = { "1d" , "5d" , "1mo" , "3mo" , "6mo" , "1y" , "2y" , "5y" , "10y" };
                std::string in;
                std::cin >> in;
                std::cout << std::endl;
                if (!searchVector(in, arr)) {
                    std::cout << "Invalid input, try again." << std::endl;
                }
                else {
                    invalid = false;
                    this->range = in;
                }
            }
        }
    }
}

void Settings::makeFieldsAndHeaders() {
    if (service.compare("/snapshot") == 0) {
        std::vector<std::pair<std::string, std::string>> request;
        request.push_back(std::pair<std::string, std::string>("apiKey", stockstracker_apikey));
        request.push_back(std::pair<std::string, std::string>("symbols", syms));
        request.push_back(std::pair<std::string, std::string>("fields", "last,lastTime"));
        fields.push_back(request);
    }
    else {
        headers["x-rapidapi-key"] = yahoo_finance_apikey;
        headers["x-rapidapi-host"] = RAPIDAPI_HOST;
        size_t r;
        std::string part;
        std::string search = syms;
        while ((r = search.find(',')) != std::string::npos) {
            part = search.substr(0, r);
            std::vector<std::pair<std::string, std::string>> request;
            request.push_back(std::pair<std::string, std::string>("interval", resolution));
            request.push_back(std::pair<std::string, std::string>("symbol", part));

            if (useRange) {
                request.push_back(std::pair<std::string, std::string>("range", "%20"));
            }
            else {
                request.push_back(std::pair<std::string, std::string>("range", range));
            }

            request.push_back(std::pair<std::string, std::string>("region", "US"));

            if (useRange) {
                request.push_back(std::pair<std::string, std::string>("period2", std::to_string(period2)));
                request.push_back(std::pair<std::string, std::string>("period1", std::to_string(period1)));
            }
            search.erase(0, r + 1);
            fields.push_back(request);
        }
        std::vector<std::pair<std::string, std::string>> request;
        request.push_back(std::pair<std::string, std::string>("interval", resolution));
        request.push_back(std::pair<std::string, std::string>("symbol", search));

        useRange ? request.push_back(std::pair<std::string, std::string>("range", "%20")) :
            request.push_back(std::pair<std::string, std::string>("range", range));

        request.push_back(std::pair<std::string, std::string>("region", "US"));

        if (useRange) {
            request.push_back(std::pair<std::string, std::string>("period2", std::to_string(period2)));
            request.push_back(std::pair<std::string, std::string>("period1", std::to_string(period1)));
        }

        fields.push_back(request);
    }
}

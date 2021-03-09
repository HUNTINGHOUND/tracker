#include "../include/JSONProcessor.hpp"
#include "../include/OHLC.hpp"
#include <iostream>
#include <chrono>


JSONProcessor::JSONProcessor(std::string& content) {
    body.Parse(content.c_str());
}

void JSONProcessor::processStockTracker(std::unordered_map<std::string, std::vector<std::pair<double,long>>>& stocks) {
    const rapidjson::Value& data = body["data"];
    assert(data.IsArray());
    for (size_t i = 0; i < data.Size(); i++) {
        const rapidjson::Value& s = data[i];

        std::string name = s["symbol"].GetString();
        std::string last = s["last"].GetString();

        std::cout << ::atof(last.c_str());
        std::cout << std::endl;

        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );


        std::pair<double, long> p(::atof(last.c_str()), ms.count());
        stocks[name].push_back(p);
    }
}

void JSONProcessor::processYahoo(std::unordered_map<std::string, std::vector<OHLC>>& stocks) {
    const rapidjson::Value& chart = body["chart"];
    assert(chart.IsObject());
    const rapidjson::Value& result = chart["result"];
    assert(result.IsArray());

    std::string sym = result[0]["meta"]["symbol"].GetString();
    const rapidjson::Value& timestamp = result[0]["timestamp"];
    const rapidjson::Value& openp = result[0]["indicators"]["quote"][0]["open"];
    const rapidjson::Value& highp = result[0]["indicators"]["quote"][0]["high"];
    const rapidjson::Value& lowp = result[0]["indicators"]["quote"][0]["low"];
    const rapidjson::Value& closep = result[0]["indicators"]["quote"][0]["close"];


    assert(timestamp.IsArray());
    assert(openp.IsArray());
    assert(highp.IsArray());
    assert(lowp.IsArray());
    assert(closep.IsArray());

    std::vector<OHLC> prices;

    for (size_t i = 0; i < timestamp.Size(); i++) {
        OHLC p;
        if (closep[i].IsNull() || highp[i].IsNull() || lowp[i].IsNull() || openp[i].IsNull())
            continue;
        p.close = closep[i].GetDouble();
        p.high = highp[i].GetDouble();
        p.low = lowp[i].GetDouble();
        p.open = openp[i].GetDouble();
        p.time = timestamp[i].GetInt();
        prices.push_back(p);
    }

    stocks[sym] = prices;
}

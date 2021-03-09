#include "../include/RSITool.hpp"
#include "../include/Email.hpp"
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <pthread.h>
#include <iostream>


void RSITool::createPeriod14(const std::vector<OHLC>& prices) {
    std::vector<double> deltas;

    double prev = prices[0].close;
    for (int i = 1; i < prices.size(); i++) {
        deltas.push_back(prices[i].close - prev);
        prev = prices[i].close;
    }

    double up = 0;
    double down = 0;
    for (double delta : deltas) {
        if (delta >= 0) {
            up += delta;
        }
        else {
            down += -delta;
        }
    }
    up /= 14.;
    down /= 14.;

    this->prevPrice = prices.back().close;
    this->up = up;
    this->down = down;

    double rs;
    if (this->down == 0 && this->up == 0) {
        rs = 1;
    }
    else if (this->down == 0) {
        rs = INT_MAX;
    }
    else {
        rs = this->up / this->down;
    }

    double initial = 100. - 100. / (1. + (rs));
    this->rsi = std::vector<double>(14, initial);
}

void RSITool::createPeriod14(const std::vector<std::pair<double, long>>& prices) {
    std::vector<double> deltas;

    double prev = prices[0].first;
    for (int i = 1; i < prices.size(); i++) {
        deltas.push_back(prices[i].first - prev);
        prev = prices[i].first;
    }

    double up = 0;
    double down = 0;
    for (double delta : deltas) {
        if (delta >= 0) {
            up += delta;
        }
        else {
            down += -delta;
        }
    }
    up /= 14.;
    down /= 14.;

    this->prevPrice = prices.back().first;
    this->up = up;
    this->down = down;

    double rs;
    if (this->down == 0 && this->up == 0) {
        rs = 1;
    }
    else if (this->down == 0) {
        rs = INT_MAX;
    }
    else {
        rs = this->up / this->down;
    }

    double initial = 100. - 100. / (1. + (rs));
    this->rsi = std::vector<double>(14, initial);
}

void RSITool::update(double currPrice) {
    double delta = currPrice - this->prevPrice;
    this->prevPrice = currPrice;

    double upval;
    double downval;

    if (delta > 0) {
        upval = delta;
        downval = 0.;
    }
    else {
        upval = 0.;
        downval = -delta;
    }

    this->up = (this->up * (14 - 1) + upval) / 14;
    this->down = (this->down * (14 - 1) + downval) / 14;

    double rs;
    if (this->down == 0 && this->up == 0) {
        rs = 1;
    }
    else if (this->down == 0) {
        rs = INT_MAX;
    }
    else {
        rs = this->up / this->down;
    }

    this->rsi.push_back(100. - 100. / (1 + (rs)));
}

struct email_message {
    Email* mail;
};


void RSITool::checkWithEmail(const std::string& sym, int upBound, int lowBound, int interval) {

    std::string host = "smtp.office365.com";
    std::string username = "STOCKALERT_NO_REPLY@outlook.com";
    std::string password = "830633xhrwyxxz";
    Email mail(host, 587, username, password);
    
    std::string rec = "morganxu2004@outlook.com";
    mail.setSender(username);
    mail.setRecipient(rec);


    if (this->rsi.back() > (upBound) && !this->over) {
        std::string subject = sym + " RSI Alert";

        mail.setSubject(subject);

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << this->prevPrice;

        const auto p1 = std::chrono::system_clock::now();

        auto t = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

        std::time_t time = t;
        std::stringstream stream2;
        stream2 << std::put_time(std::localtime(&time), "%c %Z");

        std::string body = sym + "." + "Interval: " + std::to_string(interval) + "RSI:" + std::to_string(this->rsi.back()) + " " + ss.str() + ". " + stream2.str();

        mail.setBody(body);
        
        
        mail.send();

        this->over = true;
        
    }
    if (this->rsi.back() < upBound && this->over) {
        //sendMail(sym, false, true, this->prevPrice, upBound, lowBound);
        this->over = false;
    }
    if (this->rsi.back() < (lowBound) && !this->under) {
        std::string subject = sym + " RSI Alert";

        mail.setSubject(subject);

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << this->prevPrice;

        const auto p1 = std::chrono::system_clock::now();

        auto t = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

        std::time_t time = t;
        std::stringstream stream2;
        stream2 << std::put_time(std::localtime(&time), "%c %Z");

        std::string body = sym + ". RSI:" + std::to_string(this->rsi.back()) + " " + ss.str() + ". " + stream2.str();

        mail.setBody(body);
        
        
        mail.send();
        
        
        this->under = true;
        
    }
    if (this->rsi.back() > lowBound && this->under) {
        //sendMail(sym, true, false, this->prevPrice, upBound, lowBound);
        this->under = false;
    }
}

std::string RSITool::checkWithoutEmail(int upBound, int lowBound) {
    if (this->rsi.back() > upBound && !this->over) {
        this->over = true;
        return "RSI gone over " + std::to_string(upBound);
    }
    if (this->rsi.back() < upBound && this->over) {
        this->over = false;
        return "RSI gone under " + std::to_string(upBound);
    }
    if (this->rsi.back() < lowBound && !this->under) {
        this->under = true;
        return "RSI gone under " + std::to_string(lowBound);
    }
    if (this->rsi.back() > lowBound && this->under) {
        this->under = false;
        return "RSI gone over " + std::to_string(lowBound);
    }

    return "";
}

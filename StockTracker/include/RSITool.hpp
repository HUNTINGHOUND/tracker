#ifndef RSITool_hpp
#define RSITool_hpp

#include <vector>
#include <string>
#include "OHLC.hpp"

class RSITool {
    double prevPrice;
    double up;
    double down;
    bool under;
    bool over;

public:

    std::vector<double> rsi;

    RSITool() {
        prevPrice = 0;
        up = 0;
        down = 0;
        under = false;
        over = false;
    }

    void createPeriod14(const std::vector<std::pair<double, long>>&);
    void createPeriod14(const std::vector<OHLC>&);
    void update(double);
    void checkWithEmail(const std::string&, int, int, int);
    std::string checkWithoutEmail(int, int);
};

#endif /* RSITool_hpp */

#ifndef SnapshotAnalyzer_hpp
#define SnapshotAnalyzer_hpp

#include "DataFeeder.hpp"
#include "RSITool.hpp"
#include <unordered_map>


class SnapshotAnalyzer {
    
    int upBound;
    int lowBound;
    int interval;
    std::string email;

    void rising(std::unordered_map<std::string, std::vector<std::pair<double, long>>>&, std::unordered_map<std::string, RSITool>&, DataFeeder&);
    void bound(std::unordered_map<std::string, std::vector<std::pair<double, long>>>&, std::unordered_map<std::string, RSITool>&, DataFeeder&);

public:
    void analyze(std::unordered_map<std::string, std::vector<std::pair<double, long>>>&, std::unordered_map<std::string, RSITool>&, DataFeeder&, int, int, int, int, std::string& email);
};

#endif /* SnapshotAnalyzer_hpp */

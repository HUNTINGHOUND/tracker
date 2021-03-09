#ifndef CSVrow_hpp
#define CSVrow_hpp

#include <string>

class CSVrow {
    std::string row;

public:
    void append(std::string);
    std::string toString();
};


#endif /* CSVrow_hpp */

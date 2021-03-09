#ifndef CSVFile_hpp
#define CSVFile_hpp

#include <vector>
#include "CSVrow.hpp"

class CSVFile {
    std::string name;
    std::vector<CSVrow> file;

    int capacity;
    int currRow;

public:
    CSVFile(const std::string&);
    void addRow(); //currRow should automatically be on the newly added row
    bool switchRow(int);
    int currentRow();
    void writeFile();
    void write(const std::string&);
};

#endif /* CSVFile_hpp */

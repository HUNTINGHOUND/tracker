#include "../include/CSVFile.hpp"
#include <fstream>

CSVFile::CSVFile(const std::string& name) {
    this->name = name;
    capacity = 1;
    currRow = 1;
    file.push_back(CSVrow());
}

void CSVFile::addRow() {
    capacity += 1;
    currRow = capacity;
    file.push_back(CSVrow());
}

bool CSVFile::switchRow(int r) {
    if (r > capacity) {
        return false;
    }
    currRow = r;
    return true;
}

int CSVFile::currentRow() {
    return currRow;
}

void CSVFile::writeFile() {
    std::ofstream csvfile(this->name + ".csv");
    if (csvfile.is_open()) {
        for (CSVrow& row : this->file) {
            csvfile << row.toString() << "\n";
        }
        csvfile.close();
    }
    else {
        throw std::runtime_error("File cannot be opened.");
    }
    csvfile.close();
}

void CSVFile::write(const std::string& s) {
    this->file[currRow - 1].append(s);
}


#include "../include/CSVrow.hpp"

void CSVrow::append(std::string content) {
    if (row.empty()) {
        row += content;
    }
    else {
        row += "," + content;
    }
}

std::string CSVrow::toString() {
    return this->row;
}

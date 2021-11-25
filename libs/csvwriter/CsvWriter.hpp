#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class CsvWriter {
    unsigned int col;
    std::vector<std::string> headers;
    static const std::string filePath;
public:
    CsvWriter(std::vector<int> threads);
    template <typename T>
    void addRow(const std::vector<T>& data) {
        std::ofstream file(filePath, std::ios_base::app);
        if (file.is_open()) {
            for (int i = 0; i < data.size() - 1; i++)
                file << data[i] << ",";
            file << data.back() << std::endl;
            file.close();
        } else std::cout << "Cannot open file" << std::endl;
    }
};

#endif

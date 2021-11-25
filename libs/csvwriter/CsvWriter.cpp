#include <csvwriter/CsvWriter.hpp>

using namespace std;

const string CsvWriter::filePath = "Results/Report.csv";

CsvWriter::CsvWriter(vector<int> threads) {
    col = threads.size() * 2 + 2;
    headers.push_back("Input size");
    headers.push_back("Serial");
    for (int thread: threads) {
        headers.push_back(to_string(thread) + "-Naive");
        headers.push_back(to_string(thread) + "-OMP");
    }
    ofstream file(filePath);
    file.close();
    addRow<string>(headers);

}

#include <iostream>
#include <vector>
#include <soe.hpp>
#include <tester.hpp>
#include <csvwriter/CsvWriter.hpp>

int main() {
    std::vector<int> threads {2, 4, 6, 8, 10, 12, 14, 16};
    CsvWriter writer(threads);
    std::vector<size_t> rowData;

    for (size_t n = 10e3; n <= 10e7; n *= 10) {
        std::cout << std::endl << "Finding primes under: " << n << std::endl;
        std::cout << "================================" << std::endl;
        rowData.push_back(n);
        
        // Serial sieve
        std::vector<bool> A(n);
        tester::ExecutionData data = tester::measure_exec(soe::serial, A);
        tester::print_result("Serial", data);
        rowData.push_back(data.elapsed_time);
        
        // Parallel sieves
        for (int i: threads) {
            soe::set_max_threads(i);
            
            // Custom thread-based implementation
            std::fill(A.begin(), A.end(), false);
            data = tester::measure_exec(soe::parallel_thread, A);
            tester::print_result(std::to_string(i) + "-parallel THREAD", data);
            rowData.push_back(data.elapsed_time);
            
            // OMP version
            std::fill(A.begin(), A.end(), false);
            data = tester::measure_exec(soe::parallel_omp, A);
            tester::print_result(std::to_string(i) + "-parallel OMP", data);
            rowData.push_back(data.elapsed_time);
        }
        writer.addRow<size_t>(rowData);
        rowData.resize(0);
    }
    return 0;
}

#ifndef TESTER_H
#define TESTER_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>

namespace tester {
    struct ExecutionData {
        int64_t elapsed_time;
        uint64_t return_val;
    };

    ExecutionData measure_exec(size_t (*f)(std::vector<bool>&), std::vector<bool>& A) {
        auto start = std::chrono::high_resolution_clock::now();
        size_t val = f(A);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        return ExecutionData { duration.count(), val };
    }

    void print_result(std::string tag, ExecutionData& result) {
        std::cout << std::left << std::setw(21)
            << "[" + tag + "]"
            << "Found " << result.return_val << " primes in "
            << result.elapsed_time << " microseconds"
            << std::endl;
    }
}


#endif

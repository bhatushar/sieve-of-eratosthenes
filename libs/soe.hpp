/*
Parameters
    Assuming SIMD CREW shared memory architecture
    N processors numbered 0 to N-1
    A is a boolean array initialzed with false
    |A| = n, indexed 0 to n-1
    N << sqrt(n) => processor 0 gets all numbers upto sqrt(n)

Algorithm for parallel sieve of Eratosthenes
    for p = 0 to N-1 do in parallel:
        lb[p]: lower bound of processor p on A
        ub[p]: upper bound of processor p on A
        Each processor gets at least n/N numbers, ranging from pn/N to (p+1)n/N-1
        lb[0] = 2, ub[N-1] = n-1
    for i = 2 to sqrt(n):
        if A[i] == true:
            continue
        // i is prime, mark all multiples of i in parallel
        for p = 0 to N-1 do in parallel:
            // Find the first multiple of i greater than or equal to i*i, say k
            k = max(i*i, lb[p])
            r = k mod i
            if r != 0:
                k = k + i - r
            for j = k to ub[p] in steps of i:
                A[j] = true
    Let prime_count = 0 be a variable with atomic increment
    for p = 0 to N-1 do in parallel:
        for i = lb[p] to ub[p]:
            if A[i] == false:
                prime_count++
    return prime_count

Analysis
    The first loop runs in parallel and performs fixed number of arithmetic operations => O(1)

*/

#ifndef SOE_H
#define SOE_H

#include <vector>
#include <thread>
#include <atomic>
#include <omp.h>
#include <math.h>

namespace soe {
    namespace {
        uint8_t MAX_THREADS = 1;

        inline void set_bounds(size_t n, size_t& lower_bound, size_t& upper_bound, uint8_t tid) {
            lower_bound = tid == 0 ? 2 : tid * (n / MAX_THREADS);
            upper_bound = tid == (MAX_THREADS - 1) ? n : (tid + 1) * (n / MAX_THREADS);
        }

        void mark_multiples(std::vector<bool>& A, size_t prime, uint8_t tid) {
            size_t lower_bound, upper_bound;
            set_bounds(A.size(), lower_bound, upper_bound, tid);
            lower_bound = std::max(prime * prime, lower_bound);
            size_t remainder = lower_bound % prime;
            if (remainder) lower_bound += prime - remainder;
            for (size_t j = lower_bound; j < upper_bound; j += prime)
                A[j] = true;
        }

        void count_primes(const std::vector<bool>& A, std::atomic<size_t>& prime_count, uint8_t tid) {
        size_t lower_bound, upper_bound;
        set_bounds(A.size(), lower_bound, upper_bound, tid);
        size_t count = 0;
        for (size_t i = lower_bound; i < upper_bound; i++)
            if (A[i] == false) count++;
        prime_count += count;
    }
    }

    void set_max_threads(uint8_t t) {
        MAX_THREADS = t;
        omp_set_num_threads(t);
    }

    /**
     * Run time analysis:
     * Computing square root of n takes lg(n) time.
     * For every new prime i, its multiples are marked in O(n/i) steps.
     * Finding all primes take n/2 + n/3 + n/5 + ... = O(nlg(lg(n))) time. [Divergence of the sum of the reciprocals of the primes]
     * Counting all the primes take linear time.
     * Total time taken = O(lg(n) + nlg(lg(n)) + n) = O(nlg(lg(n)))
     */
    size_t serial(std::vector<bool>& A) {
        const size_t n = A.size();
        size_t sqrt_n = sqrt(n);
        for (size_t i = 2; i <= sqrt_n; i++) {
            if (A[i] == true) continue;
            for (size_t j = i * i; j < n; j += i)
                A[j] = true;
        }
        size_t prime_count = 0;
        for (size_t i = 2; i < n; i++)
            if (A[i] == false) prime_count++;
        return prime_count;
    }

    /**
     * N < sqrt(n) is the number of processors
     * Computing square root of n takes lg(n) time
     * Each processor has to mark multiples of a prime in its assigned range of size n/N.
     * Marking multiples in parallel takes O(n/N*lg(lg(n/N))) time.
     * Each processor counts the number of primes in its assigned range in O(n/N) time.
     * Total time = O(lg(n) + n/N*lg(lg(n/N)) + n/N) = O(n/N*lg(lg(n/N))) [N < sqrt(n) => n/N > sqrt(n) > lg(n)]
     */
    size_t parallel_thread(std::vector<bool>& A) {
        size_t n = A.size();
        size_t sqrt_n = sqrt(n);
        for (size_t i = 2; i <= sqrt_n; i++) {
            if (A[i] == true) continue;
            std::vector<std::thread> threads;
            for (uint8_t tid = 1; tid < MAX_THREADS; tid++)
                threads.emplace_back(mark_multiples, std::ref(A), i, tid);
            mark_multiples(A, i, 0);
            for (auto& t: threads) t.join();
        }
        std::atomic<size_t> prime_count = 0;
        std::vector<std::thread> threads;
        for (uint8_t tid = 1; tid < MAX_THREADS; tid++)
            threads.emplace_back(count_primes, std::ref(A), std::ref(prime_count), tid);
        count_primes(A, prime_count, 0);
        for (auto& t: threads) t.join();
        return prime_count;
    }

    /**
     * Only major optimization is the division of numbers among processors when marking multiples of a prime.
     * For any prime i, the range to be traversed is [i^2, n). Out of this, every i-th element is checked.
     * Total numbers checked = (n-i^2)/i
     * This is divided among N processors as (n-i^2)/i * 1/N = o(n/N*lg(lg(n/N)))
     */
    size_t parallel_omp(std::vector<bool>& A) {
        size_t sqrt_n = sqrt(A.size());
        for (size_t i = 2; i <= sqrt_n; i++) {
            if (A[i] == true) continue;
            #pragma omp parallel for
            for (size_t j = i * i; j < A.size(); j += i)
                A[j] = true;
        }
        size_t prime_count = 0;
        #pragma omp parallel for reduction(+: prime_count)
        for (size_t i = 2; i < A.size(); i++)
                if (A[i] == false)
                    prime_count++;
        return prime_count;
    }
}

#endif

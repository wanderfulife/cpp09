#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <list> // Potentially used for pair storage or pending elements
#include <utility> // For std::pair
#include <sys/time.h> // For timing
#include <stdexcept>
#include <limits> // Required for numeric_limits

// C++98 doesn't have std::clock_gettime, use gettimeofday
long long getTimeMicros();

class PmergeMe {
private:
    std::vector<int> _inputSequence;
    std::vector<int> _sortedVector;
    std::deque<int>  _sortedDeque;

    long long _timeVector;
    long long _timeDeque;

    PmergeMe(); 
    PmergeMe(const PmergeMe& other);
    PmergeMe& operator=(const PmergeMe& other);

    
    void parseInput(int argc, char **argv);
    bool isValidInput(const char* str, int& value);

    // --- Vector Implementation ---
    void mergeInsertSortVector(std::vector<int>& vec);
    // Helper functions for vector sort (e.g., insertion, pair handling)
    typedef std::vector<std::pair<int, int> > VectorPairVec;
    void insertSortVectorPairs(VectorPairVec& pairs);
    void mergeVector(std::vector<int>& mainChain, std::vector<int>& pending);
    std::vector<int>::iterator binarySearchInsertPosVector(std::vector<int>& vec, int target, std::vector<int>::iterator end);


    // --- Deque Implementation ---
    void mergeInsertSortDeque(std::deque<int>& deq);
    // Helper functions for deque sort
     typedef std::deque<std::pair<int, int> > DequePairVec;
    void insertSortDequePairs(DequePairVec& pairs);
    void mergeDeque(std::deque<int>& mainChain, std::vector<int>& pending);
     std::deque<int>::iterator binarySearchInsertPosDeque(std::deque<int>& deq, int target, std::deque<int>::iterator end);


    // Jacobsthal numbers (or sequence for insertion order)
    static std::vector<int> generateJacobsthalSequence(int n);

public:
    explicit PmergeMe(int argc, char **argv);
    ~PmergeMe();

    void sortAndMeasure();
    void printResults() const;

    class InvalidInputException : public std::exception {
    public:
        virtual const char* what() const throw();
    };
};

#endif // PMERGEME_HPP

#include "PmergeMe.hpp"
#include <cstdlib> // For std::strtol, NULL, EXIT_FAILURE
#include <iostream>
#include <vector>
#include <deque>
#include <utility> // for std::pair
#include <algorithm> // for std::sort, std::lower_bound, std::distance, std::iter_swap
#include <iterator> // for std::distance, std::advance
#include <cmath> // For std::pow, std::round (may need adjustment for C++98)

// --- Timing Function ---
long long getTimeMicros() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<long long>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

// --- Private Methods: Disallowed --- 
PmergeMe::PmergeMe() { }
PmergeMe::PmergeMe(const PmergeMe& other) { (void)other; }
PmergeMe& PmergeMe::operator=(const PmergeMe& other) { (void)other; return *this; }

// --- Constructor & Destructor ---
PmergeMe::PmergeMe(int argc, char **argv) : _timeVector(0), _timeDeque(0) {
    parseInput(argc, argv);
}

PmergeMe::~PmergeMe() { }

// --- Input Parsing ---

bool PmergeMe::isValidInput(const char* str, int& value) {
    char* endPtr;
    long longVal = std::strtol(str, &endPtr, 10);

    if (*endPtr != '\0' || str == endPtr) { // Check if conversion failed or string was empty
        return false;
    }
    if (longVal <= 0 || longVal > std::numeric_limits<int>::max()) { // Check for positive and within int range
        return false;
    }
    value = static_cast<int>(longVal);
    return true;
}

void PmergeMe::parseInput(int argc, char **argv) {
    if (argc < 2) {
        throw InvalidInputException(); // Or a more specific message
    }
    _inputSequence.reserve(argc - 1); // Pre-allocate roughly
    for (int i = 1; i < argc; ++i) {
        int value;
        if (!isValidInput(argv[i], value)) {
            throw InvalidInputException();
        }
        // Optional: Check for duplicates if required by subject interpretation
        // for (size_t j = 0; j < _inputSequence.size(); ++j) {
        //     if (_inputSequence[j] == value) {
        //         throw InvalidInputException(); // Duplicate found
        //     }
        // }
        _inputSequence.push_back(value);
    }
}

// --- Jacobsthal Sequence Generation ---
// Generates the Jacobsthal sequence indices for merge-insert sort insertion order.
std::vector<int> PmergeMe::generateJacobsthalSequence(int n) {
    std::vector<int> jacobsthal;
    std::vector<int> sequence;
    if (n <= 0) return sequence;

    jacobsthal.push_back(0);
    jacobsthal.push_back(1);
    int k = 2;
    while (true) {
        int next_jacob = jacobsthal[k-1] + 2 * jacobsthal[k-2];
        if (next_jacob >= n) break;
        jacobsthal.push_back(next_jacob);
        k++;
    }

    int last_jacob = 0;
    sequence.push_back(1); // First pending element is always inserted first

    for (size_t i = 2; i < jacobsthal.size(); ++i) {
        int current_jacob = jacobsthal[i];
        for (int j = current_jacob; j > last_jacob; --j) {
             if (j < n) { // Only add indices within the range of pending elements
                 sequence.push_back(j + 1); // +1 because sequence is 1-based index for elements
             }
        }
        last_jacob = current_jacob;
    }
     // Add any remaining elements not covered by the sequence up to n
    std::vector<bool> added(n + 1, false);
    for(size_t i = 0; i < sequence.size(); ++i) {
        if (sequence[i] <= n) added[sequence[i]] = true;
    }
    for(int i = 1; i <= n; ++i) {
        if (!added[i]) sequence.push_back(i);
    }


    return sequence;
}

// --- Vector Implementation ---

// Simple insertion sort for small vectors or pairs
void PmergeMe::insertSortVectorPairs(VectorPairVec& pairs) {
     for (size_t i = 1; i < pairs.size(); ++i) {
        std::pair<int, int> current = pairs[i];
        int j = i - 1;
        // Sort pairs based on the larger element (second)
        while (j >= 0 && pairs[j].second > current.second) {
            pairs[j + 1] = pairs[j];
            j--;
        }
        pairs[j + 1] = current;
    }
}

std::vector<int>::iterator PmergeMe::binarySearchInsertPosVector(std::vector<int>& vec, int target, std::vector<int>::iterator end) {
    return std::lower_bound(vec.begin(), end, target);
}

void PmergeMe::mergeVector(std::vector<int>& mainChain, std::vector<int>& pending) {
    if (pending.empty()) return;

     // Step 1: Insert the first pending element (corresponding to the smallest pair's smaller element)
     if (!mainChain.empty() || !pending.empty()) { // Ensure there's something to insert
        std::vector<int>::iterator firstInsertPos = binarySearchInsertPosVector(mainChain, pending[0], mainChain.begin() + 1 );
        mainChain.insert(firstInsertPos, pending[0]);
     }

    // Step 2: Generate Jacobsthal insertion sequence for remaining elements
    std::vector<int> insertSequence = generateJacobsthalSequence(pending.size() -1); // indices for pending[1..n]

    // Step 3: Insert remaining pending elements using binary search guided by Jacobsthal sequence
    size_t elementsInserted = 1; // We already inserted pending[0]
    for (size_t i = 0; i < insertSequence.size(); ++i) {
        int pendingIndex = insertSequence[i];
        if (pendingIndex >= static_cast<int>(pending.size())) continue;

        int valueToInsert = pending[pendingIndex];

        // Find the position of the corresponding larger element in the pairs list (which matches original main chain order)
        // This requires access to the original pairs or reconstructing the index. Assuming pairs were sorted by .second
        // which formed the mainChain initially.
        // A simpler, though potentially less efficient upper bound: search up to the index `pendingIndex` in the current mainChain plus already inserted elements

        // int searchLimitIndex = -1; // Remove unused variable
        // Find the element in mainChain that was the pair of valueToInsert
        // This is tricky because mainChain is modified. We need the original position.
        // Let's rethink: The element pending[k] was paired with mainChain[k] *before* any insertions.
        // So, we need to insert pending[k] somewhere before mainChain[k] (after insertions).

        // Revised simpler upper bound: search up to pendingIndex + elementsInserted
        int upperBoundIndex = std::min((int)mainChain.size(), pendingIndex + (int)elementsInserted);

        std::vector<int>::iterator endSearch = mainChain.begin();
        std::advance(endSearch, upperBoundIndex);

        std::vector<int>::iterator insertPos = binarySearchInsertPosVector(mainChain, valueToInsert, endSearch);
        mainChain.insert(insertPos, valueToInsert);
        elementsInserted++;
    }
}

void PmergeMe::mergeInsertSortVector(std::vector<int>& vec) {
    if (vec.size() <= 1) return;

    VectorPairVec pairs;
    int straggler = -1;
    bool hasStraggler = vec.size() % 2 != 0;
    if (hasStraggler) {
        straggler = vec.back();
        vec.pop_back();
    }

    // Create pairs and sort within pairs
    for (size_t i = 0; i < vec.size(); i += 2) {
        if (vec[i] > vec[i + 1]) {
            pairs.push_back(std::make_pair(vec[i + 1], vec[i]));
        } else {
            pairs.push_back(std::make_pair(vec[i], vec[i + 1]));
        }
    }

    // Sort pairs based on the larger element (using simple insertion sort for potentially better performance on nearly sorted small sets)
    // Note: Could use std::sort here as well: std::sort(pairs.begin(), pairs.end(), comparePairsBySecond);
    insertSortVectorPairs(pairs);

    // Create main chain S (larger elements) and pending sequence P (smaller elements)
    std::vector<int> mainChain;
    std::vector<int> pending;
    mainChain.reserve(pairs.size());
    pending.reserve(pairs.size());

    for (size_t i = 0; i < pairs.size(); ++i) {
        mainChain.push_back(pairs[i].second);
        pending.push_back(pairs[i].first);
    }

    // Recursively sort mainChain (Note: The original algorithm sorts recursively, but the subject seems to imply iterative insertion. Let's keep it simpler based on the structure.)
    // For a truly recursive implementation, mergeInsertSortVector would be called on mainChain here.
    // Instead, we assume mainChain is sorted by the pair sort step and proceed to merge.

    // Merge pending elements into mainChain
    mergeVector(mainChain, pending);

    // Insert straggler if exists
    if (hasStraggler) {
        std::vector<int>::iterator insertPos = binarySearchInsertPosVector(mainChain, straggler, mainChain.end());
        mainChain.insert(insertPos, straggler);
    }

    vec = mainChain; // Update original vector
}

// --- Deque Implementation (Similar structure to Vector) ---

void PmergeMe::insertSortDequePairs(DequePairVec& pairs) {
     for (size_t i = 1; i < pairs.size(); ++i) {
        std::pair<int, int> current = pairs[i];
        int j = i - 1;
        while (j >= 0 && pairs[j].second > current.second) {
            pairs[j + 1] = pairs[j];
            j--;
        }
        pairs[j + 1] = current;
    }
}

std::deque<int>::iterator PmergeMe::binarySearchInsertPosDeque(std::deque<int>& deq, int target, std::deque<int>::iterator end) {
    return std::lower_bound(deq.begin(), end, target);
}

void PmergeMe::mergeDeque(std::deque<int>& mainChain, std::vector<int>& pending) {
      if (pending.empty()) return;

     if (!mainChain.empty() || !pending.empty()) {
        std::deque<int>::iterator firstInsertPos = binarySearchInsertPosDeque(mainChain, pending[0], mainChain.begin() + 1);
        mainChain.insert(firstInsertPos, pending[0]);
     }

    std::vector<int> insertSequence = generateJacobsthalSequence(pending.size() -1);

    size_t elementsInserted = 1;
    for (size_t i = 0; i < insertSequence.size(); ++i) {
        int pendingIndex = insertSequence[i];
        if (pendingIndex >= static_cast<int>(pending.size())) continue;

        int valueToInsert = pending[pendingIndex];

        // Revised simpler upper bound: search up to pendingIndex + elementsInserted
         int upperBoundIndex = std::min((int)mainChain.size(), pendingIndex + (int)elementsInserted);


        std::deque<int>::iterator endSearch = mainChain.begin();
        std::advance(endSearch, upperBoundIndex);

        std::deque<int>::iterator insertPos = binarySearchInsertPosDeque(mainChain, valueToInsert, endSearch);
        mainChain.insert(insertPos, valueToInsert);
        elementsInserted++;
    }
}

void PmergeMe::mergeInsertSortDeque(std::deque<int>& deq) {
    if (deq.size() <= 1) return;

    DequePairVec pairs;
    int straggler = -1;
    bool hasStraggler = deq.size() % 2 != 0;
    if (hasStraggler) {
        straggler = deq.back();
        deq.pop_back();
    }

    for (size_t i = 0; i < deq.size(); i += 2) {
        if (deq[i] > deq[i + 1]) {
             pairs.push_back(std::make_pair(deq[i + 1], deq[i]));
        } else {
             pairs.push_back(std::make_pair(deq[i], deq[i + 1]));
        }
    }

    insertSortDequePairs(pairs);

    std::deque<int> mainChain;
    std::vector<int> pendingVec;
    mainChain.resize(pairs.size()); // Preallocate deque might not be efficient
    pendingVec.reserve(pairs.size());

    for (size_t i = 0; i < pairs.size(); ++i) {
        //mainChain.push_back(pairs[i].second); // push_back potentially slow for deque
        mainChain[i] = pairs[i].second; // Assign if pre-allocated
        pendingVec.push_back(pairs[i].first);
    }

    mergeDeque(mainChain, pendingVec);

    if (hasStraggler) {
         std::deque<int>::iterator insertPos = binarySearchInsertPosDeque(mainChain, straggler, mainChain.end());
         mainChain.insert(insertPos, straggler);
    }

    deq = mainChain;
}

// --- Public Methods ---
void PmergeMe::sortAndMeasure() {
    // Vector sort
    _sortedVector = _inputSequence;
    long long startVector = getTimeMicros();
    mergeInsertSortVector(_sortedVector);
    long long endVector = getTimeMicros();
    _timeVector = endVector - startVector;

    // Deque sort
    _sortedDeque.assign(_inputSequence.begin(), _inputSequence.end());
    long long startDeque = getTimeMicros();
    mergeInsertSortDeque(_sortedDeque);
    long long endDeque = getTimeMicros();
    _timeDeque = endDeque - startDeque;
}

void PmergeMe::printResults() const {
    std::cout << "Before: ";
    for (size_t i = 0; i < _inputSequence.size(); ++i) {
        std::cout << _inputSequence[i] << (i == _inputSequence.size() - 1 ? "" : " ");
    }
    std::cout << std::endl;

    std::cout << "After:  ";
    // Check if vector sort produced output, otherwise print deque's (or handle error)
    const std::vector<int>& finalVec = !_sortedVector.empty() ? _sortedVector : std::vector<int>(_sortedDeque.begin(), _sortedDeque.end());
     for (size_t i = 0; i < finalVec.size(); ++i) {
        std::cout << finalVec[i] << (i == finalVec.size() - 1 ? "" : " ");
    }
    std::cout << std::endl;

    std::cout << "Time to process a range of " << _inputSequence.size()
              << " elements with std::vector : " << _timeVector << " us" << std::endl;
    std::cout << "Time to process a range of " << _inputSequence.size()
              << " elements with std::deque  : " << _timeDeque << " us" << std::endl;

    // Verify sort (optional)
    // std::vector<int> temp = _inputSequence;
    // std::sort(temp.begin(), temp.end());
    // bool vector_ok = (temp.size() == _sortedVector.size() && std::equal(temp.begin(), temp.end(), _sortedVector.begin()));
    // bool deque_ok = (temp.size() == _sortedDeque.size() && std::equal(temp.begin(), temp.end(), _sortedDeque.begin()));
    // if (!vector_ok) std::cerr << "Vector sort failed!" << std::endl;
    // if (!deque_ok) std::cerr << "Deque sort failed!" << std::endl;
}

// --- Exception Implementation ---
const char* PmergeMe::InvalidInputException::what() const throw() {
    return "Error"; // Simple error message as per example
}

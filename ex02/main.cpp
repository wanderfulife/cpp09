#include "PmergeMe.hpp"
#include <iostream>
#include <exception>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <positive_integer_sequence>" << std::endl;
        std::cerr << "Error: No input sequence provided." << std::endl;
        return 1;
    }

    try {
        PmergeMe sorter(argc, argv);
        sorter.sortAndMeasure();
        sorter.printResults();
    } catch (const PmergeMe::InvalidInputException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        // Catch other potential standard exceptions during processing
        std::cerr << "Error: An unexpected error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

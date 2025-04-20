#include "RPN.hpp"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Error: Invalid number of arguments." << std::endl;
        std::cerr << "Usage: ./RPN \"<expression>\"" << std::endl;
        return 1;
    }

    std::string expression = argv[1];

    try {
        int result = RPN::evaluate(expression);
        std::cout << result << std::endl;
    } catch (const RPN::EvaluationException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: An unexpected error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

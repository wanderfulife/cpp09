#include "RPN.hpp"
#include <sstream>
#include <string>
#include <cstdlib> // For std::atoi
#include <iostream> // For cerr (if needed in helper, though likely not)

RPN::RPN() { }
RPN::RPN(const RPN& other) { (void)other; }
RPN& RPN::operator=(const RPN& other) { (void)other; return *this; }
RPN::~RPN() { }


int RPN::performOp(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) throw EvaluationException("Division by zero");
            return a / b;
        default: 
            throw EvaluationException("Invalid operator"); 
    }
}


int RPN::evaluate(const std::string& expression) {
    std::stack<int> stack;
    std::stringstream ss(expression);
    std::string token;

    while (ss >> token) {
        if (token.length() == 1 && std::string("+-*/").find(token[0]) != std::string::npos) {
           if (stack.size() < 2) {
                throw EvaluationException("Insufficient operands for operator");
            }
            int operand2 = stack.top(); stack.pop();
            int operand1 = stack.top(); stack.pop();
            stack.push(performOp(operand1, operand2, token[0]));
        } else {
            
            if (token.length() != 1 || !std::isdigit(token[0])) {
                 throw EvaluationException("Invalid token: " + token);
            }
            stack.push(std::atoi(token.c_str()));
        }
    }

    if (stack.size() != 1) {
        throw EvaluationException("Invalid expression: too many operands or operators left");
    }

    return stack.top();
}

RPN::EvaluationException::EvaluationException(const std::string& message) : _message("Error: " + message) {}

RPN::EvaluationException::~EvaluationException() throw() {}

const char* RPN::EvaluationException::what() const throw() {
    return _message.c_str();
}

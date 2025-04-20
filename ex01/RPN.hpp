#ifndef RPN_HPP
#define RPN_HPP

#include <string>
#include <stack>
#include <stdexcept>

class RPN {
private:
    RPN(); 
    RPN(const RPN& other);
    RPN& operator=(const RPN& other);
    ~RPN();

    static int performOp(int a, int b, char op);

public:
    static int evaluate(const std::string& expression);

    class EvaluationException : public std::exception {
        private:
            std::string _message;
        public:
            EvaluationException(const std::string& message);
            virtual ~EvaluationException() throw(); // Destructor needs throw()
            virtual const char* what() const throw();
    };
};

#endif // RPN_HPP

#include "cppexprpars.hpp"
#include <iostream>

int main(void) {
    cppexprpars::ExprParser parser;

    // Set the expression to be computed
    parser.set_expression("abs(x) * sin(y) + 2");

    // Set variables
    parser.set_variable("x", -3.0);
    parser.set_variable("y", 1.57);  // ~pi/2

    // Register a new function
    parser.register_function("abs", [](const std::vector<double>& args) {
        if (args.size() != 1) throw std::runtime_error("abs expects 1 argument");
        return std::abs(args[0]);
    });

    // Evaluate the expression and get the result
    auto result = parser.evaluate();
    std::cout << "Result: " << result << std::endl;  // Should be close to 5.0
}

#include "cppexprpars/parser.hpp"
#include <iostream>

int main(void) {
    cppexprpars::ExprParser parser;
    parser.set_expression("x * sin(y) + 2");

    parser.set_variable("x", 3.0);
    parser.set_variable("y", 1.57);  // ~pi/2

    std::cout << "x = " << parser.get_variable("x") << std::endl;
    std::cout << "y = " << parser.get_variable("y") << std::endl;

    auto result = parser.evaluate();
    std::cout << "Result: " << result << std::endl;  // Should be close to 5.0
}

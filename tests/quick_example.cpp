#include "cppexprpars.hpp"
#include <iostream>

int main(void) {
    cppexprpars::ExprParser parser;

    // Set the expression to be computed
    parser.set_expression("abs(x) * sin(y) + 2");

    // Set variables
    parser.set_variable("x", -3.0);
    parser.set_variable("y", 1.57);  // ~pi/2

    // Register a new function (name, logic, number of arguments)
    parser.register_function("abs", [](const std::vector<double>& args) {
        return std::abs(args[0]);
    }, 1);

    // Override sqrt() function with custom ArityMismatchHandler
    parser.register_function(
        "sqrt",
        [](const std::vector<double>& args) {
            return std::sqrt(args[0]);
        },
        1,
        [](const std::string& name, size_t expected, size_t received) {
            std::cerr << "Function `" << name << "` received wrong number of args: "
                    << "expected " << expected << ", got " << received << std::endl;
        }
    );

    // Evaluate the expression and get the result
    auto result = parser.evaluate();
    std::cout << "Result: " << result << std::endl;  // Should be 5.0 (or very close to it)

    // Test overriden sqrt() function
    parser.set_expression("27 / sqrt(9, 2)");        // Should output "Function `sqrt` received wrong number of args: expected 1, got 2"
    result = parser.evaluate();
    std::cout << "Result: " << result << std::endl;  // Should be `nan`
}

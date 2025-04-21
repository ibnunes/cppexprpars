#include "cppexprpars/tokenizer.hpp"
#include "cppexprpars/parser.hpp"
#include "cppexprpars/expr_node.hpp"
#include "cppexprpars/function.hpp"
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <cassert>
#include <unordered_map>

using namespace cppexprpars;

// Define a simple variable resolver using an unordered_map
// std::unordered_map<std::string, double> variables = {
//     {"x", 10.0}, // predefined variable for testing
// };

// Simple function to resolve variables directly from the map
// double resolve_variable(const std::string& name) {
//     if (variables.find(name) != variables.end()) {
//         return variables[name];
//     }
//     throw std::runtime_error("Unknown variable: " + name);
// }

void test_constant_expression() {
    Tokenizer tokenizer("3.14");
    Parser parser(std::move(tokenizer));
    auto expr = parser.parse();
    assert(std::abs(expr->evaluate() - 3.14) < 1e-6);
    std::cout << "test_constant_expression passed!" << std::endl;
}

void test_variable_expression() {
    Tokenizer tokenizer("x + 2");
    Parser parser(std::move(tokenizer));

    EvaluationContext context = EvaluationContext::default_context();
    context.set_variable("x", 10.0);
    cppexprpars::VariableExprNode::set_default_context(&context);

    // Use the simple variable resolver to resolve "x"
    auto expr = parser.parse();
    assert(std::abs(expr->evaluate() - 12.0) < 1e-6); // 10.0 (x) + 2 = 12.0
    std::cout << "test_variable_expression passed!" << std::endl;
}

void test_variable_expression_2() {
    ExprParser parser;  // Automatically loads with default_context() and default_registry()
    parser.set_expression("x + 2");
    parser.set_variable("x", 10.0);
    ExprFloat result = parser.evaluate();
    assert(std::abs(result - 12.0) < 1e-6); // 10.0 (x) + 2 = 12.0
    std::cout << "test_variable_expression passed!" << std::endl;
}

void test_function_expression() {
    Tokenizer tokenizer("sqrt(16)");
    Parser parser(std::move(tokenizer));

    // Set up the FunctionRegistry
    FunctionRegistry registry = FunctionRegistry::default_registry();
    cppexprpars::FuncExprNode::set_default_registry(&registry);

    auto expr = parser.parse();
    assert(std::abs(expr->evaluate() - 4.0) < 1e-6); // sqrt(16) = 4
    std::cout << "test_function_expression passed!" << std::endl;
}

void test_binary_expression() {
    Tokenizer tokenizer("2 + 3 * x");
    Parser parser(std::move(tokenizer));

    // Use the simple variable resolver to resolve "x"
    auto expr = parser.parse();
    assert(std::abs(expr->evaluate() - 32.0) < 1e-6); // 2 + 3 * 10 = 32.0
    std::cout << "test_binary_expression passed!" << std::endl;
}

int main() {
    try {
        test_constant_expression();
        test_variable_expression();
        test_variable_expression_2();
        test_function_expression();
        test_binary_expression();

        std::cout << "All tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

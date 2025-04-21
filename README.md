# cppexprpars

A lightweight and modern C++ expression parser and evaluator.

`cppexprpars` parses and evaluates mathematical expressions from strings, supporting variables, functions, and basic arithmetic operations. It is designed to be simple to integrate and easy to extend.

## Features

- Supports `+`, `-`, `*`, `/`, `^` operators
- Parentheses for grouping
- Floating point numbers (including scientific notation)
- User-defined variables
- Built-in and custom functions
- Clean and modern C++ interface
- Zero dependencies

## Quick Example

```cpp
#include "cppexprpars/parser.hpp"

int main(void) {
    cppexprpars::ExprParser parser;
    parser.set_expression("x * sin(y) + 2");

    parser.set_variable("x", 3.0);
    parser.set_variable("y", 1.57);  // ~pi/2

    auto result = parser.evaluate();
    std::cout << "Result: " << result << std::endl;  // Should be close to 5.0
}
```

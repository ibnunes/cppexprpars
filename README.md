# cppexprpars

A lightweight and modern C++ expression parser.

`cppexprpars` parses and evaluates mathematical expressions from strings, supporting variables, functions, and basic arithmetic operations. It is designed to be simple to integrate, easy to extend, and fast enough for embedded and desktop applications alike.


## Features

- Basic math operators: `+`, `-`, `*`, `/`, `%`, `^`;
- Proper operator precedence and parentheses grouping;
- Floating point literals (including scientific notation);
- Built-in functions: `sin`, `cos`, `tan`, `log`, `exp`, `sqrt`;
- Custom function registration;
- Named variables (both lowercase and uppercase: `a–z`, `A–Z`);
- Zero external dependencies.

## Usage

### Quick Example

```cpp
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
```

### Extending

- Add custom functions with `register_function(name, callback, nargs, [on_invalid_args])`
- Modify the context at runtime with `set_variable(name, value)`
- Access or override function and variable resolution

### Integration

You can simply add the source files to your project:

1. Clone the repo
2. Include the headers from `cppexprpars/`
3. Link your build system to the parser source (or use the static library via CMake)

<!--
## How It Works

`cppexprpars` tokenizes the input expression, parses it into an abstract syntax tree (AST), and evaluates it using a context-aware system:

- `ExprParser` is the high-level interface
- `EvaluationContext` holds variable bindings
- `FunctionRegistry` maps string names to callable functions
-->

<!--
## Testing

This library includes a test suite using standard `assert()`-based tests.

To run tests, simply compile and run the test binary:

```
./build.sh Debug
```
-->

## License

* **MIT License**
* See [`LICENSE`](LICENSE.md) for details


## Contributing

Contributions, bug reports, and ideas are welcome!
Feel free to open issues or submit pull requests.

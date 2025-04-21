#pragma once

namespace cppexprpars {

enum class BinaryOp {
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    Power
    // TODO: Add more binary operators?
};

enum class UnaryOp {
    Plus,
    Minus
    // TODO: Add more unary operators?
};

} // namespace cppexprpars

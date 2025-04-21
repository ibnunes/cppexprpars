#pragma once

#include "cppexprpars/expr_node.hpp"
#include <string>
#include <variant>
#include <cctype>

namespace cppexprpars {

enum class TokenType {
    End,
    Number,
    Identifier,
    Plus, Minus, Star, Slash,
    Caret,
    LeftParen, RightParen,
    Comma,
    Invalid
};

static const char* strOfTok[] = {
    "End",
    "Number",
    "Identifier",
    "Plus", "Minus", "Star", "Slash",
    "Caret",
    "LeftParen", "RightParen",
    "Comma",
    "Invalid"
};

struct Token {
    TokenType type;
    std::string text;
    ExprFloat number_value = 0.0;

    Token() : type(TokenType::Invalid), text(""), number_value(0.0) {}

    Token(TokenType t, const std::string& txt = "", ExprFloat val = 0.0)
        : type(t), text(txt), number_value(val) {}

    Token& operator=(const Token& other) = default;

    // If needed in the future, the `=` operator overload will have to be like this:
    // Token& operator=(const Token& other) {
    //     type = other.type;
    //     text = other.text;
    //     number_value = other.number_value;
    //     return *this;
    // }
};

} // namespace cppexprpars

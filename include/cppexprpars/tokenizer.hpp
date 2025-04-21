#pragma once

#include "token.hpp"
#include <string>
#include <cctype>
#include <stdexcept>

namespace cppexprpars {

class Tokenizer {
public:
    explicit Tokenizer(const std::string& input) : input_(input), pos_(0) {
        next_token();
    }

    const Token& current() const { return current_token_; }

    void next_token() {
        skip_whitespace();
        if (pos_ >= input_.size()) {
            current_token_ = {TokenType::End, ""};
            return;
        }

        char ch = input_[pos_];
        if (std::isdigit(ch) || ch == '.') {
            parse_number();
        } else if (std::isalpha(ch) || ch == '_') {
            parse_identifier();
        } else {
            switch (ch) {
                case '+': make_token(TokenType::Plus); break;
                case '-': make_token(TokenType::Minus); break;
                case '*': make_token(TokenType::Star); break;
                case '/': make_token(TokenType::Slash); break;
                case '^': make_token(TokenType::Caret); break;
                case '(': make_token(TokenType::LeftParen); break;
                case ')': make_token(TokenType::RightParen); break;
                case ',': make_token(TokenType::Comma); break;
                default:
                    current_token_ = {TokenType::Invalid, std::string(1, ch)};
                    ++pos_;
                    break;
            }
        }
    }

private:
    std::string input_;
    size_t pos_;
    Token current_token_;

    void skip_whitespace() {
        while (pos_ < input_.size() && std::isspace(input_[pos_]))
            ++pos_;
    }

    void parse_number() {
        size_t start = pos_;
        bool has_dot = false;

        // Integer and fractional part
        while (pos_ < input_.size() && (std::isdigit(input_[pos_]) || input_[pos_] == '.')) {
            if (input_[pos_] == '.') {
                if (has_dot) break; // Only one dot allowed
                has_dot = true;
            }
            ++pos_;
        }

        // Scientific notation (e.g. 1.2e-3)
        if (pos_ < input_.size() && (input_[pos_] == 'e' || input_[pos_] == 'E')) {
            ++pos_;
            if (pos_ < input_.size() && (input_[pos_] == '+' || input_[pos_] == '-')) {
                ++pos_;
            }
            while (pos_ < input_.size() && std::isdigit(input_[pos_])) {
                ++pos_;
            }
        }

        std::string number_text = input_.substr(start, pos_ - start);
        try {
            double value = std::stod(number_text);
            current_token_ = {TokenType::Number, number_text, value};
        } catch (...) {
            current_token_ = {TokenType::Invalid, number_text};
        }
    }

    void parse_identifier() {
        size_t start = pos_;
        while (pos_ < input_.size() && (std::isalnum(input_[pos_]) || input_[pos_] == '_'))
            ++pos_;
        current_token_ = {TokenType::Identifier, input_.substr(start, pos_ - start)};
    }

    void make_token(TokenType type) {
        current_token_ = {type, std::string(1, input_[pos_])};
        ++pos_;
    }
};

} // namespace cppexprpars

//  cppexprpars.hpp - Lightweight C++ Expression Parser (Header)
//
//  A modern, single-file expression parser for C++ that supports variables,
//  functions, and basic arithmetic. Designed to be easy to embed and extend.
//
//  https://github.com/ibnunes/cppexprpars
//
//  -------------------------------------------------------------------------
//
//  MIT License
//
//  Copyright (c) 2025 Igor Nunes
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.



#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <functional>
#include <cmath>
#include <cctype>


namespace cppexprpars {

using ExprFloat = double;
using ExprInt = uint64_t;

using Function = std::function<ExprFloat(const std::vector<ExprFloat>&)>;
using VariableResolver = std::function<ExprFloat(const std::string&)>;


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

enum class ExprNodeType {
    Constant,
    Variable,
    Binary,
    Unary,
    Function
};

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

// static const char* strOfTok[] = {
//     "End",
//     "Number",
//     "Identifier",
//     "Plus", "Minus", "Star", "Slash",
//     "Caret",
//     "LeftParen", "RightParen",
//     "Comma",
//     "Invalid"
// };

class ExprNode {
public:
    virtual ~ExprNode() = default;
    virtual ExprFloat evaluate() const = 0;
};

using ExprNodePtr = std::unique_ptr<ExprNode>;



class BinaryExprNode : public ExprNode {
public:
    BinaryExprNode(BinaryOp op, ExprNodePtr left, ExprNodePtr right)
        : op_(op), left_(std::move(left)), right_(std::move(right)) {}

    BinaryExprNode(char op, ExprNodePtr left, ExprNodePtr right)
        : op_(charToBinaryOp(op)), left_(std::move(left)), right_(std::move(right)) {}

    ExprFloat evaluate() const override;

protected:
    static BinaryOp charToBinaryOp(char op_char);

private:
    BinaryOp op_;
    ExprNodePtr left_;
    ExprNodePtr right_;
};



class UnaryExprNode : public ExprNode {
public:
    UnaryExprNode(UnaryOp op, ExprNodePtr operand)
        : op_(op), operand_(std::move(operand)) {}

    UnaryExprNode(char op, ExprNodePtr operand)
        : op_(charToUnaryOp(op)), operand_(std::move(operand)) {}

    ExprFloat evaluate() const override;

protected:
    static UnaryOp charToUnaryOp(char op_char);

private:
    UnaryOp op_;
    ExprNodePtr operand_;
};



class FunctionRegistry {
public:
    void register_function(const std::string& name, Function fn);
    Function get_function(const std::string& name) const;

    static FunctionRegistry default_registry();

private:
    std::unordered_map<std::string, Function> functions_;
};

void set_default_registry(const FunctionRegistry* registry);
FunctionRegistry* get_default_registry();



class EvaluationContext {
public:
    void set_variable(const std::string& name, ExprFloat value);
    ExprFloat get_variable(const std::string& name) const;
    static const EvaluationContext& default_context();

private:
    std::unordered_map<std::string, ExprFloat> variables_;
};

void set_default_context(const EvaluationContext* context);
EvaluationContext* get_default_context();



class VariableExprNode : public ExprNode {
public:
    // Use predefined context
    VariableExprNode(std::string name, const EvaluationContext* context = get_default_context())
        : name_(std::move(name)),
            resolver_([context](const std::string& varName) {
                return context->get_variable(varName);
            }) {}

    // Advanced use case: custom resolver
    VariableExprNode(std::string name, VariableResolver resolver)
        : name_(std::move(name)), resolver_(std::move(resolver)) {}

    ExprFloat evaluate() const override;

    inline void set_context(const EvaluationContext* context);
    inline void set_context_as_default();

private:
    std::string name_;
    VariableResolver resolver_;
};



class FuncExprNode : public ExprNode {
public:
    FuncExprNode(std::string name,
                    std::vector<ExprNodePtr> args,
                    const FunctionRegistry* registry = get_default_registry())
        : name_(std::move(name)),
            args_(std::move(args)),
            registry_(registry) {}

    ExprFloat evaluate() const override;

    inline void set_registry(const FunctionRegistry* registry);
    inline void set_registry_as_default();

private:
    std::string name_;
    std::vector<ExprNodePtr> args_;
    const FunctionRegistry* registry_;
};



class ConstantExprNode : public ExprNode {
public:
    explicit ConstantExprNode(ExprFloat value) : value_(value) {}

    ExprFloat evaluate() const override;

private:
    ExprFloat value_;
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



class Tokenizer {
public:
    explicit Tokenizer(const std::string& input) : input_(input), pos_(0) {
        next_token();
    }

    inline const Token& current() const { return current_token_; }
    void next_token();


private:
    std::string input_;
    size_t pos_;
    Token current_token_;

    void skip_whitespace();
    void parse_number();
    void parse_identifier();
    void make_token(TokenType type);
};



class Parser {
public:
    explicit Parser(Tokenizer tokenizer) :
        tokenizer_(std::move(tokenizer)),
        context_(cppexprpars::get_default_context()),
        registry_(cppexprpars::get_default_registry())
        {}

    explicit Parser(Tokenizer tokenizer, EvaluationContext* context, FunctionRegistry* registry) :
        tokenizer_(std::move(tokenizer)),
        context_(context),
        registry_(registry)
        {}

    std::unique_ptr<ExprNode> parse();

    inline void set_context(EvaluationContext* context) {
        this->context_ = context;
    }

    inline void set_registry(FunctionRegistry* registry) {
        this->registry_ = registry;
    }

private:
    Tokenizer tokenizer_;
    EvaluationContext* context_;
    FunctionRegistry* registry_;

    std::unique_ptr<ExprNode> parse_expression(int precedence = 0);
    std::unique_ptr<ExprNode> parse_primary();
    int get_precedence(TokenType type) const;
    bool is_right_associative(TokenType type) const;
};



class ExprParser {
public:
    ExprParser() :
        context_(EvaluationContext::default_context()),
        registry_(FunctionRegistry::default_registry()) { }

    void set_expression(const std::string& expr);
    void set_variable(const std::string& name, ExprFloat value);
    ExprFloat get_variable(const std::string& name) const;
    void register_function(const std::string& name, Function fn);
    ExprFloat evaluate();

private:
    std::string expression_;
    EvaluationContext context_;
    FunctionRegistry registry_;
};

}   // namespace cppexprpars

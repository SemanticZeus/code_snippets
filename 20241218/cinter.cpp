#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <vector>
#include <stdexcept>
#include <cctype>

// Token types
enum class TokenType { Identifier, Number, Operator, Keyword, Symbol, End };

// Token structure
struct Token {
    TokenType type;
    std::string value;
};

// Lexer: Tokenizes input
class Lexer {
public:
    explicit Lexer(const std::string& input) : input_(input), pos_(0) {}

    Token next() {
        skipWhitespace();
        if (pos_ >= input_.size()) return {TokenType::End, ""};

        char ch = input_[pos_];
        if (std::isalpha(ch)) return identifier();
        if (std::isdigit(ch)) return number();
        if (isOperator(ch)) return comparisonOperator();
        if (isSymbol(ch)) return {TokenType::Symbol, std::string(1, input_[pos_++])};

        throw std::runtime_error("Unexpected character: " + std::string(1, ch));
    }

private:
    std::string input_;
    size_t pos_;

    void skipWhitespace() {
        while (pos_ < input_.size() && std::isspace(input_[pos_])) ++pos_;
    }

    Token identifier() {
        size_t start = pos_;
        while (pos_ < input_.size() && (std::isalnum(input_[pos_]) || input_[pos_] == '_')) ++pos_;
        std::string value = input_.substr(start, pos_ - start);
        return {value == "if" || value == "int" || value == "print" ? TokenType::Keyword : TokenType::Identifier, value};
    }

    Token number() {
        size_t start = pos_;
        while (pos_ < input_.size() && std::isdigit(input_[pos_])) ++pos_;
        return {TokenType::Number, input_.substr(start, pos_ - start)};
    }

    bool isOperator(char ch) {
        return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || 
               ch == '<' || ch == '>' || ch == '!';
    }

    Token comparisonOperator() {
        std::string op(1, input_[pos_++]);
        if ((op == "=" || op == "!" || op == "<" || op == ">") && pos_ < input_.size() && input_[pos_] == '=') {
            op += input_[pos_++];
        }
        return {TokenType::Operator, op};
    }

    bool isSymbol(char ch) {
        return ch == ';' || ch == '{' || ch == '}' || ch == '(' || ch == ')';
    }
};

// AST Node
struct ASTNode {
    std::string type; // "Declaration", "Assignment", "BinaryOp", etc.
    std::string value;
    std::vector<ASTNode> children;
};

// Parser: Builds AST
class Parser {
public:
    explicit Parser(Lexer lexer) : lexer_(lexer), current_(lexer_.next()) {}

    ASTNode parse() {
        return program();
    }

private:
    Lexer lexer_;
    Token current_;

    void advance() {
        current_ = lexer_.next();
    }

    void expect(TokenType type, const std::string& value = "") {
        if (current_.type != type || (!value.empty() && current_.value != value)) {
            throw std::runtime_error("Unexpected token: " + current_.value);
        }
        advance();
    }

    ASTNode program() {
        std::vector<ASTNode> statements;
        while (current_.type != TokenType::End && current_.value != "}") {
            statements.push_back(statement());
        }
        return {"Program", "", statements};
    }

    ASTNode statement() {
        if (current_.type == TokenType::Keyword && current_.value == "int") {
            return declaration();
        } else if (current_.type == TokenType::Identifier) {
            return assignment();
        } else if (current_.type == TokenType::Keyword && current_.value == "if") {
            return ifStatement();
        } else if (current_.type == TokenType::Keyword && current_.value == "print") {
            return printStatement();
        }
        throw std::runtime_error("Unexpected token in statement: " + current_.value);
    }

    ASTNode declaration() {
        expect(TokenType::Keyword, "int");
        std::string varName = current_.value;
        expect(TokenType::Identifier);
        expect(TokenType::Operator, "=");
        ASTNode expr = expression();
        expect(TokenType::Symbol, ";");
        return {"Declaration", varName, {expr}};
    }

    ASTNode assignment() {
        std::string varName = current_.value;
        expect(TokenType::Identifier);
        expect(TokenType::Operator, "=");
        ASTNode expr = expression();
        expect(TokenType::Symbol, ";");
        return {"Assignment", varName, {expr}};
    }

    ASTNode ifStatement() {
        expect(TokenType::Keyword, "if");
        expect(TokenType::Symbol, "(");
        ASTNode condition = expression();
        expect(TokenType::Symbol, ")");
        expect(TokenType::Symbol, "{");
        ASTNode body = program(); // Allow multiple statements inside the if block
        expect(TokenType::Symbol, "}");
        return {"IfStatement", "", {condition, body}};
    }

    ASTNode printStatement() {
        expect(TokenType::Keyword, "print");
        expect(TokenType::Symbol, "(");
        ASTNode expr = expression();
        expect(TokenType::Symbol, ")");
        expect(TokenType::Symbol, ";");
        return {"Print", "", {expr}};
    }

    ASTNode expression() {
        ASTNode left = comparison();
        while (current_.type == TokenType::Operator && (current_.value == "+" || current_.value == "-")) {
            std::string op = current_.value;
            advance();
            ASTNode right = comparison();
            left = {"BinaryOp", op, {left, right}};
        }
        return left;
    }

    ASTNode comparison() {
        ASTNode left = term();
        if (current_.type == TokenType::Operator && 
            (current_.value == "<" || current_.value == ">" || current_.value == "<=" ||
             current_.value == ">=" || current_.value == "==" || current_.value == "!=")) {
            std::string op = current_.value;
            advance();
            ASTNode right = term();
            left = {"ComparisonOp", op, {left, right}};
        }
        return left;
    }

    ASTNode term() {
        ASTNode left = factor();
        while (current_.type == TokenType::Operator && (current_.value == "*" || current_.value == "/")) {
            std::string op = current_.value;
            advance();
            ASTNode right = factor();
            left = {"BinaryOp", op, {left, right}};
        }
        return left;
    }

    ASTNode factor() {
        if (current_.type == TokenType::Number) {
            ASTNode node = {"Number", current_.value, {}};
            advance();
            return node;
        } else if (current_.type == TokenType::Identifier) {
            ASTNode node = {"Variable", current_.value, {}};
            advance();
            return node;
        } else if (current_.type == TokenType::Symbol && current_.value == "(") {
            advance();
            ASTNode node = expression();
            expect(TokenType::Symbol, ")");
            return node;
        }
        throw std::runtime_error("Unexpected token in factor: " + current_.value);
    }
};

// Interpreter: Executes the AST
class Interpreter {
public:
    void execute(const ASTNode& node) {
        if (node.type == "Program") {
            for (const auto& stmt : node.children) {
                execute(stmt);
            }
        } else if (node.type == "Declaration") {
            variables_[node.value] = evaluate(node.children[0]);
        } else if (node.type == "Assignment") {
            variables_[node.value] = evaluate(node.children[0]);
        } else if (node.type == "IfStatement") {
            if (evaluate(node.children[0])) {
                execute(node.children[1]); // Execute the body
            }
        } else if (node.type == "Print") {
            std::cout << evaluate(node.children[0]) << std::endl;
        } else {
            throw std::runtime_error("Unsupported statement type: " + node.type);
        }
    }

private:
    std::unordered_map<std::string, int> variables_;

    int evaluate(const ASTNode& node) {
        if (node.type == "Number") {
            return std::stoi(node.value);
        } else if (node.type == "Variable") {
            return variables_.at(node.value);
        } else if (node.type == "BinaryOp") {
            int left = evaluate(node.children[0]);
            int right = evaluate(node.children[1]);
            if (node.value == "+") return left + right;
            if (node.value == "-") return left - right;
            if (node.value == "*") return left * right;
            if (node.value == "/") return left / right;
        } else if (node.type == "ComparisonOp") {
            int left = evaluate(node.children[0]);
            int right = evaluate(node.children[1]);
            if (node.value == "<") return left < right;
            if (node.value == ">") return left > right;
            if (node.value == "<=") return left <= right;
            if (node.value == ">=") return left >= right;
            if (node.value == "==") return left == right;
            if (node.value == "!=") return left != right;
        }
        throw std::runtime_error("Unsupported expression type: " + node.type);
    }
};

// Main Function
int main() {
    try {
        std::string code = R"(
            int x = 10;
            int y = 20;
            if (x < y) {
                x = x + 1;
                x = 2*x+5;
            }
            print(x);
        )";

        Lexer lexer(code);
        Parser parser(lexer);
        ASTNode ast = parser.parse();

        Interpreter interpreter;
        interpreter.execute(ast);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}


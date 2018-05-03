#ifndef KURSACH_TOKENS_H
#define KURSACH_TOKENS_H

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

using namespace std;

enum class TokenType {
    Letter, Operation, LeftParenthesis, RightParenthesis
};

ostream& operator<<(ostream& o, TokenType t);

class Token {
private:
    string value;
    TokenType type;
public:
    Token(TokenType type, const string& value)
            :value(value), type(type) { }

    const string& getValue() const { return value; }

    TokenType getType() const { return type; }

    friend ostream& operator<<(ostream& o, const Token& t)
    {
        o << "[" << t.type << ", " << t.value << "]";
        return o;
    }
};

class Tokenizer {
private:
    vector<Token> tokens;
    string input;
    size_t k;
public:
    Tokenizer()
            :k(0) { }

    void tokenize(std::string input)
    {
        if (!tokens.empty()) {
            throw logic_error("");
        }

        for (char ch : input) {
            if (ch=='(') {
                tokens.emplace_back(TokenType::LeftParenthesis, "(");
            }
            else if (ch==')') {
                tokens.emplace_back(TokenType::RightParenthesis, ")");
            }
            else if (isupper(ch)) {
                string letter;
                letter += ch;
                tokens.emplace_back(TokenType::Letter, letter);
            }
            else if (isspace(ch)) {
                continue;
            }
            else if (ch=='v' || ch=='^' || ch=='!' || ch=='@') {
                string op;
                op += ch;
                tokens.emplace_back(TokenType::Operation, op);
            }
            else {
                throw invalid_argument("");
            }
        }
    }

    bool hasNext() const { return k!=tokens.size(); }

    Token next() const
    {
        if (k<tokens.size())
            return tokens[k];
        else
            throw std::out_of_range("");
    }

    void popNext()
    {
        if (k<tokens.size())
            ++k;
        else
            throw std::out_of_range("");
    }

    friend ostream& operator<<(ostream& o, const Tokenizer& t)
    {
        o << "(";
        for (auto& token: t.tokens) {
            o << token << " ";
        }
        o << ")";
        return o;
    }
};

#endif //KURSACH_TOKENS_H

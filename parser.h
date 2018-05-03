#ifndef KURSACH_PARSER_H
#define KURSACH_PARSER_H

#include <map>
#include <string>
#include <memory>
#include "tokens.h"

using namespace std;

class Node {
public:
    virtual bool evaluate(const map<string, bool>& interpret) = 0;

    virtual void print() = 0;
};

class Letter : public Node {
private:
    string name;
public:
    explicit Letter(Token t)
    {
        if (t.getType()!=TokenType::Letter)
            throw invalid_argument("");
        name = t.getValue();
    }

    bool evaluate(const map<string, bool>& interpret) override
    {
        return interpret.find(name)->second;
    }

    void print() override
    {
        cerr << name;
    }
};

class Operation : public Node {
private:
    string operation;
    vector<unique_ptr<Node>> args;
public:
    explicit Operation(Token t)
    {
        if (t.getType()!=TokenType::Operation)
            throw invalid_argument("");
        operation = t.getValue();
    }

    void addArg(unique_ptr<Node> arg)
    {
        args.push_back(move(arg));
    }

    bool evaluate(const map<string, bool>& interpret) override
    {
        bool val = args[0]->evaluate(interpret);
        if (operation=="!")
            val = !val;

        for (int i = 1; i<args.size(); ++i) {
            bool v = args[i]->evaluate(interpret);
            if (operation=="v")
                val = (val || v);
            else if (operation=="^")
                val = (val && v);
            else if (operation=="@")
                val = (val!=v);
        }

        return val;
    }

    void print() override
    {
        cerr << "[ " << operation << " ";
        for (const auto& x: args) {
            x->print();
            cerr << " ";
        }
        cerr << "]";
    }
};

unique_ptr<Node> parse(Tokenizer& t);

#endif //KURSACH_PARSER_H

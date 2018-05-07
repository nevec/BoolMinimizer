#include "parser.h"

using namespace std;

unique_ptr<Node> parseExpression(Tokenizer& t);

unique_ptr<Node> parseValue(Tokenizer& t)
{
    Token top = t.next();
    bool hasNegation = false;

    if (top.getType()==TokenType::Operation && top.getValue()=="!") {
        hasNegation = true;
        t.popNext();
        top = t.next();
    }

    if (top.getType()==TokenType::Letter) {
        t.popNext();
        auto value = unique_ptr<Node>(new Letter(top));
        if (!hasNegation)
            return value;
        else {
            auto negation = unique_ptr<Operation>(new Operation(Token(TokenType::Operation, "!")));
            negation->addArg(move(value));
            return move(negation);
        }
    }
    else if (top.getType()==TokenType::LeftParenthesis) {
        t.popNext();
        auto result = parseExpression(t);
        Token last = t.next();
        if (last.getType()!=TokenType::RightParenthesis)
            throw invalid_argument(") missing");
        t.popNext();
        if (!hasNegation)
            return result;
        else {
            auto negation = unique_ptr<Operation>(new Operation(Token(TokenType::Operation, "!")));
            negation->addArg(move(result));
            return move(negation);
        }
    }
    else
        throw invalid_argument("letter or ( expected");
}

unique_ptr<Node> parseExpression(Tokenizer& t)
{
    Token top = t.next();
//    if (top.getType() == TokenType::Operation && top.getValue() == "!") {
//        t.popNext();
//        auto val = parseValue(t);
//        auto result = unique_ptr<Operation>(new Operation(top));
//        result->addArg(move(val));
//        return move(result);
//    } else {
    auto val = parseValue(t);
    if (t.hasNext()) {
        Token op = t.next();
        if (op.getType()!=TokenType::Operation)
            return val;

        t.popNext();
        auto val2 = parseValue(t);

        auto result = unique_ptr<Operation>(new Operation(op));
        result->addArg(move(val));
        result->addArg(move(val2));
        while (t.hasNext()) {
            Token next_op = t.next();
            if (next_op.getType()!=TokenType::Operation)
                break;

            if (next_op.getValue()!=op.getValue())
                throw invalid_argument("only the same operation allowed");
            t.popNext();
            result->addArg(parseValue(t));
        }
        return move(result);
    }
    else {
        return val;
    }
//    }
}

unique_ptr<Node> parse(Tokenizer& t)
{
    if (!t.hasNext())
        throw invalid_argument("empty string");

    auto result = parseExpression(t);
    if (t.hasNext())
        throw invalid_argument("too many tokens");

    return result;
}


#include "tokens.h"

ostream& operator<<(ostream& o, TokenType t)
{
    switch (t) {
    case TokenType::Operation:
        o << "Op";
        break;
    case TokenType::Letter:
        o << "Letter";
        break;
    case TokenType::RightParenthesis:
    case TokenType::LeftParenthesis:
        o << "Parenthesis";
        break;
    }
    return o;
}

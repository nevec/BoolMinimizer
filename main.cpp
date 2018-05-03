#include <iostream>
#include <vector>
#include "tokens.h"
#include "parser.h"
using namespace std;

int main() {
    string s;
    cin >> s;
    Tokenizer tokenizer;
    tokenizer.tokenize(s);

    //cout << tokenizer << endl;
    auto tree = parse(tokenizer);
    tree->print();
    return 0;
}
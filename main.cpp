#include <iostream>
#include <vector>
#include "tokens.h"

using namespace std;

int main() {

    string s;
    cin >> s;
    Tokenizer tokenizer;
    tokenizer.tokenize(s);

    cout << tokenizer << endl;
    return 0;
}
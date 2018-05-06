#include <iostream>
#include <vector>
#include "tokens.h"
#include "parser.h"
#include "minimizer.h"
#include <bitset>
using namespace std;

int main() {
    string s;
    int n;
    cin >> n >> s;

    TruthTable t(s, n);
    auto primes = getPrimeImplicants(t);
    for (auto p:primes)
        cout << p << endl;
    return 0;
}
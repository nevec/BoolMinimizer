#include <iostream>
#include <vector>
#include "tokens.h"
#include "parser.h"
#include "minimizer.h"

using namespace std;

int main() {
    string s;
    int n;
    cin >> n >> s;

    TruthTable t(s, n);
    auto primes = getPrimeImplicants(t);

    for (auto p:primes)
        cout << p << endl;
    cout << "\n-----\n\n";

    auto finalForms = PetricksMethod(t);
    cout << finalForms.size() << endl;
    for (auto form : finalForms) {
        for (auto p: form)
            cout << p << "v";
        cout << endl;
    }

    return 0;
}
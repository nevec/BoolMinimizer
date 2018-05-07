#include <iostream>
#include <vector>
#include <algorithm>
#include "tokens.h"
#include "parser.h"
#include "minimizer.h"

using namespace std;

TruthTable tryParse(const string& s)
{
    try {
        Tokenizer tokenizer;
        tokenizer.tokenize(s);
        auto root = parse(tokenizer);
        TruthTable table(root);
        return table;
    }
    catch (exception& e) {

        return TruthTable(s);
    }
}

int main()
{
    while (true) {
        cout << "Enter expression (q for quit, h for help): ";
        string s;
        getline(cin, s);
        if (s=="q")
            return 0;
        if (s=="h") {
            cout
                    << "This program minimizes expressions of boolean algebra using Quine-McCluskey algorithm and Petrick's method.\n"
                       " There\'re 2 supported input formats:\n";
            cout << "1. Sequence of 1s and 0s from the last column of truth table.\n";
            cout << "2. Algebraic expression in standard notation. Conventions:\n"
                 << "\t(A,B,C..) - variables\n\t! NOT\n\t^  AND\n\tv  OR\n\t@  XOR\n\t-  ->(implication)" << endl;
            continue;
        }
        TruthTable table;
        try {
            table = tryParse(s);
        }
        catch (exception& e) {
            cout << "Bad expression. Try again\n";
            continue;
        }

        vector<vector<Implicant>> minimalForms(minimize(table));

        auto best = min_element(minimalForms.begin(), minimalForms.end(),
                [](const vector<Implicant>& a, const vector<Implicant>& b) {
                    return totalLength(a)<totalLength(b);
                });

        cout << "\nBest minimum form: ";
        prettyPrint(*best, table.getVars(), cout);
        cout << endl << endl;

        if (minimalForms.size()>1) {
            cout << "There\'re " << minimalForms.size() << " possible final solutions\n";

            for (const vector<Implicant>& form: minimalForms) {
                prettyPrint(form, table.getVars(), cout);
                cout << endl;
            }
        }
    }

    return 0;
}
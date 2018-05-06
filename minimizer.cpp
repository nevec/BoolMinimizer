#include "minimizer.h"
#include <cmath>
#include <algorithm>

using namespace std;

TruthTable createTable(const unique_ptr<Node>& root)
{
    set<string> vars;
    vector<bool> table; // interpretations encoded in binary form

    root->addVariable(vars);
    size_t n = vars.size();
    size_t nInterpr = (size_t) 1 << n;

    table.resize(nInterpr);

    map<string, bool> interpretation;
    for (size_t i = 0; i<nInterpr; ++i) {
        size_t k = 0;
        for (const auto& var: vars) {
            interpretation[var] = bool(i & ((size_t) 1 << k));
            ++k;
        }
        table[i] = root->evaluate(interpretation);
    }
    return TruthTable(vars, table);
}

vector<Implicant> getFDNF(const TruthTable& t) //  full disjunctive normal form
{
    vector<bool> table(t.getTable());
    vector<Implicant> fdnf;

    int n = t.getVars().size();

    for (int i = 0; i<(int) table.size(); ++i) {
        if (table[i])
            fdnf.emplace_back(i, n);
    }

    sort(fdnf.begin(), fdnf.end(), [](const Implicant& a, const Implicant& b) {
        return a.countOnes()<b.countOnes();
    });

    return fdnf;
}

vector<Implicant> getPrimeImplicants(const TruthTable& t)
{
    vector<Implicant> start(getFDNF(t));
    vector<Implicant> next;

    vector<Implicant> prime;

    while (true) {
        int n = t.getVars().size();

        vector<int> count(n+1);
        vector<bool> excluded(start.size());

        for (const auto& x: start)
            count[x.countOnes()]++;
        for (int i = 1; i<n+1; ++i)
            count[i] = count[i-1]+count[i];

        count.insert(count.begin(), 0);

        for (int i = 0; i<n; ++i) {
            int s1 = count[i];
            int e1 = count[i+1]-1;
            int s2 = count[i+1];
            int e2 = count[i+2]-1;

            for (int first = s1; first<=e1; ++first) {
                for (int second = s2; second<=e2; ++second) {
                    if (start[first].isComplementary(start[second])) {
                        next.push_back(start[first].combine(start[second]));
                        excluded[first] = excluded[second] = true;
                    }
                }
            }
        }

        for (int i = 0; i<excluded.size(); ++i)
            if (!excluded[i])
                prime.push_back(start[i]);

        if (next.empty())
            break;
        start.swap(next);
        next.clear();

        sort(start.begin(), start.end());
        auto ptr = unique(start.begin(), start.end());
        start.erase(ptr, start.end());
        sort(start.begin(), start.end(), [](const Implicant& a, const Implicant& b) {
            return a.countOnes()<b.countOnes();
        });
    }

    return prime;
}


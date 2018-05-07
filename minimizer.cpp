#include "minimizer.h"
#include <iterator>

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

vector<Implicant> getPrimeImplicants(const vector<Implicant>& fdnf, size_t n)
{
    vector<Implicant> start(fdnf);
    vector<Implicant> next;

    vector<Implicant> prime;

    while (true) {
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

set<int> make_union(const set<int>& x, const set<int>& y)
{
    set<int> xy;
    set_union(x.begin(), x.end(), y.begin(), y.end(), inserter(xy, xy.begin()));
    return xy;
}

set<set<int>> concatenation(const set<set<int>>& a, const set<set<int>>& b)
{
    set<set<int>> result;
    for (auto x:a) {
        for (auto y:b) {
            auto xy = make_union(x, y);
            set<set<int>> new_res(result);
            bool take_xy = true;

            for (auto i:result) {
                auto test = make_union(i, xy);
                if (test==xy)
                    take_xy = false;
                else if (test==i) {
                    new_res.erase(i);
                }
            }
            result.swap(new_res);
            if (take_xy)
                result.insert(xy);
        }
    }
    return result;
}

vector<vector<Implicant>> PetricksMethod(const vector<Implicant>& fdnf, const vector<Implicant>& prime)
{
    set<set<set<int>>> expr;

    for (const auto& i:fdnf) {
        set<set<int>> mult;
        int index = 0;
        for (auto p:prime) {
            if (p.covers(i))
                mult.insert({index});
            index++;
        }
        expr.insert(mult);
    }
    while (expr.size()>1) {
        auto a = *expr.begin();
        auto b = *next(expr.begin());
        expr.erase(expr.begin(), next(next(expr.begin())));

        auto c = concatenation(a, b);
        expr.insert(c);
    }
    vector<vector<Implicant>> final;

    for (const auto& x: *expr.begin()) {
        vector<Implicant> finalForm;
        for (auto val:x)
            finalForm.push_back(prime[val]);
        final.push_back(finalForm);
    }

    return final;
}

vector<vector<Implicant>> minimize(const TruthTable& t)
{
    auto fdnf = getFDNF(t);
    if (fdnf.empty())
        return vector<vector<Implicant>>(1);

    auto prime = getPrimeImplicants(fdnf, t.getVars().size());

    return PetricksMethod(fdnf, prime);
}

void prettyPrint(const vector<Implicant>& v, const set<string>& vars, ostream& o)
{
    if (v.empty())
        o << "0";

    for (int i = 0; i<v.size(); ++i) {
        int k = count_if(v[i].state.begin(), v[i].state.end(), [](int x) { return x==1 || x==0; });

        if (k==0)
            o << "1";
        else {
            int kk = 0;
            if (k>1)
                o << "(";

            auto var_name = vars.begin();

            for (int j = 0; j<v[i].state.size(); ++j) {
                int val = v[i].state[j];

                if (val==1) {
                    o << *var_name << ((kk!=k-1) ? " ^ " : "");
                    ++kk;
                }
                else if (val==0) {
                    o << "!" << *var_name << ((kk!=k-1) ? " ^ " : "");
                    ++kk;
                }
                ++var_name;
            }
            if (k>1)
                o << ")";
            if (i!=v.size()-1)
                o << " v ";
        }
    }

}

int totalLength(const vector<Implicant>& v)
{
    int sum = 0;
    for (const auto& x: v)
        sum += x.countOnes();
    return sum;
}
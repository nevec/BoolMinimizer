#ifndef KURSACH_MINIMIZER_H
#define KURSACH_MINIMIZER_H

#include <utility>
#include <vector>
#include <set>
#include <string>
#include <memory>
#include <algorithm>
#include <ostream>
#include "parser.h"

using namespace std;

class TruthTable {
private:
    set<string> vars;
    vector<bool> table;
public:
    TruthTable() = default;

    TruthTable(set<string> vars, vector<bool> table)
            :vars(move(vars)), table(move(table)) { }

    TruthTable(const unique_ptr<Node>& root)
    {
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
    }

    TruthTable(const string& s)
    {
        int n;
        for (n = 1; (1 << n)<s.size(); ++n);
        if ((1 << n)!=s.size())
            throw invalid_argument("wrong format");

        table.resize((size_t) (1U << n));

        for (int i = 0; i<n; ++i)
            vars.insert(string("x")+to_string(i+1));

        for (int i = 0; i<s.size(); ++i) {
            if (s[i]=='1')
                table[i] = true;
            else if (s[i]!='0')
                throw invalid_argument("wrong format");
        }
    }

    const set<string>& getVars() const
    {
        return vars;
    }

    const vector<bool>& getTable() const
    {
        return table;
    }
};

class Implicant {
private:
    vector<int> state;
public:
    Implicant(size_t s, size_t n)
            :state(n)
    {
        for (int i = 0; i<n; ++i)
            state[i] = (s & (1 << i))!=0;
    }

    bool covers(const Implicant& o) const
    {
        for (int i = 0; i<(int) state.size(); ++i)
            if ((state[i]!=2 && state[i]!=o.state[i]))
                return false;
        return true;
    }

    bool isComplementary(const Implicant& o) const
    {
        int k = 0;
        for (int i = 0; i<(int) state.size(); ++i) {
            if (state[i]+o.state[i]==1)
                k++;
            else if (state[i]!=o.state[i])
                return false;
        }

        return (k==1);
    }

    int countOnes() const { return count(state.begin(), state.end(), 1); }

    Implicant combine(const Implicant& o) const
    {
        Implicant result(*this);
        for (int i = 0; i<(int) state.size(); ++i)
            if (state[i]+o.state[i]==1)
                result.state[i] = 2;
        return result;
    }

    friend ostream& operator<<(ostream& os, const Implicant& implicant)
    {
        size_t n = implicant.state.size();
        os << " (";
        for (int i = n-1; i>=0; --i) {
            if (implicant.state[i]==0) {
                os << "!" << n-i << ((i!=0) ? " ^ " : "");
            }
            else if (implicant.state[i]==1)
                os << n-i << ((i!=0) ? " ^ " : "");
        }
        os << ") ";
        return os;
    }

    bool operator<(const Implicant& rhs) const
    {
        for (int i = 0; i<state.size(); ++i)
            if (state.at(i)<rhs.state.at(i))
                return true;
            else if (state.at(i)>rhs.state.at(i))
                return false;
        return false;
    }

    friend void prettyPrint(const vector<Implicant>& v, const set<string>& vars, ostream& o);
    bool operator==(const Implicant& rhs) const
    {
        return !(*this<rhs) && !(rhs<*this);
    }
};

void prettyPrint(const vector<Implicant>& v, const set<string>& vars, ostream& o);

vector<vector<Implicant>> minimize(const TruthTable& t);

int totalLength(const vector<Implicant>& v);

#endif //KURSACH_MINIMIZER_H

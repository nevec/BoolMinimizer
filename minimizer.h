#ifndef KURSACH_MINIMIZER_H
#define KURSACH_MINIMIZER_H

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
    TruthTable(const set<string>& vars, const vector<bool>& table)
            :vars(vars), table(table) { }

    TruthTable(const string& s, int n)
            :table(1 << n), vars()
    {
        for (int i = 0; i<n; ++i)
            vars.insert(string("x")+to_string(i+1));

        for (int i = 0; i<(1 << n); ++i) {
            if (s.at(i)=='1')
                table[i] = true;
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
    Implicant(int s, int n)
            :state(n)
    {
        for (int i = 0; i<n; ++i)
            state[i] = (s & (1 << i))!=0;
    }

    bool covers(const Implicant& o) const
    {
        for (int i = 0; i<(int) state.size(); ++i)
            if (state[i]==1 && o.state[i]!=1)
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

        for (int i = n-1; i>=0; --i) {
            if (implicant.state[i]==0) {
                os << "!" << n-i << " ";
            }
            else if (implicant.state[i]==1)
                os << n-i << " ";
        }
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

    bool operator==(const Implicant& rhs) const
    {
        return !(*this<rhs) && !(rhs<*this);
    }
};

TruthTable createTable(const unique_ptr<Node>& root);

vector<Implicant> getPrimeImplicants(const TruthTable& t);

#endif //KURSACH_MINIMIZER_H

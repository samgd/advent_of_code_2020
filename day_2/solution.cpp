#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

using std::ifstream;
using std::istream;
using std::string;

struct Policy {
    int lower;
    int upper;
    char c;

    friend istream &operator>>(istream &is, Policy &p)
    {
        char sep;

        is >> p.lower;

        is >> sep;
        if (sep != '-') {
            is.putback(sep);
            is.clear(std::istream::failbit);
            return is;
        }

        is >> p.upper;

        is >> p.c;

        return is;
    }
};

int main()
{
    ifstream data{"input.txt"};

    Policy policy;
    string s;
    int n_valid_p1 = 0;
    int n_valid_p2 = 0;

    while (data >> policy) {
        data.ignore(2);  // skip space and colon
        getline(data, s);

        // Part 1: ensure policy character count is within bounds
        auto result = std::count(s.begin(), s.end(), policy.c);
        if (policy.lower <= result && result <= policy.upper) {
            n_valid_p1++;
        }

        // Part 2: ensure exactly one character at either lower or upper
        // is the policy character
        if ((s[policy.lower - 1] == policy.c && s[policy.upper - 1] != policy.c) ||
            (s[policy.lower - 1] != policy.c && s[policy.upper - 1] == policy.c)) {
            n_valid_p2++;
        }
    }

    std::cout << n_valid_p1 << " " << n_valid_p2 << std::endl;
}

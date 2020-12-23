#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <numeric>
#include <string>
#include <unordered_map>

class Cups {
public:
    Cups(std::initializer_list<int> c)
        : cups{c}
    {
        current = cups.begin();
        if (cups.size() == 0) {
            throw std::invalid_argument{"must be at least 1 cup"};
        }
        for (auto it = cups.begin(); it != cups.end(); ++it) {
            locations[*it] = it;
        }
    }

    void move()
    {
        // extract three cups after current
        std::list<int> to_move;
        auto it = current;
        it++;
        for (int i = 1; i <= 3; i++) {
            if (it == cups.end()) {
                it = cups.begin();
            }
            to_move.push_back(*it);
            cups.erase(it++);

            // erase invalidates iterator; this is OK to update now as splice
            // used later to reinsert elements does not effect iterators
            auto last = --to_move.end();
            locations[*last] = last;
        }

        // compute and find destination
        int dest_val = *current - 1;
        if (dest_val == 0) {
            dest_val = cups.size() + 3;
        }
        while (std::find(to_move.begin(), to_move.end(), dest_val) != to_move.end()) {
            dest_val = dest_val - 1;
            if (dest_val == 0) {
                dest_val = cups.size() + 3;
            }
        };
        auto dest = locations[dest_val];

        // insert 3 elements after dest
        cups.splice(++dest, to_move);

        // update current
        current++;
        if (current == cups.end()) {
            current = cups.begin();
        }
    }

    std::string after_cup(int cup = 1) const
    {
        auto it = std::find(cups.begin(), cups.end(), cup);
        if (it == cups.end()) {
            return "";
        }
        std::string result;
        auto end_cycle = it++;
        while (it != end_cycle) {
            if (it == cups.end()) {
                it = cups.begin();
            }
            result += std::to_string(*it++);
        }
        return result;
    }

    std::list<int> after_n(int n, int cup = 1) const
    {
        std::list<int> result;
        std::list<int>::const_iterator pos = locations.at(cup);
        pos++;
        for (int i = 0; i < n; i++) {
            if (pos == cups.end()) {
                pos = cups.cbegin();
            }
            result.push_back(*pos++);
        }
        return result;
    }

    void append_to(int n)
    {
        for (int i = cups.size() + 1; i < n + 1; i++) {
            cups.push_back(i);
            locations[i] = --cups.end();
        }
    }

private:
    std::list<int> cups;
    std::unordered_map<int, std::list<int>::iterator> locations;
    std::list<int>::iterator current;
};

int main()
{
    // Part 1
    Cups cups({4, 6, 7, 5, 2, 8, 1, 9, 3});
    for (int i = 0; i < 100; i++) {
        cups.move();
    }
    std::cout << cups.after_cup(1) << std::endl;

    // Part 2
    Cups cups_2({4, 6, 7, 5, 2, 8, 1, 9, 3});
    cups_2.append_to(1'000'000);
    for (int i = 0; i < 10'000'000; i++) {
        cups_2.move();
    }
    auto after_1 = cups_2.after_n(2, 1);
    long long result = std::accumulate(
        after_1.begin(), after_1.end(), 1ll, std::multiplies<long long>()
    );
    std::cout << result << std::endl;
}

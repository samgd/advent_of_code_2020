/* Day 1: Report Repair.
 *
 * Part 1: O(n)
 * Part 2: Average case n^2
 *
 */
#include <fstream>
#include <iostream>
#include <optional>
#include <unordered_set>
#include <unordered_map>

using std::unordered_map;
using std::unordered_multiset;
using std::ifstream;
using std::optional;


// Returns the multiple of the two numbers that sum to target if they exist.
optional<int> mult_pair(const unordered_multiset<int> &vals, const int target)
{
    unordered_map<int, int> diffs;
    for (const auto val : vals) {
        diffs[val] = target - val;
    }

    // search through all diffs and see if any exists in the original set of
    // vals, if so the total sums to target
    for (const auto &d : diffs) {
        auto search = diffs.find(d.second);
        if (search != diffs.end()) {
            return d.first * d.second;
        }
    }

    return std::nullopt;
}


int main()
{
    // read integer values from file into multiset
    ifstream data{"input.txt"};
    unordered_multiset<int> vals;
    int val;
    while (data >> val) {
        vals.insert(val);
    }

    // Part 1: find two values that sum to TARGET and output their multiple
    const int TARGET = 2020;
    auto result = mult_pair(vals, TARGET);
    if (!result) {
        std::cout << "Failed to find two values that sum to "
                  << TARGET
                  << std::endl;
        return 1;
    }
    std::cout << *result << std::endl;

    // Part 2: find three values that sum to TARGET and output their multiple

    // copy vals so it can be modified during the loop without the iterator
    auto tmp_vals = vals;

    for (const auto v1 : vals) {
        // Reuse solution from Part 1: For each value, form a new set that does
        // not contain the value and a new target. Then we only need to find
        // two values that sum to this new target.
        //
        // This is an average case n^2 solution.
        auto v1_target = TARGET - v1;
        tmp_vals.extract(v1);
        auto result = mult_pair(tmp_vals, v1_target);
        if (result) {
            std::cout << v1 * *result << std::endl;
            return 0;
        }
        tmp_vals.insert(v1);
    }

    std::cout << "Failed to find three values that sum to "
              << TARGET
              << std::endl;
}

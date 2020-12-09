#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

using std::ifstream;
using std::string;
using std::unordered_multiset;
using std::vector;

// Returns a vector containing each integer on each line in file.
vector<int> read_data(string file)
{
    ifstream data{file};

    vector<int> values;
    int value;
    while (data >> value) {
        values.push_back(value);
    }
    return values;
}

// Returns true if target is the sum of two values in window_set.
bool is_sum(const unordered_multiset<int>& window_set, int target) {
    unordered_multiset<int> tmp_set{window_set};
    for (auto value : window_set) {
        auto other = target - value;
        auto n_other = window_set.count(other);
        if (
            (value != other && n_other >= 1) ||
            (value == other && n_other >= 2)
        ) {
            return true;
        }
    }
    return false;
}

// Returns the first value that is not the sum of any two of the previous window_size.
std::optional<int> first_sum_of_two(const vector<int>& values, const int window_size)
{
    vector<int>::size_type start = 0, end = 0;
    unordered_multiset<int> window;

    // fill window
    for (int i = 0; i < window_size; i++) {
        window.insert(values[end++]);
    }

    // move window, return when we find a value that isn't sum of two in window
    while (end != values.size()) {
        auto value = values[end];
        if (!is_sum(window, value)) {
            return value;
        }
        window.erase(values[start++]);
        window.insert(values[end++]);
    }
    return {};
}

// Returns the min and max of the contiguous array of values that sums to target.
std::pair<int, int> min_max_sum_to(const vector<int>& values, const int target)
{
    vector<int>::size_type start = 0, end = 0;
    int total = 0;
    while (true) {
        if (total == target) {
            break;
        }
        total += values[end++];
        while (start <= end && total > target) {
            total -= values[start];
            start++;
        }
    }
    auto min = *std::min_element(values.begin() + start, values.begin() + end);
    auto max = *std::max_element(values.begin() + start, values.begin() + end);
    return {min, max};
}

int main()
{
    vector<int> values = read_data("input.txt");

    std::optional<int> first_sum = first_sum_of_two(values, 25);
    if (!first_sum) {
        std::cout << "Part 1 failed" << std::endl;
        return 1;
    }
    std::cout << *first_sum << std::endl;

    auto min_max = min_max_sum_to(values, *first_sum);

    std::cout << std::get<0>(min_max) + std::get<1>(min_max) << std::endl;
}

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using std::vector;
using std::ifstream;
using std::unordered_map;

// Returns a map of <difference between two adapters: number of occurences>.
unordered_map<int, int> get_diff_count(const vector<int>& adapters)
{
    unordered_map<int, int> diff_count;

    int jolt = 0;
    for (auto adapter : adapters) {
        if (adapter - jolt > 3) {
            throw std::invalid_argument("");
        }
        diff_count[adapter - jolt]++;
        jolt = adapter;
    }
    jolt += 3; // built-in adapter rated 3 higher
    diff_count[3]++;

    return diff_count;
}

unsigned long iter_n_arrange(const vector<int>& adapters) {
    vector<unsigned long> counts(adapters.size());
    counts[0] = 1;
    for (vector<int>::size_type i = 0; i < adapters.size(); i++) {
        for (auto j = i + 1; j < adapters.size() && adapters[j] - adapters[i] <= 3; j++) {
            counts[j] += counts[i];
        }
    }
    return counts[adapters.size() - 1];
}

int main()
{
    ifstream data{"input.txt"};

    vector<int> adapters = {0};

    std::copy(
        std::istream_iterator<int>(data),
        std::istream_iterator<int>(),
        std::back_inserter(adapters)
    );

    std::sort(adapters.begin(), adapters.end());

    auto diff_count = get_diff_count(adapters);

    std::cout << diff_count[1] * diff_count[3] << std::endl;
    std::cout << iter_n_arrange(adapters) << std::endl;
}

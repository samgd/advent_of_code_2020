#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

using std::getline;
using std::ifstream;
using std::pair;
using std::string;
using std::vector;

int main()
{
    ifstream data{"input.txt"};

    // right X, down Y: {X, Y}
    vector<pair<int, int>> slopes = {{1, 1}, {3, 1}, {5, 1}, {7, 1}, {1, 2}};
    vector<unsigned int> n_trees(slopes.size());
    unsigned int line_no = 0;
    string line;

    while (getline(data, line)) {
        for (typename decltype(slopes)::size_type i = 0; i < slopes.size(); i++) {
            auto &slope = slopes[i];
            if (line_no % slope.second != 0) {
                // skip until moved down by Y
                continue;
            }
            // current pos is X times the number of downward steps, tree patten
            // repeats so take modulus
            auto pos = (slope.first * (line_no / slope.second)) % line.size();
            n_trees[i] += line[pos] == '#';
        }
        line_no++;
    }

    auto total = std::accumulate(
        n_trees.begin(), n_trees.end(), 1u, std::multiplies<unsigned int>()
    );
    std::cout << n_trees[1] << " " << total << std::endl;
}

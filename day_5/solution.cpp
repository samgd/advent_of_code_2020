#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using std::ifstream;
using std::sort;
using std::string;
using std::vector;

int main()
{
    ifstream data{"input.txt"};
    string line;
    vector<int> seat_ids;

    // Seat IDs are binary numbers where positions:
    //
    //     [0,  3): encode 0/1 as L/R
    //     [3, 10): encode 0/1 as F/B
    //
    // Parse as a binary number with this encoding.
    auto get_seat_id = [](const int seat_id, const char letter) {
        return (seat_id * 2) + (letter == 'B' || letter == 'R');
    };
    while (getline(data, line)) {
        seat_ids.push_back(accumulate(line.begin(), line.end(), 0, get_seat_id));
    }

    sort(seat_ids.begin(), seat_ids.end());

    // Part 1: Possible without sorting but we need that for Part 2 anyway.
    auto max_id = *(seat_ids.end() - 1);
    std::cout << max_id << std::endl;

    // Part 2: Iterate to find a distance of 2 between any two adjacent values
    // to indicate a -1/+1 seat ID difference with mine being the missing ID.
    int my_seat = -1;
    for (auto i = seat_ids.begin(); i != seat_ids.end() - 1; i++) {
        if ((*(i + 1) - *i) == 2) {
            my_seat = *i + 1;
            break;
        }
    }
    if (my_seat == -1) {
        std::cout << "Own seat not found" << std::endl;
        return 1;
    }
    std::cout << my_seat << std::endl;
}

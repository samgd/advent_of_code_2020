#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::vector;

vector<int> parse_input(string filepath)
{
    std::ifstream data{filepath};
    if (!data) {
        throw std::invalid_argument{"unable to open file"};
    }
    vector<int> numbers;
    int number;
    while (data >> number) {
        numbers.push_back(number);
        data.get();
    }
    if (!data.eof()) {
        throw std::invalid_argument{"failed to parse input"};
    }
    return numbers;
}

int number_at_turn(const vector<int>& starting_numbers, const int target_turn)
{
    // map of <number: turn last spoken>
    std::unordered_map<int, int> last_spoken;

    // fill with starting numbers
    int last_turn = 0;
    for (auto number : starting_numbers) {
        last_spoken[number] = ++last_turn;
    }

    // iterate until reached target_turn
    int last_number = starting_numbers.back();
    while (last_turn != target_turn) {
        if (last_spoken.find(last_number) == last_spoken.end()) {
            // new number
            last_spoken[last_number] = last_turn++;
            last_number = 0;
        } else {
            // seen before
            auto diff = last_turn - last_spoken.at(last_number);
            last_spoken[last_number] = last_turn++;
            last_number = diff;
        }
    }
    return last_number;
}

int main()
{
    const auto starting_numbers = parse_input("input.txt");

    // Part 1
    auto number = number_at_turn(starting_numbers, 2020);
    std::cout << number << std::endl;

    // Part 2
    number = number_at_turn(starting_numbers, 30'000'000);
    std::cout << number << std::endl;
}

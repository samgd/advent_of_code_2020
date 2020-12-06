#include <bitset>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using std::bitset;
using std::ifstream;
using std::string;

size_t char_id(char x)
{
    switch (x) {
    case 'a': return  0;
    case 'b': return  1;
    case 'c': return  2;
    case 'd': return  3;
    case 'e': return  4;
    case 'f': return  5;
    case 'g': return  6;
    case 'h': return  7;
    case 'i': return  8;
    case 'j': return  9;
    case 'k': return 10;
    case 'l': return 11;
    case 'm': return 12;
    case 'n': return 13;
    case 'o': return 14;
    case 'p': return 15;
    case 'q': return 16;
    case 'r': return 17;
    case 's': return 18;
    case 't': return 19;
    case 'u': return 20;
    case 'v': return 21;
    case 'w': return 22;
    case 'x': return 23;
    case 'y': return 24;
    case 'z': return 25;
    };
    throw std::invalid_argument{"unknown char"};
}

int main()
{
    ifstream data{"input.txt"};
    string line;

    // Use a bitset to store current state of answers for a group.
    //
    // For Part 1, group_p1[i] is set if _any_ person in the group answers
    // question i.  All bits in group_p1 start out false and are set to true as
    // each person's answers are read and recorded.
    //
    // For Part 2, group_p2[i] is set if _all_ people in the group answer
    // question i. All bits in group_p2 start out true and group_p2[i] is set
    // to false if the current person does not answer question i.
    //
    // group_p1.count() will return the number of questions to which anyone
    // answered yes.
    //
    // group_p2.count() will return the number of questions to which everyone
    // has answered yes.
    bitset<26> group_p1, group_p2;
    group_p2.set();

    bitset<26> person;         // bitset to read current person's answers into
    bool proc_group = false;   // true if a group is currently being processed

    int sum_counts_p1 = 0;     // sum of all counts for p1
    int sum_counts_p2 = 0;     // sum of all counts for p1

    while (getline(data, line)) {
        if (line == "") {
            // Blank line: add current group count and reset for new group.
            if (proc_group) {
                sum_counts_p1 += group_p1.count();
                group_p1.reset();

                sum_counts_p2 += group_p2.count();
                group_p2.set();
            }
            proc_group = false;
            continue;
        }
        proc_group = true;
        person.reset();
        for (char c : line) {
            person.set(char_id(c));
        }
        group_p1 |= person;
        group_p2 &= person;
    }
    if (proc_group) {
        sum_counts_p1 += group_p1.count();
        sum_counts_p2 += group_p2.count();
    }

    std::cout << sum_counts_p1 << " " << sum_counts_p2 << std::endl;
}

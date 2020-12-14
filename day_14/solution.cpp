#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::vector;

constexpr int ValueSize = 36;
using Value = std::bitset<ValueSize>;
using Memory = std::unordered_map<Value, Value>;

struct Instruction {
    enum class Type {
        MASK, STORE
    };

    Type type;

    // bitmasks for MASK type where ith entry is 1 if mask_Y[i] = Y
    Value mask_0;
    Value mask_1;
    Value mask_X;

    // address and value for STORE type
    Value address;
    Value value;
};

// Returns a vector of Instruction's from the file at filepath.
vector<Instruction> parse_input(string filepath)
{
    std::ifstream data{filepath};

    if (!data) {
        throw std::invalid_argument{"unable to open filepath"};
    }

    std::regex match{
        "(mask) = ([0X1]{36})|(mem)\\[(\\d+)\\] = (\\d+)"
    };
    std::smatch results;
    string line;
    vector<Instruction> instructions;
    while (getline(data, line)) {
        if (!std::regex_match(line, results, match)) {
            throw std::invalid_argument{"invalid input line"};
        }
        if (results[1].matched) {
            // MASK
            Instruction ins;
            ins.type = Instruction::Type::MASK;
            for (int i = 0; i < ValueSize; i++) {
                switch (*(results[2].first + i)) {
                case '0':
                    ins.mask_0[ValueSize - i - 1] = 1;
                    break;
                case '1':
                    ins.mask_1[ValueSize - i - 1] = 1;
                    break;
                case 'X':
                    ins.mask_X[ValueSize - i - 1] = 1;
                    break;
                }
            }
            instructions.push_back(ins);
        } else if (results[3].matched) {
            // STORE
            Instruction ins;
            ins.type = Instruction::Type::STORE;
            ins.address = std::stoi(results[4].str());
            ins.value = std::stoi(results[5].str());
            instructions.push_back(ins);
        }
    }

    return instructions;
}

// Returns a Memory after the instruction's have been simulated under Part 1.
Memory simulate_v1(vector<Instruction> instructions)
{
    Memory memory;

    Value mask_0;
    Value mask_1;

    for (Instruction &ins : instructions) {
        switch (ins.type) {
        case Instruction::Type::MASK:
            mask_0 = ins.mask_0;
            mask_1 = ins.mask_1;
            break;
        case Instruction::Type::STORE:
            auto value = ins.value;
            value &= ~mask_0;
            value |= mask_1;
            memory[ins.address] = value;
            break;
        }
    }

    return memory;
}

// Returns a Memory after the instruction's have been simulated under Part 2.
Memory simulate_v2(vector<Instruction> instructions)
{
    Memory memory;

    Value mask;
    Value mask_float;

    for (Instruction &ins : instructions) {
        switch (ins.type) {
        case Instruction::Type::MASK:
            mask = ins.mask_1;
            mask_float = ins.mask_X;
            break;
        case Instruction::Type::STORE:
            auto address = ins.address;
            address |= mask;

            Value address_float;
            // Iterate over n possible index combinations, where index n is in
            // [0, 2^mask_float.count()).
            //
            // For each index, get it's bitset representation (bits). For the
            // kth _set_ bit in mask_float, set the kth bit in address_float to
            // the kth bit in the bitset representation.
            //
            // e.g. (for 4 bit mask)
            //
            // mask_float = 0101 (0th and 2nd bits are floating)
            // address = 1000
            //
            // | i | bitset repr | effective mask | address_float |
            // | 0 |          00 |           0000 |          1000 |
            // | 1 |          01 |           0001 |          1001 |
            // | 2 |          10 |           0100 |          1100 |
            // | 3 |          11 |           0101 |          1101 |
            //
            for (unsigned long long i = 0; i < std::pow(2, mask_float.count()); i++) {
                Value bits = i;
                address_float = address;

                int bit_loc = 0;
                for (int j = 0; j < ValueSize; j++) {
                    if (mask_float[j]) {
                        address_float[j] = bits[bit_loc++];
                    }
                }
                memory[address_float] = ins.value;
            }
            break;
        }
    }

    return memory;
}

// Returns the sum of all values in memory.
unsigned long long sum_memory(Memory &memory)
{
    auto total = 0ull;
    for (auto &mentry : memory) {
        total += mentry.second.to_ullong();
    }
    return total;
}

int main()
{
    auto instructions = parse_input("input.txt");

    // Part 1
    auto memory_v1 = simulate_v1(instructions);
    std::cout << sum_memory(memory_v1) << std::endl;

    // Part 2
    auto memory_v2 = simulate_v2(instructions);
    std::cout << sum_memory(memory_v2) << std::endl;
}

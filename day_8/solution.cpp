#include <fstream>
#include <iostream>
#include <set>
#include <vector>

using std::istream;
using std::ostream;
using std::set;
using std::vector;

// An Instruction in the boot code.
struct Instruction {
    enum class OpCode {
        ACC, JMP, NOP
    };

    OpCode opcode;
    int arg;
};

istream& operator>>(istream& is, Instruction& ins)
{
    constexpr std::streamsize count = 6;
    char opcode_str[count];

    if (is.flags() & std::ios_base::skipws) {
        is >> std::ws;
    }

    is.get(opcode_str, count);

    if (opcode_str[3] != ' ' || !(opcode_str[4] == '-' || opcode_str[4] == '+')) {
        is.clear(std::ios::failbit);
    }

    if (opcode_str[0] == 'a' && opcode_str[1] == 'c' && opcode_str[2] == 'c') {
        ins.opcode = Instruction::OpCode::ACC;
    } else if (opcode_str[0] == 'j' && opcode_str[1] == 'm' && opcode_str[2] == 'p') {
        ins.opcode = Instruction::OpCode::JMP;
    } else if (opcode_str[0] == 'n' && opcode_str[1] == 'o' && opcode_str[2] == 'p') {
        ins.opcode = Instruction::OpCode::NOP;
    } else {
        is.clear(std::ios::failbit);
    }

    is >> ins.arg;

    if (opcode_str[4] == '-') {
        ins.arg *= -1;
    }

    return is;
}

ostream& operator<<(ostream& os, Instruction& ins)
{
    switch (ins.opcode) {
    case Instruction::OpCode::ACC:
        os << "acc ";
        break;
    case Instruction::OpCode::JMP:
        os << "jmp ";
        break;
    case Instruction::OpCode::NOP:
        os << "nop ";
        break;
    };

    if (ins.arg >= 0)
        os << '+';
    os << ins.arg;

    return os;
}

// Part 1: Returns accumulator up to point of first repeated instruction.
int accum_until_repeat(vector<Instruction>& instructions)
{
    set<int> executed;
    int pc = 0;
    int accum = 0;

    while (executed.find(pc) == executed.end()) {
        auto &ins = instructions[pc];
        executed.insert(pc);

        switch (ins.opcode) {
        case Instruction::OpCode::ACC:
            accum += ins.arg;
            pc += 1;
            break;
        case Instruction::OpCode::JMP:
            pc += ins.arg;
            break;
        case Instruction::OpCode::NOP:
            pc += 1;
            break;
        }
    }

    return accum;
}

// Returns a vector where the ith entry is the set of pc's that lead to pc = i.
//
// Complexity: O(n). Requires iterating through the instruction sequence once.
vector<set<int>> instructions_came_from(const vector<Instruction>& instructions)
{
    vector<set<int>> came_from(instructions.size() + 1);
    for (int pc = 0; pc < static_cast<int>(instructions.size()); pc++) {
        auto &ins = instructions[pc];
        switch (ins.opcode) {
        case Instruction::OpCode::ACC:
        case Instruction::OpCode::NOP:
            came_from[pc + 1].insert(pc);
            break;
        case Instruction::OpCode::JMP:
            came_from[pc + ins.arg].insert(pc);
            break;
        }
    }
    return came_from;
}

// Returns a vector where entry i is true if pc = i leads to termination.
//
// Complexity: O(n). After executing instruction i, the program counter always
// is set to the same next instruction (which may be +1 or +/-N if JMP).
// Therefore each instruction "points" to one other instruction or the implicit
// "end" instruction. These pointers form a graph.
//
// Perform a DFS backwards from the implicit "end" instruction, marking those
// nodes encountered as leading to termination.
//
// There are no cycles. Proof: assume there was a cycle, then the instructions
// in the cycle can only point to other instructions in the cycle. But this
// leads to a contraction as one of the instructions in the cycle must have
// pointed out of the cycle in order for the path to read the "end"
// instruction.
//
// As each instruction only points to one other and there are no cycles, each
// instruction is visited at most once when doing a DFS backwards through the
// graph from the implicit end instruction.
vector<bool> instructions_terminate(const vector<Instruction>& instructions)
{
    vector<set<int>> came_from = instructions_came_from(instructions);
    vector<bool> terminate(instructions.size(), false);

    vector<int> to_visit; // stack to keep record of nodes to visit during DFS

    // populate stack with instructions that point to end
    for (auto ins_idx : came_from[instructions.size()]) {
        to_visit.push_back(ins_idx);
    }

    // DFS
    while (to_visit.size() != 0) {
        auto pc = to_visit.back();
        to_visit.pop_back();

        terminate[pc] = true;
        for (auto ins_idx : came_from[pc]) {
            to_visit.push_back(ins_idx);
        }
    }

    return terminate;
}

// Returns the accumulated value for the corrected program upon termination.
//
// Complexity: O(n).
int accum_loop_fix(const vector<Instruction>& instructions)
{
    // Find those instructions that lead to termination.
    const vector<bool> terminates = instructions_terminate(instructions);

    // Iterate through program. Flip the current JMP or NOP instruction
    // if it would lead to termination. Ensure to only flip once else we might
    // introduce a new cycle!
    int pc = 0;
    int accum = 0;
    bool flipped = false;

    while (pc != static_cast<int>(instructions.size())) {
        auto &ins = instructions[pc];

        switch (ins.opcode) {
        case Instruction::OpCode::ACC:
            accum += ins.arg;
            pc += 1;
            break;
        case Instruction::OpCode::JMP:
            if (!flipped && terminates[pc + 1]) {
                // this is the instruction to flip!
                flipped = true;
                pc += 1;
            } else {
                pc += ins.arg;
            }
            break;
        case Instruction::OpCode::NOP:
            if (!flipped && terminates[pc + ins.arg]) {
                // this is the instruction to flip!
                flipped = true;
                pc += ins.arg;
            } else {
                pc += 1;
            }
            break;
        }
    }

    return accum;
}

int main()
{
    std::ifstream data{"input.txt"};
    Instruction ins;
    vector<Instruction> instructions;

    while (data >> ins) {
        instructions.push_back(ins);
    }

    std::cout << accum_until_repeat(instructions) << std::endl;
    std::cout << accum_loop_fix(instructions) << std::endl;
}

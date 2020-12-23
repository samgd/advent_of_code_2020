#include <fstream>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

using std::string;
using std::vector;

class Instruction {
public:
    enum class Type {
        ADD, MUL, OPEN, CLOSE, VALUE
    };

    Instruction(Type t, long long v = 0) : type{t}, value{v} { };

    Type type;
    long long value;

    friend std::ostream& operator<<(std::ostream& os, const Instruction &ins)
    {
        switch (ins.type) {
        case Type::ADD:
            os << '+';
            break;
        case Type::MUL:
            os << '*';
            break;
        case Type::OPEN:
            os << '(';
            break;
        case Type::CLOSE:
            os << ')';
            break;
        case Type::VALUE:
            os << ins.value;
            break;
        default:
            throw std::invalid_argument("unknown type");
        }
        return os;
    }
};

vector<Instruction> parse_input(std::istream& data)
{
    vector<Instruction> instructions;

    bool proc_value = false;
    long long value = 0;
    char c;

    while (true) {
        c = data.get();
        if (data.eof()) {
            break;
        }
        switch (c) {
        case '0': case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9':
            proc_value = true;
            value = 10*value + (c - '0');
            break;
        case ' ':
        case '\n':
            if (proc_value) {
                instructions.push_back(Instruction(Instruction::Type::VALUE, value));
                proc_value = false;
                value = 0.0;
            }
            break;
        case '(':
        case ')':
        case '*':
        case '+':
            if (proc_value) {
                instructions.push_back(Instruction(Instruction::Type::VALUE, value));
                proc_value = false;
                value = 0.0;
            }
            Instruction::Type type;
            if (c == '(') {
                type = Instruction::Type::OPEN;
            } else if (c == ')') {
                type = Instruction::Type::CLOSE;
            } else if (c == '*') {
                type = Instruction::Type::MUL;
            } else if (c == '+') {
                type = Instruction::Type::ADD;
            } else {
                throw std::invalid_argument{"unknown type"};
            }
            instructions.push_back(Instruction(type));
            break;
        default:
            string err = "unknown char: ";
            err += c;
            throw std::invalid_argument{err};
        }
    }
    if (proc_value) {
        instructions.push_back(Instruction(Instruction::Type::VALUE, value));
    }

    return instructions;
}

void process(
    std::stack<Instruction>& operand_stack,
    std::stack<Instruction>& operator_stack
) {
    long long a, b, c;

    a = operand_stack.top().value;
    operand_stack.pop();
    b = operand_stack.top().value;
    operand_stack.pop();

    Instruction op = operator_stack.top();
    operator_stack.pop();
    switch (op.type) {
    case Instruction::Type::MUL:
        c = a * b;
        break;
    case Instruction::Type::ADD:
        c = a + b;
        break;
    default:
        throw std::invalid_argument("invalid op");
    }

    operand_stack.push(Instruction(Instruction::Type::VALUE, c));
}

Instruction evaluate_part1(const vector<Instruction>& instructions)
{
    std::stack<Instruction> operand_stack;
    std::stack<Instruction> operator_stack;

    for (const auto& ins : instructions) {
        switch (ins.type) {
        case Instruction::Type::VALUE:
            operand_stack.push(ins);
            break;
        case Instruction::Type::ADD:
        case Instruction::Type::MUL:
            if (operator_stack.empty()) {
                operator_stack.push(ins);
            } else {
                while (
                    !operator_stack.empty() &&
                    operator_stack.top().type != Instruction::Type::OPEN
                ) {
                    process(operand_stack, operator_stack);
                }
                operator_stack.push(ins);
            }
            break;
        case Instruction::Type::OPEN:
            operator_stack.push(ins);
            break;
        case Instruction::Type::CLOSE:
            while (
                !operator_stack.empty() &&
                operator_stack.top().type != Instruction::Type::OPEN
            ) {
                process(operand_stack, operator_stack);
            }
            operator_stack.pop(); // remove open parenthesis
            break;
        }
    }

    while (!operator_stack.empty()) {
        process(operand_stack, operator_stack);
    }

    if (operand_stack.size() != 1) {
        string err = "operand stack has > 1 value";
        throw std::invalid_argument{err};
    }

    return operand_stack.top();
}

Instruction evaluate_part2(const vector<Instruction>& instructions)
{
    std::stack<Instruction> operand_stack;
    std::stack<Instruction> operator_stack;

    for (const auto& ins : instructions) {
        switch (ins.type) {
        case Instruction::Type::VALUE:
            operand_stack.push(ins);
            break;
        case Instruction::Type::ADD:
            if (
                operator_stack.empty() ||
                operator_stack.top().type == Instruction::Type::MUL
            ) {
                operator_stack.push(ins);
            } else {
                while (
                    !operator_stack.empty() &&
                    operator_stack.top().type != Instruction::Type::OPEN &&
                    operator_stack.top().type != Instruction::Type::MUL
                ) {
                    process(operand_stack, operator_stack);
                }
                operator_stack.push(ins);
            }
            break;
        case Instruction::Type::MUL:
            while (
                !operator_stack.empty() &&
                operator_stack.top().type != Instruction::Type::OPEN
            ) {
                process(operand_stack, operator_stack);
            }
            operator_stack.push(ins);
            break;
        case Instruction::Type::OPEN:
            operator_stack.push(ins);
            break;
        case Instruction::Type::CLOSE:
            while (
                !operator_stack.empty() &&
                operator_stack.top().type != Instruction::Type::OPEN
            ) {
                process(operand_stack, operator_stack);
            }
            operator_stack.pop(); // remove open parenthesis
            break;
        }
    }

    while (!operator_stack.empty()) {
        process(operand_stack, operator_stack);
    }

    return operand_stack.top();
}

long long process_homework(
    string filepath,
    Instruction (&evaluate)(const vector<Instruction>& instructions)
) {
    std::ifstream data{filepath};
    if (!data) {
        throw std::invalid_argument{"unable to open filepath"};
    }

    string line;
    long long total = 0;
    while (getline(data, line)) {
        std::stringstream ss{line};
        const auto instructions = parse_input(ss);
        total += evaluate(instructions).value;
    }

    return total;
}

int main()
{
    std::cout << process_homework("input.txt", evaluate_part1) << std::endl;
    std::cout << process_homework("input.txt", evaluate_part2) << std::endl;
}

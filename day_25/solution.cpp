#include <fstream>
#include <iostream>
#include <string>

long transform(long loop_size, long subject_number, long mod = 20201227)
{
    long long val = 1;
    for (int i = 0; i < loop_size; i++) {
        val *= subject_number;
        val %= mod;
    }
    return val;
}

long recover_loop_size(long public_key, long subject_number = 7, long mod = 20201227)
{
    long val = subject_number;
    long ls;
    for (ls = 1; val != public_key; ++ls) {
        val *= subject_number;
        val %= mod;
    }
    return ls;
}

std::pair<long, long> read_keys(std::string filepath)
{
    std::ifstream data{filepath};
    if (!data) {
        throw std::invalid_argument{"unable to open file"};
    }

    std::pair<long, long> keys;

    std::string line;
    getline(data, line);
    keys.first = std::stol(line);

    getline(data, line);
    keys.second = std::stol(line);

    return keys;
}

int main()
{
    auto keys = read_keys("input.txt");

    long card_pk = keys.first;
    long door_pk = keys.second;

    auto card_ls = recover_loop_size(card_pk);
    auto enc_key = transform(card_ls, door_pk);

    std::cout << enc_key << std::endl;
}

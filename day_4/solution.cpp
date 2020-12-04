#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <bitset>
#include <string>
#include <string_view>
#include <ios>
#include <stdexcept>

using std::bitset;
using std::noskipws;
using std::ifstream;
using std::invalid_argument;
using std::out_of_range;
using std::smatch;
using std::sregex_iterator;
using std::stringstream;
using std::stoi;
using std::string;

enum class FieldType : size_t {
    BYR, IYR, EYR, HGT, HCL, ECL, PID, CID
};

FieldType from_string(const string &s) {
    if (s == "byr") {
        return FieldType::BYR;
    } else if (s == "iyr") {
        return FieldType::IYR;
    } else if (s == "eyr") {
        return FieldType::EYR;
    } else if (s == "hgt") {
        return FieldType::HGT;
    } else if (s == "hcl") {
        return FieldType::HCL;
    } else if (s == "ecl") {
        return FieldType::ECL;
    } else if (s == "pid") {
        return FieldType::PID;
    } else if (s == "cid") {
        return FieldType::CID;
    }
    throw invalid_argument{"unknown FieldType " + s};
}

// Returns true when all fields are set that are required for a valid passport.
bool valid_passport(bitset<8> fields)
{
    return (
        fields.all() ||
        (fields.count() == 7 && !fields[static_cast<size_t>(FieldType::CID)])
    );
}

// Returns true if the integer year given in s is in [lower, upper].
bool valid_year(const string &s, int lower, int upper)
{
    try {
        int year = stoi(s);
        return lower <= year && year <= upper;
    } catch (invalid_argument) {
        return false;
    }
}

// Returns true if the value in s is valid for the given FieldType.
bool valid_value(FieldType ft, const string &s)
{
    switch (ft) {
    case FieldType::BYR:
        return valid_year(s, 1920, 2002);
    case FieldType::IYR:
        return valid_year(s, 2010, 2020);
    case FieldType::EYR:
        return valid_year(s, 2020, 2030);
    case FieldType::HGT:
        try {
            int height;
            string measure;
            stringstream ss{s};
            ss >> noskipws >> height >> measure ;
            return (
                ss &&
                (
                    (measure == "cm" && 150 <= height && height <= 193) ||
                    (measure == "in" && 59 <= height && height <= 76)
                )
            );
        } catch (invalid_argument) {
            return false;
        } catch (out_of_range) {
            return false;
        }
    case FieldType::HCL:
        {
            auto pred = [](char c) {
                return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f');
            };
            return (
                s.size() == 7 &&
                s[0] == '#' &&
                all_of(s.begin() + 1, s.end(), pred)
            );
        }
    case FieldType::ECL:
        return (
            s == "amb" || s == "blu" || s == "brn" || s == "gry" ||
            s == "grn" || s == "hzl" || s == "oth"
        );
    case FieldType::PID:
        return s.size() == 9 && all_of(s.begin(), s.end(), isdigit);
    case FieldType::CID:
        return true;
    }
    throw invalid_argument{"unknown FieldType"};
}

int main()
{
    ifstream data{"input.txt"};

    bitset<8> fields_p1, fields_p2;
    int n_valid_p1 = 0, n_valid_p2 = 0;

    string line;

    std::regex field_regex{
        "(byr|iyr|eyr|hgt|hcl|ecl|pid|cid):([^[:space:]]+)[[:space:]]*"
    };

    while (getline(data, line)) {
        if (line == "") {
            n_valid_p1 += valid_passport(fields_p1);
            fields_p1.reset();

            n_valid_p2 += valid_passport(fields_p2);
            fields_p2.reset();

            continue;
        }
        auto fields_begin = sregex_iterator(line.begin(), line.end(), field_regex);
        auto fields_end = std::sregex_iterator();
        for (std::sregex_iterator i = fields_begin; i != fields_end; i++) {
            smatch match = *i;
            FieldType ft = from_string(match[1]);
            auto bit = static_cast<size_t>(ft);

            // Part 1: no validation required.
            fields_p1.set(bit, true);

            // Part 2: validation required.
            fields_p2.set(bit, valid_value(ft, match[2]));
        }
    }
    n_valid_p1 += valid_passport(fields_p1);
    n_valid_p2 += valid_passport(fields_p2);

    std::cout << n_valid_p1 << " " << n_valid_p2 << std::endl;

}

#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "graph.h"

using std::string;
using std::unordered_map;
using std::unordered_set;
using std::unordered_multiset;

using Ingredient = string;
using Allergy = string;
using AllergyMap = unordered_map<Allergy, unordered_set<Ingredient>>;
using IngredientCount = unordered_multiset<Ingredient>;

std::pair<AllergyMap, IngredientCount> parse_input(string filepath)
{
    std::ifstream data{filepath};
    if (!data) {
        throw std::invalid_argument{"unable to open " + filepath};
    }

    AllergyMap allergy_map;
    IngredientCount ingredient_counts;

    std::regex pattern{
        "([[:alpha:]]+(?: [[:alpha:]]+)*)"
        " \\(contains ([[:alpha:]]+(?:, [[:alpha:]]+)*)\\)"
    };
    std::smatch matches;

    string line;
    while (getline(data, line)) {
        if (!std::regex_match(line, matches, pattern)) {
            throw std::invalid_argument{"invalid input: " + line};
        }
        if (!matches[2].matched) {
            // no allergy restrictions
            continue;
        }
        // parse ingredients into vector
        unordered_set<Ingredient> ingredients;
        std::stringstream items{matches[1]};
        string item;
        while (items >> item) {
            ingredients.insert(item);
            ingredient_counts.insert(item);
        }

        std::stringstream allergies{matches[2].str()};
        Allergy allergy;
        while (allergies >> allergy) {
            if (allergy.back() == ',') {
                allergy.pop_back();
            }

            if (allergy_map.find(allergy) == allergy_map.end()) {
                // new allergy, add all ingredients
                allergy_map[allergy] = ingredients;
                continue;
            }
            // allergy already added, intersect with current ingredients
            auto& allergy_set = allergy_map[allergy];

            for (auto it = allergy_set.begin(); it != allergy_set.end(); ) {
                if (ingredients.find(*it) == ingredients.end()) {
                    it = allergy_set.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    return {allergy_map, ingredient_counts};
}

int main()
{
    auto result = parse_input("input.txt");

    unordered_set<Ingredient> ingredients {
        result.second.begin(),
        result.second.end()
    };

    // Part 1
    unordered_set<Ingredient> allergy_free{ingredients};
    for (auto& entry : result.first) {
        for (auto& ingredient : entry.second) {
            allergy_free.erase(ingredient);
        }
    }

    int count = 0;
    for (auto& entry : allergy_free) {
        count += result.second.count(entry);
    }

    std::cout << count << std::endl;

    // Part 2
    // construct bipartite graph between ingredients and allergies

    // assign unique ID to all ingredients and allergies
    //
    // convenience maps to convert to/from int and string representations
    unordered_map<string, int> string_index;
    unordered_map<int, string> index_string;
    int n = 0;   // index of next vertex to add
    // assign all ingredients that might contain allergies an index
    for (auto& ingredient : ingredients) {
        if (allergy_free.find(ingredient) != allergy_free.end()) {
            continue;
        }
        string_index[ingredient] = n;
        index_string[n] = ingredient;
        n++;
    }
    int n_ingredients = string_index.size();
    // assign all allergies that link to ingredients an index
    for (auto& entry : result.first) {
        if (entry.second.size() == 0) {
            continue;
        }
        string_index[entry.first] = n;
        index_string[n] = entry.first;
        n++;
    }
    int n_allergy = string_index.size() - n_ingredients;

    // construct graph
    Graph g(n_ingredients, n_allergy);
    for (auto& entry : result.first) {
        for (auto& ingredient : entry.second) {
            g.add_edge(string_index[entry.first], string_index[ingredient]);
        }
    }
    // find matching
    auto matching = g.matching();

    // create vector of <allergy, ingredient>
    std::vector<std::pair<string, string>> matching_str;
    for (auto &edge : matching) {
        matching_str.push_back(
            {index_string[edge.second], index_string[edge.first]}
        );
    }

    // sort by allergy
    std::sort(matching_str.begin(), matching_str.end());

    // output
    for (auto it = matching_str.begin(); it + 1 != matching_str.end(); it++) {
        std::cout << it->second << ",";
    }
    std::cout << matching_str.back().second << std::endl;
}

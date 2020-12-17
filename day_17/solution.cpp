#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

template <unsigned int N>
struct Position {
    std::array<int, N> pos;

    friend bool operator==(const Position<N>& lhs, const Position<N>& rhs) {
        for (unsigned int i = 0; i < N; i++) {
            if (lhs.pos[i] != rhs.pos[i]) {
                return false;
            }
        }
        return true;
    }
};

template <unsigned int N>
using Grid = std::unordered_set<Position<N>>;

namespace std {

    template<unsigned int N>
    struct hash<Position<N>> {
        std::size_t operator()(const Position<N>& pos) const
        {
            std::size_t value = 0;
            for (int i = 0; i < N; i++) {
                value ^= std::hash<int>()(pos.pos[i]);
            }
            return value;
        }
    };

}

template <unsigned int N>
Grid<N> parse_input(std::string filepath)
{
    std::ifstream data{filepath};
    if (!data) {
        throw std::invalid_argument{"failed to open filepath"};
    }

    Grid<N> grid;
    Position<N> pos{};
    std::string line;

    while (getline(data, line)) {
        for (const char &c : line) {
            switch (c) {
            case '.':
                // inactive
                break;
            case '#':
                // active
                grid.insert(pos);
                break;
            }
            pos.pos[1] += 1;
        }
        pos.pos[1] = 0;
        pos.pos[0] += 1;
    }

    return grid;
}

template <unsigned int N>
Grid<N> cycle(const Grid<N>& grid)
{
    std::unordered_map<Position<N>, int> n_active_neighbors;

    // for each active cube, increment the active neighbor count of all
    // neighbor cubes
    for (const auto &pos : grid) {
        for (int i = 0; i < std::pow(3, N); i++) {
            Position<N> neighbor{pos.pos};
            int mod = i;
            for (unsigned int p = 0; p < N; p++, mod /= 3) {
                neighbor.pos[p] += (mod % 3) - 1;
            }
            if (neighbor == pos) {
                continue;
            }
            n_active_neighbors[neighbor]++;
        }
    }

    // create new grid containing all active cubes
    Grid<N> new_grid;
    for (auto &entry : n_active_neighbors) {
        if (grid.find(entry.first) != grid.end()) {
            // cube active, remains active if 2 or 3 active neighbors
            if (entry.second == 2 || entry.second == 3) {
                new_grid.insert(entry.first);
            }
        } else {
            // cube inactive, becomes active if 3 active neighbors
            if (entry.second == 3) {
                new_grid.insert(entry.first);
            }
        }
    }

    return new_grid;
}

int main()
{
    // Part 1
    Grid<3> grid3d = parse_input<3>("input.txt");
    for (int i = 0; i < 6; i++) {
        grid3d = cycle(grid3d);
    }
    std::cout << grid3d.size() << std::endl;

    // Part 2
    Grid<4> grid4d = parse_input<4>("input.txt");
    for (int i = 0; i < 6; i++) {
        grid4d = cycle(grid4d);
    }
    std::cout << grid4d.size() << std::endl;
}

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::string;
using std::unordered_set;
using std::vector;

// Axial (trapezoidal/oblique/skewed) coordinate system.
//
// The cube coordinate system takes a diagonal plane out of a 3D cube where the
// plane ensures the following equation holds
//
//     x + y + z = 0
//
// This provides a hexagonal coordinate system where the coordinates are
// 3-tuples.
//
// This equation means there is redundant information and only two of the three
// coordinates need storing. The axial coordinate system only stores two of
// them.
class Coordinate {
public:
    enum class Direction : unsigned char {
        E, W, SE, NW, NE, SW
    };

    Coordinate(int column = 0, int row = 0) : column{column}, row{row} { }

    int get_column() const { return column; }

    int get_row() const { return row; }

    void move(const Direction dir)
    {
        switch (dir) {
        case Direction::E:
            column += 1;
            break;
        case Direction::W:
            column -= 1;
            break;

        case Direction::SE:
            row += 1;
            break;
        case Direction::NW:
            row -= 1;
            break;

        case Direction::NE:
            column += 1;
            row -= 1;
            break;
        case Direction::SW:
            column -= 1;
            row += 1;
            break;
        };
    }

    bool operator==(const Coordinate &c) const
    {
        return column == c.column && row == c.row;
    }

private:
    int column;
    int row;
};

namespace std {
    template <>
    struct hash<Coordinate> {
        std::size_t operator()(const Coordinate& c) const
        {
            // implementation from boost::hash_combine
            //
            // cannot naively xor the hash of column and row as xor is
            // commutative
            auto seed = std::hash<int>()(c.get_column());
            seed ^= std::hash<int>()(c.get_row()) + 0x9e3779b9 + (seed<<6) + (seed>>2);
            return seed;
        }
    };
}

// Returns all directions in the given string - assumes input is valid.
vector<Coordinate::Direction> parse_directions(const string& directions)
{
    vector<Coordinate::Direction> result;

    for (auto it = directions.begin(); it != directions.end(); it++) {
        switch (*it) {
        case 'e':
            result.push_back(Coordinate::Direction::E);
            break;
        case 'w':
            result.push_back(Coordinate::Direction::W);
            break;
        case 's':
            if (*(++it) == 'e') {
                result.push_back(Coordinate::Direction::SE);
            } else {
                result.push_back(Coordinate::Direction::SW);
            }
            break;
        case 'n':
            if (*(++it) == 'e') {
                result.push_back(Coordinate::Direction::NE);
            } else {
                result.push_back(Coordinate::Direction::NW);
            }
            break;
        }
    }

    return result;
}

// Returns the coordinates of black tiles after executing all flips in filepath.
unordered_set<Coordinate> get_black_tiles(string filepath)
{
    std::ifstream data{filepath};
    if (!data) {
        throw std::invalid_argument{"unable to open " + filepath};
    }

    unordered_set<Coordinate> black_tiles;
    string line;
    while (getline(data, line)) {
        auto directions = parse_directions(line);

        Coordinate c;
        for (auto d : directions) {
            c.move(d);
        }

        auto black_tile = black_tiles.find(c);
        if (black_tile != black_tiles.end()) {
            // tile at coordinate is black, flip to white by removing
            black_tiles.erase(black_tile);
        } else {
            // tile at coordinate is white, flip to black by adding
            black_tiles.insert(c);
        }
    }

    return black_tiles;
}

// Updates the tiles according to the following rules:
//
// - Any black tile with zero or more than 2 black tiles immediately adjacent
//   to it is flipped to white.
// - Any white tile with exactly 2 black tiles immediately adjacent to it is
//   flipped to black.
unordered_set<Coordinate> update_tiles(unordered_set<Coordinate> black_tiles)
{
    static const vector<Coordinate::Direction> directions {
        Coordinate::Direction::E,
        Coordinate::Direction::W,
        Coordinate::Direction::SE,
        Coordinate::Direction::SW,
        Coordinate::Direction::NE,
        Coordinate::Direction::NW
    };

    // for each current black tile, increment each neighbors adjacent count by 1
    std::unordered_map<Coordinate, int> n_adj_black;
    for (const auto& c : black_tiles) {
        for (auto dir : directions) {
            auto neighbor = c;
            neighbor.move(dir);
            n_adj_black[neighbor]++;
        }
    }

    // for each tile with black neighbors, check what color it should be
    unordered_set<Coordinate> new_tiles;
    for (const auto& entry : n_adj_black) {
        if (black_tiles.find(entry.first) != black_tiles.end()) {
            // currently black
            if (!(entry.second == 0 || entry.second > 2)) {
                new_tiles.insert(entry.first);
            }
        } else {
            // currently white
            if (entry.second == 2) {
                new_tiles.insert(entry.first);
            }
        }
    }

    return new_tiles;
}

int main()
{
    // Part 1
    auto black_tiles = get_black_tiles("input.txt");
    std::cout << black_tiles.size() << std::endl;

    // Part 2
    const int N_DAYS = 100;
    for (int day = 0; day < N_DAYS; day++) {
        black_tiles = update_tiles(black_tiles);
    }
    std::cout << black_tiles.size() << std::endl;
}

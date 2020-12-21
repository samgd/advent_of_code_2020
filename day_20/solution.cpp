#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "tile.h"

using std::istream;
using std::ostream;
using std::string;
using std::vector;

// Returns a vector of Tile's from the file at filepath.
vector<Tile> parse(string filepath)
{
    std::ifstream data{filepath};
    if (!data) {
        std::invalid_argument{"unable to open filepath"};
    }

    vector<Tile> tiles;

    Tile t;
    while (data >> t) {
        tiles.push_back(t);
    }
    if (data.eof()) {
        tiles.push_back(t);
    }

    return tiles;
}

// Returns true if the assignment at poisition is valid on the left and top.
bool valid_assignment(
    int position,
    const vector<Tile>& assignments
) {
    const Tile& tile = assignments.at(position);

    int square_size = std::sqrt(assignments.size());

    if (position >= square_size) {
        // check above
        const Tile& above = assignments.at(position - square_size);
        for (int j = 0; j < tile.n_cols; j++) {
            if (above.last_on_col(j) != tile.first_on_col(j)) {
                return false;
            }
        }
    }

    if (position % square_size != 0) {
        // check left
        const Tile& left = assignments.at(position - 1);
        for (int i = 0; i < tile.n_rows; i++) {
            if (left.last_on_row(i) != tile.first_on_row(i)) {
                return false;
            }
        }
    }

    return true;
}

// Returns true if able to fill assignments with tiles starting from position.
//
// For a 3x3 square, the assignment/position indices are:
//
//     0 1 2
//     3 4 5
//     6 7 8
//
bool fill(int position, const vector<Tile>& tiles, vector<Tile>& assignments)
{
    for (Tile tile : tiles) {
        bool skip_tile = false;
        for (int i = 0; i < position; i++) {
            if (assignments[i].id == tile.id) {
                // tile already assigned
                skip_tile = true;
                break;
            }
        }
        if (skip_tile) {
            continue;
        }
        for (int orient = 0; orient < 4; orient++) {
            for (int f = 0; f < 2; f++) {
                assignments.at(position) = tile;
                if (valid_assignment(position, assignments)) {
                    if (position == static_cast<int>(assignments.size() - 1) || fill(position + 1, tiles, assignments)) {
                        return true;
                    }
                }
                tile.horizontal_flip();
            }
            tile.rotate();
        }
    }
    return false;
}

// Assemble tiles into an image.
vector<Tile> assemble(const vector<Tile>& tiles)
{
    unsigned int square_size = std::sqrt(tiles.size());
    if (square_size*square_size != tiles.size()) {
        throw std::invalid_argument{"number of tiles invalid"};
    }
    vector<Tile> assignments(tiles.size());

    if (!fill(0, tiles, assignments)) {
        throw std::invalid_argument{"failed to assemble"};
    }

    return assignments;
}

// Returns the multiple of the four corner tile IDs.
long long corner_mult(const vector<Tile>& assignments)
{
    unsigned int square_size = std::sqrt(assignments.size());


    long long result = 1;
    result *= assignments[0].id;
    result *= assignments[square_size-1].id;
    result *= assignments[square_size*(square_size - 1)].id;
    result *= assignments[square_size*square_size - 1].id;

    return result;
}

// Returns the count of the number of '#' not included in a monster.
int count_not_pattern(const Tile& image, const Tile& pattern)
{
    int n = std::accumulate(image.pixels.begin(), image.pixels.end(), 0);

    std::set<std::pair<int, int>> in_monster;

    for (int i = 0; i < image.n_rows - pattern.n_rows + 1; i++) {
        for (int j = 0; j < image.n_cols - pattern.n_cols + 1; j++) {
            bool failed = false;
            std::set<std::pair<int, int>> cur_monster;
            for (int x = 0; x < pattern.n_rows; x++) {
                for (int y = 0; y < pattern.n_cols; y++) {
                    bool pat_pixel = pattern.pixels.at(x*pattern.n_cols + y);
                    bool img_pixel = image.pixels.at((i+x)*image.n_cols + j + y);
                    if (pat_pixel && img_pixel) {
                        cur_monster.insert({i + x, j + y});
                    }
                    if (pat_pixel && img_pixel != pat_pixel) {
                        failed = true;
                        break;
                    }
                }
                if (failed) {
                    break;
                }
            }
            if (!failed) {
                for (auto& v : cur_monster) {
                    in_monster.insert(v);
                }
            }
        }
    }
    return n - in_monster.size();
}

int main()
{
    // Part 1
    vector<Tile> tiles = parse("input.txt");
    auto assignment = assemble(tiles);
    std::cout << corner_mult(assignment) << std::endl;

    // Part 2
    Tile image(assignment);

    const string pattern_str = (
        "Tile 0:\n"
        "..................#.\n"
        "#....##....##....###\n"
        ".#..#..#..#..#..#...\n"
        "\n"
    );
    std::stringstream ss{pattern_str};
    Tile pattern;
    ss >> pattern;

    // find correct orientation, rotating pattern is equivalent to rotating the
    // image
    bool found = false;
    for (int o = 0; o < 4; o++) {
        for (int f = 0; f < 2; f++) {
            found = image.contains(pattern);
            if (found) {
                break;
            }
            pattern.horizontal_flip();
        }
        if (found) {
            break;
        }
        pattern.rotate();
    }
    if (!found) {
        throw std::invalid_argument{"unable to find pattern"};
    }

    std::cout << count_not_pattern(image, pattern) << std::endl;
}

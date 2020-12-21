#include <cmath>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "tile.h"

Tile::Tile(const std::vector<Tile>& assignment, int id)
    : id{id}, orientation{N}, flip{LEFT}
{
    int square_size = std::sqrt(assignment.size());
    if (square_size*square_size != static_cast<int>(assignment.size())) {
        throw std::invalid_argument{"number of tiles invalid"};
    }

    int per_row = assignment[0].n_rows - 2;
    int per_col = assignment[0].n_cols - 2;

    n_rows = square_size*per_row;
    n_cols = square_size*per_col;

    pixels.resize(n_rows*n_cols);

    for (int tile_i = 0; tile_i < square_size; tile_i++) {
        for (int tile_j = 0; tile_j < square_size; tile_j++) {
            const Tile& t = assignment[tile_i * square_size + tile_j];

            for (int i = 0; i < per_row; i++) {
                for (int j = 0; j < per_col; j++) {
                    pixels[(tile_i*per_row+i)*n_cols + tile_j*per_col + j] = t.pixels[(i+1)*t.n_cols + j + 1];
                }
            }
        }
    }
}

void Tile::horizontal_flip()
{
    std::vector<bool> flipped(pixels.size());

    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_cols; j++) {
            flipped[(i + 1)*n_cols - 1 - j] = pixels[i*n_cols + j];
        }
    }

    pixels = flipped;

    switch (flip) {
    case LEFT:
        flip = RIGHT;
        break;
    case RIGHT:
        flip = LEFT;
        break;
    };

}

void Tile::rotate()
{
    std::vector<bool> rotated(pixels.size());

    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_cols; j++) {
            rotated.at(j*n_rows + (n_rows - 1 - i)) = pixels[i*n_cols + j];
        }
    }

    std::swap(n_rows, n_cols);
    pixels = rotated;

    switch (orientation) {
    case N:
        orientation = E;
        break;
    case E:
        orientation = S;
        break;
    case S:
        orientation = W;
        break;
    case W:
        orientation = N;
        break;
    };
}

bool Tile::last_on_row(int i) const
{
    return pixels[(i + 1)*n_cols - 1];
}

bool Tile::first_on_row(int i) const
{
    return pixels[i*n_cols];
}

bool Tile::first_on_col(int j) const
{
    return pixels[j];
}

bool Tile::last_on_col(int j) const
{
    return pixels[pixels.size() - n_cols + j];
}

bool Tile::contains(const Tile& pattern)
{
    for (int i = 0; i < n_rows - pattern.n_rows + 1; i++) {
        for (int j = 0; j < n_cols - pattern.n_cols + 1; j++) {
            // check if pattern matches
            bool failed = false;
            for (int x = 0; x < pattern.n_rows; x++) {
                for (int y = 0; y < pattern.n_cols; y++) {
                    bool pixel = pattern.pixels.at(x*pattern.n_cols + y);
                    if (pixel && pixels.at((i+x)*n_cols + j + y) != pixel) {
                        failed = true;
                        break;
                    }
                }
                if (failed) {
                    break;
                }
            }
            if (!failed) {
                return true;
            }
        }
    }
    return false;
}

std::istream& operator>>(std::istream& is, Tile& tile)
{
    std::string line;
    getline(is, line);

    std::regex pattern("Tile ([[:digit:]]+):");
    std::smatch matches;
    if (!std::regex_match(line, matches, pattern)) {
        is.clear(std::ios::failbit);
        return is;
    }

    tile.id = std::stoi(matches[1].str());
    tile.pixels.clear();
    tile.n_rows = 0;
    tile.n_cols = 0;
    tile.flip = Tile::LEFT;
    tile.orientation = Tile::N;

    std::vector<bool> pixels;
    while (getline(is, line)) {
        if (line.size() <= 1) {
            break;
        }

        for (int i = 0; i < static_cast<int>(line.size()); i++) {
            pixels.push_back(line[i] == '#');
        }
        tile.n_rows += 1;
        tile.n_cols = line.size();
    }
    tile.pixels = pixels;

    return is;
}

std::ostream& operator<<(std::ostream& os, const Tile& tile)
{
    os << "Tile " << tile.id << ":\n";

    for (int i = 0; i < tile.n_rows; i++) {
        for (int j = 0; j < tile.n_cols; j++) {
            os << (tile.pixels[i*tile.n_cols + j] ? '#' : '.');
        }
        os << '\n';
    }

    return os;
}

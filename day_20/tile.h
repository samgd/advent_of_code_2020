#pragma once

#include <iostream>
#include <vector>

class Tile {
public:
    enum Orientation {
        N, E, S, W
    };

    enum Flip {
        LEFT, RIGHT
    };

    Tile() {};

    explicit Tile(const std::vector<Tile>& assignment, int id = 0);

    void horizontal_flip();

    void rotate();

    bool last_on_row(int i) const;

    bool first_on_row(int i) const;

    bool first_on_col(int j) const;

    bool last_on_col(int j) const;

    bool contains(const Tile& pattern);

    int id;
    Orientation orientation;
    Flip flip;
    std::vector<bool> pixels;
    int n_rows;
    int n_cols;
};

std::istream& operator>>(std::istream& is, Tile& tile);

std::ostream& operator<<(std::ostream& os, const Tile& tile);

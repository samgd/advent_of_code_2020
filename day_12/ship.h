#pragma once

#include <iostream>

class Instruction {
public:
    enum class Action {
        N, S, E, W, L, R, F
    };

    using Value = int;

    Action action;
    Value value;
};

std::istream& operator>>(std::istream& is, Instruction& ins);
std::ostream& operator<<(std::ostream& os, const Instruction& ins);

class Ship {
public:
    enum class Direction {
        N, S, E, W
    };

    virtual void move(const Instruction& ins) = 0;

    virtual int get_latitude() const = 0;

    virtual int get_longitude() const = 0;
};

std::ostream& operator<<(std::ostream&, const Ship::Direction&);

class AbsoluteShip : public Ship {
public:
    AbsoluteShip() : direction{Direction::E}, latitude{0}, longitude{0} { };

    void move(const Instruction& ins);

    int get_latitude() const { return latitude; }

    int get_longitude() const { return longitude; }
private:
    Direction direction;
    int latitude;
    int longitude;

    void turn(const int clockwise);
};

class WaypointShip : public Ship {
public:
    WaypointShip()
        : latitude{0}, longitude{0}, waypoint_lat{10}, waypoint_long{1} { };

    void move(const Instruction& ins);

    int get_latitude() const { return latitude; }

    int get_longitude() const { return longitude; }
private:
    int latitude;
    int longitude;

    int waypoint_lat;
    int waypoint_long;

    void turn(const int clockwise);
};

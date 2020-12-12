#include "ship.h"

#include <iostream>
#include <stdexcept>

std::istream& operator>>(std::istream& is, Instruction& ins)
{
    Instruction::Action act;
    Instruction::Value val;

    char a;
    is >> a;
    switch (a) {
    case 'N':
        act = Instruction::Action::N;
        break;
    case 'S':
        act = Instruction::Action::S;
        break;
    case 'E':
        act = Instruction::Action::E;
        break;
    case 'W':
        act = Instruction::Action::W;
        break;
    case 'L':
        act = Instruction::Action::L;
        break;
    case 'R':
        act = Instruction::Action::R;
        break;
    case 'F':
        act = Instruction::Action::F;
        break;
    default:
        is.unget();
        is.clear(std::ios::failbit);
        return is;
    };

    is >> val;
    if (is.fail()) {
        return is;
    }

    ins.action = act;
    ins.value = val;

    return is;
}

std::ostream& operator<<(std::ostream& os, const Instruction& ins)
{
    switch (ins.action) {
    case Instruction::Action::N:
        os << 'N';
        break;
    case Instruction::Action::S:
        os << 'S';
        break;
    case Instruction::Action::E:
        os << 'E';
        break;
    case Instruction::Action::W:
        os << 'W';
        break;
    case Instruction::Action::L:
        os << 'L';
        break;
    case Instruction::Action::R:
        os << 'R';
        break;
    case Instruction::Action::F:
        os << 'F';
        break;
    };

    os << ins.value;

    return os;
}

std::ostream& operator<<(std::ostream& os, const Ship::Direction& dir)
{
    switch (dir) {
    case Ship::Direction::N:
        os << 'N';
        break;
    case Ship::Direction::S:
        os << 'S';
        break;
    case Ship::Direction::E:
        os << 'E';
        break;
    case Ship::Direction::W:
        os << 'W';
        break;
    };
    return os;
}

void AbsoluteShip::move(const Instruction& ins)
{
    switch (ins.action) {
    case Instruction::Action::N:
        longitude += ins.value;
        break;
    case Instruction::Action::S:
        longitude -= ins.value;
        break;
    case Instruction::Action::E:
        latitude += ins.value;
        break;
    case Instruction::Action::W:
        latitude -= ins.value;
        break;
    case Instruction::Action::L:
        turn(-ins.value);
        break;
    case Instruction::Action::R:
        turn(ins.value);
        break;
    case Instruction::Action::F:
        switch (direction) {
        case Direction::N:
            longitude += ins.value;
            break;
        case Direction::S:
            longitude -= ins.value;
            break;
        case Direction::E:
            latitude += ins.value;
            break;
        case Direction::W:
            latitude -= ins.value;
            break;
        };
        break;
    }
}

void AbsoluteShip::turn(const int clockwise)
{
    if (clockwise % 90 != 0) {
        throw std::invalid_argument{"turns must be 90 degrees"};
    }
    auto dir = clockwise % 360;
    if (dir < 0) {
        dir += 360;
    }
    for (auto n_rots = dir / 90; n_rots > 0; n_rots--) {
        switch (direction) {
        case Direction::N:
            direction = Direction::E;
            break;
        case Direction::S:
            direction = Direction::W;
            break;
        case Direction::E:
            direction = Direction::S;
            break;
        case Direction::W:
            direction = Direction::N;
            break;
        }
    }
}

void WaypointShip::move(const Instruction& ins)
{
    switch (ins.action) {
    case Instruction::Action::N:
        waypoint_long += ins.value;
        break;
    case Instruction::Action::S:
        waypoint_long -= ins.value;
        break;
    case Instruction::Action::E:
        waypoint_lat += ins.value;
        break;
    case Instruction::Action::W:
        waypoint_lat -= ins.value;
        break;
    case Instruction::Action::L:
        turn(-ins.value);
        break;
    case Instruction::Action::R:
        turn(ins.value);
        break;
    case Instruction::Action::F:
        longitude += ins.value * waypoint_long;
        latitude += ins.value * waypoint_lat;
        break;
    }
}

void WaypointShip::turn(const int clockwise)
{
    if (clockwise % 90 != 0) {
        throw std::invalid_argument{"turns must be 90 degrees"};
    }
    auto dir = clockwise % 360;
    if (dir < 0) {
        dir += 360;
    }
    for (auto n_rots = dir / 90; n_rots > 0; --n_rots) {
        auto tmp_long = waypoint_long;
        waypoint_long = -waypoint_lat;
        waypoint_lat = tmp_long;
    }
}

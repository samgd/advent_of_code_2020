#include <fstream>

#include "ship.h"

int main()
{
    std::ifstream data{"input.txt"};

    Instruction ins;
    AbsoluteShip as;
    WaypointShip ws;

    while (data >> ins) {
        as.move(ins);
        ws.move(ins);
    }

    // Part 1
    auto manhattan_dist = std::abs(as.get_latitude()) + std::abs(as.get_longitude());
    std::cout << manhattan_dist << std::endl;

    // Part 2
    manhattan_dist = std::abs(ws.get_latitude()) + std::abs(ws.get_longitude());
    std::cout << manhattan_dist << std::endl;
}

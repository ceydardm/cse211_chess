#ifndef POSITION_HPP
#define POSITION_HPP

#include <functional> 
#include <cstddef>   

struct Position {
    int x;
    int y;

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};


namespace std {
    template <>
    struct hash<Position> {
        std::size_t operator()(const Position& pos) const {
            return (std::hash<int>()(pos.x) << 1) ^ std::hash<int>()(pos.y);
        }
    };
}

#endif

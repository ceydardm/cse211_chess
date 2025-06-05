#pragma once

#include "Board.hpp"
#include <unordered_map>
#include <vector>
#include <queue>

class MoveValidator {
public:
    MoveValidator(const Board& board);
    bool isPathValid(const Position& from, const Position& to);

private:
    void buildGraph();
    bool bfs(const Position& start, const Position& target);

    const Board& board;
    std::unordered_map<std::string, std::vector<Position>> adjacencyList;

    std::string posToKey(const Position& pos) const;
};

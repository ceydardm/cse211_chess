#include "MoveValidator.hpp"
#include <cmath>

MoveValidator::MoveValidator(const Board& board) : board(board) {
    buildGraph();
}

std::string MoveValidator::posToKey(const Position& pos) const {
    return std::to_string(pos.x) + "," + std::to_string(pos.y);
}

void MoveValidator::buildGraph() {
    int size = board.board_size;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            Position current = {x, y};
            std::string key = posToKey(current);
            std::vector<Position> neighbors;

            // Komşuları ekle
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    Position neighbor = {x + dx, y + dy};
                    if (board.isPositionValid(neighbor) &&
                        (!board.hasPieceAt(board.getKey(neighbor.x, neighbor.y)) || neighbor.x == x && neighbor.y == y)) {
                        neighbors.push_back(neighbor);
                    }
                }
            }

            adjacencyList[key] = neighbors;
        }
    }
}

bool MoveValidator::isPathValid(const Position& from, const Position& to) {
    return bfs(from, to);
}

bool MoveValidator::bfs(const Position& start, const Position& target) {
    std::queue<Position> q;
    std::unordered_map<std::string, bool> visited;

    q.push(start);
    visited[posToKey(start)] = true;

    while (!q.empty()) {
        Position current = q.front();
        q.pop();

        if (current.x == target.x && current.y == target.y)
            return true;

        for (const auto& neighbor : adjacencyList[posToKey(current)]) {
            std::string key = posToKey(neighbor);
            if (!visited[key]) {
                visited[key] = true;
                q.push(neighbor);
            }
        }
    }

    return false;
}

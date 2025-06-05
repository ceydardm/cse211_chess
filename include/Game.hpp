#ifndef GAME_HPP
#define GAME_HPP

#include "Board.hpp"
#include "ConfigReader.hpp"
#include "Rules.hpp"

#include <string>
#include <stack>
#include <queue>
#include <vector>
#include <memory>

struct MoveRecord {
    int x1, y1, x2, y2;
    std::unique_ptr<PieceConfig> capturedPiece;
};

class Game {
public:
    Game(const GameConfig& config);
    void start();

private:
    Board board;
    GameConfig config;
    bool isWhiteTurn;
    int turnCount;

    std::stack<MoveRecord> moveHistory;
    std::queue<std::string> cooldownQueue;

    bool processMove(const std::string& input);
    bool parseInput(const std::string& input, int& x1, int& y1, int& x2, int& y2) const;
    bool checkEndGame() const;

    void recordMove(int x1, int y1, int x2, int y2, std::unique_ptr<PieceConfig> captured);
    void handleCooldowns();
};

#endif

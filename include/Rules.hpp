#ifndef RULES_HPP
#define RULES_HPP

#include "Board.hpp"
#include "ConfigReader.hpp"

#include <vector>
#include <unordered_map>

class Rules {
public:
    static bool isCheck(const Board& board, bool isWhiteTurn);
    static bool isCheckmate(const Board& board, bool isWhiteTurn);
    static bool isValidMove(const Board& board, int x1, int y1, int x2, int y2);

    static bool canCastle(const Board& board, int kingX, int kingY, bool isLeft);
    static bool isMoveBlocked(const Board& board, int x1, int y1, int x2, int y2);
    static bool violatesCustomRule(const Board& board, const PieceConfig& piece, int x2, int y2);

    static std::vector<std::pair<int, int>> getReachablePositions(const Board& board, int x, int y);
    static bool isKingSurrounded(const Board& board, bool isWhiteTurn);

private:
    static bool isPathClear(const Board& board, int x1, int y1, int x2, int y2);
    static bool threatensKing(const Board& board, const PieceConfig& attacker, int x, int y);
};

#endif 

#include "ConfigReader.hpp"
#include "PieceHelper.hpp"

char PieceHelper::getTypeSymbol(const PieceConfig& piece) {
    return piece.type.empty() ? '.' : piece.type[0];
}

bool PieceHelper::isWhiteAt(const PieceConfig& piece, const Position& pos) {
    for (const auto& side : piece.positions) {
        for (const auto& p : side.second) {
            if (p.x == pos.x && p.y == pos.y) {
                return side.first == "white";
            }
        }
    }
    return false;
}

std::string PieceHelper::positionToKey(const Position& pos) {
    return std::to_string(pos.x) + "," + std::to_string(pos.y);
}

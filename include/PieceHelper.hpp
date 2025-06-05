#ifndef PIECE_HELPER_HPP
#define PIECE_HELPER_HPP

#include "ConfigReader.hpp"

#include <string>

class PieceHelper {
public:
    static char getTypeSymbol(const PieceConfig& piece);

    static bool isWhiteAt(const PieceConfig& piece, const Position& pos);

    static std::string positionToKey(const Position& pos);
};

#endif

#include "Rules.hpp"
#include "Board.hpp"
#include "ConfigReader.hpp"
#include <iostream>
#include <cmath>

bool Rules::isCheck(const Board& board, bool isWhiteTurn) {
    Position kingPosition = { -1, -1 };

    for (int y = 0; y < board.board_size; ++y) {
        for (int x = 0; x < board.board_size; ++x) {
            auto piece = board.getPiece(board.getKey(x, y));
            if (piece && piece->type == "King" && piece->getIsWhite() == isWhiteTurn) {
                kingPosition = { x, y };
                break;
            }
        }
    }

    if (kingPosition.x == -1)
        return false;

    for (int y = 0; y < board.board_size; ++y) {
        for (int x = 0; x < board.board_size; ++x) {
            auto piece = board.getPiece(board.getKey(x, y));
            if (piece && piece->getIsWhite() != isWhiteTurn) {
                if (isValidMove(board, x, y, kingPosition.x, kingPosition.y)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Rules::isCheckmate(const Board& board, bool isWhiteTurn) {
    if (!isCheck(board, isWhiteTurn))
        return false;

    for (int y = 0; y < board.board_size; ++y) {
        for (int x = 0; x < board.board_size; ++x) {
            auto piece = board.getPiece(board.getKey(x, y));
            if (piece && piece->getIsWhite() == isWhiteTurn) {
                for (int targetY = 0; targetY < board.board_size; ++targetY) {
                    for (int targetX = 0; targetX < board.board_size; ++targetX) {
                        Board testBoard = board.clone();
                        if (testBoard.movePieceForCloneBoard(x, y, targetX, targetY) &&
                            !isCheck(testBoard, isWhiteTurn)) {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool Rules::canCastle(const Board& board, int kingX, int kingY, bool isLeft) {
    int rookX = isLeft ? 0 : board.board_size - 1;
    auto king = board.getPiece(board.getKey(kingX, kingY));
    auto rook = board.getPiece(board.getKey(rookX, kingY));

    if (!king || !rook || king->type != "King" || rook->type != "Rook") {
        return false;
    }

    int xStart = std::min(kingX, rookX) + 1;
    int xEnd = std::max(kingX, rookX);
    for (int x = xStart; x < xEnd; ++x) {
        if (board.hasPieceAt(board.getKey(x, kingY))) {
            return false;
        }
    }

    if (isCheck(board, king->getIsWhite())) {
        return false;
    }

    for (int x = xStart; x <= xEnd; ++x) {
        Board testBoard = board.clone();
        testBoard.movePiece(kingX, kingY, x, kingY);
        if (isCheck(testBoard, king->getIsWhite())) {
            return false;
        }
    }

    return true;
}

bool Rules::isValidMove(const Board& board, int x1, int y1, int x2, int y2) {
    if (x1 == x2 && y1 == y2) return false;

    auto piece = board.getPiece(board.getKey(x1, y1));
    if (!piece) return false;

    const auto& rules = piece->getMovementRules();
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    auto targetPiece = board.getPiece(board.getKey(x2, y2));
    if (targetPiece && targetPiece->getIsWhite() == piece->getIsWhite()) {
        return false;
    }

    if (rules.l_shape && ((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) {
        return true;
    }

    if (rules.diagonal && dx == dy) {
        int xStep = (x2 > x1) ? 1 : -1;
        int yStep = (y2 > y1) ? 1 : -1;
        for (int i = 1; i < dx; ++i) {
            if (board.hasPieceAt(board.getKey(x1 + i * xStep, y1 + i * yStep))) {
                return false;
            }
        }
        return true;
    }

    if (piece->type != "Pawn" && rules.forward && dx == 0) {
        int yStep = (y2 > y1) ? 1 : -1;
        for (int y = y1 + yStep; y != y2; y += yStep) {
            if (board.hasPieceAt(board.getKey(x1, y))) {
                return false;
            }
        }
        return true;
    }

    if (rules.sideways && dy == 0) {
        int xStep = (x2 > x1) ? 1 : -1;
        for (int x = x1 + xStep; x != x2; x += xStep) {
            if (board.hasPieceAt(board.getKey(x, y1))) {
                return false;
            }
        }
        return true;
    }

    if (piece->type == "Pawn" && rules.forward) {
        bool isMovingForward = piece->getIsWhite() ? (y2 > y1) : (y2 < y1);

        if (isMovingForward && dx == 0 && dy == 1 && !board.hasPieceAt(board.getKey(x2, y2))) {
            return true;
        }

        if (rules.first_move_forward && isMovingForward && dx == 0 && dy == 2) {
            bool isFirstMove = (piece->getIsWhite() && y1 == 1) || (!piece->getIsWhite() && y1 == board.board_size - 2);
            int yStep = piece->getIsWhite() ? 1 : -1;
            if (isFirstMove && !board.hasPieceAt(board.getKey(x1, y1 + yStep)) &&
                !board.hasPieceAt(board.getKey(x2, y2))) {
                return true;
            }
        }

        if (rules.diagonal_capture && dx == 1 && dy == 1 && targetPiece && targetPiece->getIsWhite() != piece->getIsWhite()) {
            return true;
        }
    }

    return false;
}

#include "board.hpp"
#include <iostream>
#include <memory>
#include <sstream>

Board::Board(int size) : board_size(size) {
    board_map.clear();
    portals.clear();
}

void Board::initialize(const std::vector<PieceConfig> &pieces, const std::vector<PortalConfig> &portals) {
    this->portals = portals;
    for (const auto &piece : pieces) {
        // Beyaz taşları yerleştir
        if (piece.positions.count("white") > 0) {
            for (const auto &pos : piece.positions.at("white")) {
                if (isPositionValid(pos)) {
                    std::string key = posToKey(pos.x, pos.y);
                    auto copy = std::make_shared<PieceConfig>(piece);
                    copy->isWhite = true; 
                    board_map[key] = copy;
                } else {
                    std::cerr << "Invalid white position: (" << pos.x << ", " << pos.y << ")\n";
                }
            }
        }

        // Siyah taşları yerleştir
        if (piece.positions.count("black") > 0) {
            for (const auto &pos : piece.positions.at("black")) {
                if (isPositionValid(pos)) {
                    std::string key = posToKey(pos.x, pos.y);
                    auto copy = std::make_shared<PieceConfig>(piece);
                    copy->isWhite = false;  
                    board_map[key] = copy;
                } else {
                    std::cerr << "Invalid black position: (" << pos.x << ", " << pos.y << ")\n";
                }
            }
        }
    }
}


Board Board::clone() const {
    Board newBoard(board_size);
    for (const auto &entry : board_map) {
        newBoard.board_map[entry.first] = std::make_shared<PieceConfig>(*entry.second);
    }
    newBoard.portals = portals;
    return newBoard;
}

bool Board::isPositionValid(const Position &pos) const {
    return pos.x >= 0 && pos.x < board_size && pos.y >= 0 && pos.y < board_size;
}

void Board::print() const {
    // Sütun harflerini yaz
    std::cout << "\033[33m   ";
    for (char c = 'a'; c < 'a' + board_size; ++c) {
        std::cout << c << " ";
    }
    std::cout << "\033[0m" << std::endl;

    // Satırları yaz
    for (int y = 0; y < board_size; ++y) {
        std::cout << "\033[33m" << (board_size - y) << " ";
        if (board_size - y < 10) std::cout << " ";
        std::cout << "\033[0m";

        for (int x = 0; x < board_size; ++x) {
            std::string key = posToKey(x, y);
            auto it = board_map.find(key);

            if (it != board_map.end() && it->second) {
                // Renk: beyaz için beyaz, siyah için mavi
                std::cout << (it->second->isWhite ? "\033[37m" : "\033[34m");

                // Taş simgesi
                char symbol = (it->second->type == "Knight") ? 'N' : it->second->type[0];
                std::cout << symbol << "\033[0m ";
            } else {
                std::cout << ". ";
            }
        }

        std::cout << "\033[33m" << (board_size - y) << "\033[0m" << std::endl;
    }

    // Alt satırda tekrar sütun harflerini yaz
    std::cout << "\033[33m   ";
    for (char c = 'a'; c < 'a' + board_size; ++c) {
        std::cout << c << " ";
    }
    std::cout << "\033[0m" << std::endl;
}



bool Board::movePiece(int x1, int y1, int x2, int y2) {
    handlePortal(x1, y1, x2, y2);

    std::string fromKey = posToKey(x1, y1);
    std::string toKey = posToKey(x2, y2);

    auto itFrom = board_map.find(fromKey);
    if (itFrom == board_map.end() || !itFrom->second) {
        std::cerr << "Error: No piece at source position (" << x1 << ", " << y1 << ")." << std::endl;
        return false;
    }

    if (!isPositionValid({x2, y2})) {
        std::cerr << "Error: Target position (" << x2 << ", " << y2 << ") is invalid." << std::endl;
        return false;
    }

    board_map[toKey] = std::move(itFrom->second);
    board_map.erase(itFrom);
    return true;
}

bool Board::movePieceForCloneBoard(int x1, int y1, int x2, int y2) {
    std::string fromKey = posToKey(x1, y1);
    std::string toKey = posToKey(x2, y2);

    auto itFrom = board_map.find(fromKey);
    if (itFrom == board_map.end() || !itFrom->second) return false;
    if (!isPositionValid({x2, y2})) return false;

    board_map[toKey] = std::move(itFrom->second);
    board_map.erase(itFrom);
    return true;
}

void Board::handlePortal(int x1, int y1, int &x2, int &y2) {
    auto piece = getPiece(x1, y1);
    if (!piece) return;

    std::string color = piece->getIsWhite() ? "white" : "black";

    for (const auto &portal : portals) {
        if (portal.positions.entry.x == x1 && portal.positions.entry.y == y1) {
            const auto &allowed = portal.properties.allowed_colors;
            if (std::find(allowed.begin(), allowed.end(), color) != allowed.end()) {
                x2 = portal.positions.exit.x;
                y2 = portal.positions.exit.y;
                std::cout << "Moved through portal: (" << x1 << ", " << y1 << ") -> (" << x2 << ", " << y2 << ")\n";
                return;
            } else {
                std::cout << "Portal at (" << x1 << ", " << y1 << ") not allowed for " << color << " pieces.\n";
            }
        }
    }
}

std::string Board::posToKey(int x, int y) const {
    return std::to_string(x) + "," + std::to_string(y);
}

std::string Board::getKey(int x, int y) const {
    return posToKey(x, y);
}

bool Board::hasPieceAt(const std::string &key) const {
    auto it = board_map.find(key);
    return it != board_map.end() && it->second;
}

const std::shared_ptr<PieceConfig>& Board::getPiece(const std::string &key) const {
    auto it = board_map.find(key);
    if (it != board_map.end()) {
        return it->second;
    }
    static std::shared_ptr<PieceConfig> nullPiece = nullptr;
    return nullPiece;
}

const std::shared_ptr<PieceConfig>& Board::getPiece(int x, int y) const {
    std::string key = posToKey(x, y);
    return getPiece(key);  
}


void Board::handleCooldowns() {
    // Placeholder 
}

#include "ConfigReader.hpp"
#include "Game.hpp"
#include "MoveValidator.hpp"
#include "Rules.hpp"
#include <iostream>
#include <string>

void displaySpecialAbilities(const SpecialAbilities &abilities) {
    if (abilities.castling) std::cout << "Castling ";
    if (abilities.royal) std::cout << "Royal ";
    if (abilities.jump_over) std::cout << "Jump-over ";
    if (abilities.promotion) std::cout << "Promotion ";
    if (abilities.en_passant) std::cout << "En-passant ";
    for (const auto &[key, value] : abilities.custom_abilities) {
        if (value) std::cout << key << " ";
    }
}

void displayPieceInfo(const PieceConfig &piece) {
    std::cout << "Type: " << piece.type << " (Count: " << piece.count << ")\n";
    std::cout << "  Movement: ";
    if (piece.movement.forward > 0) std::cout << "Forward: " << piece.movement.forward << " ";
    if (piece.movement.sideways > 0) std::cout << "Sideways: " << piece.movement.sideways << " ";
    if (piece.movement.diagonal > 0) std::cout << "Diagonal: " << piece.movement.diagonal << " ";
    if (piece.movement.l_shape) std::cout << "L-shape: Yes ";
    if (piece.movement.diagonal_capture > 0) std::cout << "Diagonal Capture: " << piece.movement.diagonal_capture << " ";
    if (piece.movement.first_move_forward > 0) std::cout << "First Move Forward: " << piece.movement.first_move_forward << " ";
    std::cout << "\n  Special Abilities: ";
    displaySpecialAbilities(piece.special_abilities);
    std::cout << "\n";

    if (piece.positions.count("white") > 0) {
        std::cout << "  White positions: ";
        for (const auto &pos : piece.positions.at("white")) {
            std::cout << "(" << pos.x << "," << pos.y << ") ";
        }
        std::cout << "\n";
    }
    if (piece.positions.count("black") > 0) {
        std::cout << "  Black positions: ";
        for (const auto &pos : piece.positions.at("black")) {
            std::cout << "(" << pos.x << "," << pos.y << ") ";
        }
        std::cout << "\n";
    }
}

int fileToX(char file) { return file - 'a'; }
int rankToY(char rank, int boardSize) { return boardSize - (rank - '0'); }

bool isKingAlive(const Board &board, bool isWhite) {
    for (int y = 0; y < board.board_size; ++y) {
        for (int x = 0; x < board.board_size; ++x) {
            auto piece = board.getPiece(x, y);
            if (piece && piece->type == "King" && piece->getIsWhite() == isWhite) {
                return true;
            }
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    std::string configPath = "data/chess_pieces.json";
    if (argc > 1) {
        configPath = argv[1];
    }

    ConfigReader configReader;
    if (!configReader.loadFromFile(configPath)) {
        std::cerr << "Failed to load configuration. Exiting.\n";
        return 1;
    }

    const GameConfig &config = configReader.getConfig();

    std::cout << "==== Game Configuration ====\n";
    std::cout << "Game: " << config.game_settings.name << "\n";
    std::cout << "Board size: " << config.game_settings.board_size << "x" << config.game_settings.board_size << "\n";
    std::cout << "Turn limit: " << config.game_settings.turn_limit << "\n";

    std::cout << "\n==== Standard Pieces ====\n";
    for (const auto &piece : config.pieces) {
        displayPieceInfo(piece);
    }

    if (!config.custom_pieces.empty()) {
        std::cout << "\n==== Custom Pieces ====\n";
        for (const auto &piece : config.custom_pieces) {
            displayPieceInfo(piece);
        }
    }

    std::cout << "\n==== Portals ====\n";
    for (const auto &portal : config.portals) {
        std::cout << "Portal ID: " << portal.id << "\n";
        std::cout << "  Entry: (" << portal.positions.entry.x << "," << portal.positions.entry.y << ")\n";
        std::cout << "  Exit: (" << portal.positions.exit.x << "," << portal.positions.exit.y << ")\n";
        std::cout << "  Preserve direction: " << (portal.properties.preserve_direction ? "Yes" : "No") << "\n";
        std::cout << "  Cooldown: " << portal.properties.cooldown << " turns\n";
        std::cout << "  Allowed colors: ";
        for (const auto &color : portal.properties.allowed_colors) {
            std::cout << color << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n==== Starting Game ====\n";
    Board board(config.game_settings.board_size);
    board.initialize(config.pieces, config.portals);
    board.print();

    bool isWhiteTurn = true;
    int turn = 1;

    while (turn <= config.game_settings.turn_limit) {
        std::string input;
        std::cout << (isWhiteTurn ? "White" : "Black") << "'s move (or type 'exit' to quit): ";
        std::getline(std::cin, input);

        if (input == "exit") {
            std::cout << "Game exited by user.\n";
            break;
        }

        if (input.length() == 4 &&
            input[0] >= 'a' && input[0] <= 'h' &&
            input[2] >= 'a' && input[2] <= 'h' &&
            input[1] >= '1' && input[1] <= '8' &&
            input[3] >= '1' && input[3] <= '8') {

            int x1 = fileToX(input[0]);
            int y1 = rankToY(input[1], board.board_size);
            int x2 = fileToX(input[2]);
            int y2 = rankToY(input[3], board.board_size);

            if (!board.isPositionValid({x1, y1}) || !board.isPositionValid({x2, y2})) {
                std::cout << "Invalid coordinates.\n";
                continue;
            }

            auto piece = board.getPiece(x1, y1);
            if (!piece || piece->getIsWhite() != isWhiteTurn) {
                std::cout << "No valid piece at source, or not your turn.\n";
                continue;
            }

            if (!Rules::isValidMove(board, x1, y1, x2, y2)) {
                std::cout << "Invalid move!\n";
                continue;
            }

            // İlk hareket
            board.movePiece(x1, y1, x2, y2);
            int fromPortalX = x2;
            int fromPortalY = y2;
            board.handlePortal(x2, y2, x2, y2);
            if (x2 != fromPortalX || y2 != fromPortalY) {
                board.movePiece(fromPortalX, fromPortalY, x2, y2);
            }

            isWhiteTurn = !isWhiteTurn;
            ++turn;
            board.print();

            //Kral hayatta mı
            bool whiteAlive = isKingAlive(board, true);
            bool blackAlive = isKingAlive(board, false);
            if (!whiteAlive || !blackAlive) {
                std::cout << (whiteAlive ? "White" : "Black") << " wins! Opponent's king is gone.\n";
                break;
            }

        } else {
            std::cout << "Invalid input format! Use format like a2a4.\n";
        }
    }

    return 0;
}

#include "Game.hpp"
#include "Board.hpp"
#include "Rules.hpp"
#include "MoveValidator.hpp"  
#include "PieceHelper.hpp"


#include <iostream>
#include <string>
#include <sstream>

Game::Game(const GameConfig& config)
    : board(config.game_settings.board_size), config(config), isWhiteTurn(true), turnCount(0) {
    board.initialize(config.pieces, config.portals);
}

void Game::start() {
    std::cout << "Starting game: " << config.game_settings.name << std::endl;
    std::cout << "Board size: " << config.game_settings.board_size << "x" << config.game_settings.board_size << std::endl;
    std::cout << "Turn limit: " << config.game_settings.turn_limit << std::endl;

    std::string input;
    while (turnCount < config.game_settings.turn_limit) {
        board.print();
        std::cout << (isWhiteTurn ? "White's turn: " : "Black's turn: ");

        std::getline(std::cin, input);
        if (input == "quit") {
            std::cout << "Game over. Goodbye!" << std::endl;
            break;
        }

        if (!processMove(input)) {
            std::cout << "Invalid move. Try again." << std::endl;
            continue;
        }

        if (checkEndGame()) {
            std::cout << (isWhiteTurn ? "Black" : "White") << " wins!" << std::endl;
            break;
        }
    }

    if (turnCount >= config.game_settings.turn_limit) {
        std::cout << "Turn limit reached! Game is a draw." << std::endl;
    }
}

bool Game::processMove(const std::string& input) {
    int x1, y1, x2, y2;
    if (!parseInput(input, x1, y1, x2, y2)) {
        std::cerr << "Invalid input format. Use format like e2e4." << std::endl;
        return false;
    }

    std::string fromKey = board.getKey(x1, y1);
    std::string toKey = board.getKey(x2, y2);

    if (!board.hasPieceAt(fromKey)) {
        std::cerr << "No piece at the source position." << std::endl;
        return false;
    }

    const auto& piece = board.getPiece(fromKey);

    if (PieceHelper::isWhiteAt(*piece, {x1, y1}) != isWhiteTurn) {
        std::cerr << "It's not your turn." << std::endl;
        return false;
    }

    if (!Rules::isValidMove(board, x1, y1, x2, y2)) {
        std::cerr << "Invalid move for this piece." << std::endl;
        return false;
    }


    MoveValidator validator(board);
    if (!validator.isPathValid({x1, y1}, {x2, y2})) {
        std::cerr << "Path is blocked. Move not allowed.\n";
        return false;
    }

    board.handlePortal(x1, y1, x2, y2);

    std::unique_ptr<PieceConfig> captured = nullptr;
    if (board.hasPieceAt(toKey)) {
        captured = std::make_unique<PieceConfig>(*board.getPiece(toKey));
    }

    if (!board.movePiece(x1, y1, x2, y2)) {
        std::cerr << "Move failed." << std::endl;
        return false;
    }

    recordMove(x1, y1, x2, y2, std::move(captured));
    handleCooldowns();

    isWhiteTurn = !isWhiteTurn;
    turnCount++;
    std::cout << "Move successful: (" << x1 << ", " << y1 << ") -> (" << x2 << ", " << y2 << ")\n";
    return true;
}

bool Game::parseInput(const std::string& input, int& x1, int& y1, int& x2, int& y2) const {
    if (input.length() != 4) return false;

    char x1Char = input[0];
    char y1Char = input[1];
    char x2Char = input[2];
    char y2Char = input[3];

    x1 = x1Char - 'a';
    y1 = config.game_settings.board_size - (y1Char - '0');
    x2 = x2Char - 'a';
    y2 = config.game_settings.board_size - (y2Char - '0');

    return board.isPositionValid({x1, y1}) && board.isPositionValid({x2, y2});
}

bool Game::checkEndGame() const {
    return Rules::isCheckmate(board, !isWhiteTurn);
}

void Game::recordMove(int x1, int y1, int x2, int y2, std::unique_ptr<PieceConfig> captured) {
    moveHistory.push({x1, y1, x2, y2, std::move(captured)});
}

void Game::handleCooldowns() {
    if (!cooldownQueue.empty()) {
        cooldownQueue.pop();
    }
}

#ifndef BOARD_HPP
#define BOARD_HPP

#include "ConfigReader.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

class Board
{
public:
    int board_size;

    // Pozisyon -> Taş eşlemesi
    std::unordered_map<std::string, std::shared_ptr<PieceConfig>> board_map;

    // Portal bilgileri
    std::vector<PortalConfig> portals;

    Board(int size);

    // Tahtayı başlat
    void initialize(const std::vector<PieceConfig> &pieces,
                    const std::vector<PortalConfig> &portals);

    // Pozisyon geçerli mi
    bool isPositionValid(const Position &pos) const;

    // Tahtayı konsola yazdır
    void print() const;

    // Yeni bir klon tahta döndür
    Board clone() const;

    // Taş hareket ettir
    bool movePiece(int x1, int y1, int x2, int y2);

    // Kopyalanan tahtada taş hareketi
    bool movePieceForCloneBoard(int x1, int y1, int x2, int y2);

    // Portal kontrolü 
    void handlePortal(int x1, int y1, int &x2, int &y2);
    void handleCooldowns();

    // Oynanan hamleyi kaydet
    void recordMove(const std::string &input);

    std::string getKey(int x, int y) const;


    bool hasPieceAt(const std::string &key) const;


    const std::shared_ptr<PieceConfig> &getPiece(const std::string &key) const;
    const std::shared_ptr<PieceConfig>& getPiece(int x, int y) const;


private:

    std::string posToKey(int x, int y) const;
};

#endif 

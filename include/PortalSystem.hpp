#ifndef PORTALSYSTEM_HPP
#define PORTALSYSTEM_HPP

#include "Position.hpp"
#include <vector>
#include <unordered_map>
#include <queue>

struct Portal
{
    Position entry;
    Position exit;
    bool isTwoWay;
};

struct CooldownEntry {
    Position portalEntry;
    int remainingTurns;
};

class PortalSystem
{
public:
    PortalSystem();

    void addPortal(const Position& entry, const Position& exit, bool isTwoWay);
    bool canUsePortal(const Position& pos) const;
    Position getTeleportExit(const Position& entry, const Position& direction) const;
    void applyCooldown(const Position& entry);
    void decrementCooldowns();
    bool isOnCooldown(const Position& pos) const;

private:
    std::vector<Portal> portals;
    std::unordered_map<Position, Portal> portalMap;
    std::queue<CooldownEntry> cooldownQueue;
};

#endif

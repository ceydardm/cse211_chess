#include "PortalSystem.hpp"
#include <algorithm>

PortalSystem::PortalSystem() {}

void PortalSystem::addPortal(const Position& entry, const Position& exit, bool isTwoWay)
{
    Portal portal = { entry, exit, isTwoWay };
    portals.push_back(portal);
    portalMap[entry] = portal;

    if (isTwoWay)
    {
        Portal reversePortal = { exit, entry, isTwoWay };
        portals.push_back(reversePortal);
        portalMap[exit] = reversePortal;
    }
}

bool PortalSystem::canUsePortal(const Position& pos) const
{
    return portalMap.find(pos) != portalMap.end() && !isOnCooldown(pos);
}

Position PortalSystem::getTeleportExit(const Position& entry, const Position& direction) const
{
    auto it = portalMap.find(entry);
    if (it != portalMap.end())
    {

        return it->second.exit;
    }
    return entry;
}

void PortalSystem::applyCooldown(const Position& entry)
{
    CooldownEntry entryCooldown = { entry, 2 }; 
    cooldownQueue.push(entryCooldown);
}

void PortalSystem::decrementCooldowns()
{
    size_t size = cooldownQueue.size();
    std::queue<CooldownEntry> updatedQueue;

    for (size_t i = 0; i < size; ++i)
    {
        CooldownEntry cooldown = cooldownQueue.front();
        cooldownQueue.pop();
        cooldown.remainingTurns--;

        if (cooldown.remainingTurns > 0)
        {
            updatedQueue.push(cooldown);
        }
    }

    cooldownQueue = updatedQueue;
}

bool PortalSystem::isOnCooldown(const Position& pos) const
{
    std::queue<CooldownEntry> temp = cooldownQueue;

    while (!temp.empty())
    {
        if (temp.front().portalEntry.x == pos.x && temp.front().portalEntry.y == pos.y)
        {
            return true;
        }
        temp.pop();
    }

    return false;
}

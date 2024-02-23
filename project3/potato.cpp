#include "potato.h"

// constructor
Potato::Potato(int hops_) : hops(hops_) {
    playerTrace = std::vector<int>();
};

// decrease the number of hops by 1
void Potato::decreaseHops() {
    hops--;
}

// append the current playerId to playerTrace
void Potato::appendPlayerId(int playerId) {
    playerTrace.push_back(playerId);
}

// return the current number of hops
int Potato::getCurrHop() {
    return hops;
}

// return the whole playerTrace
const std::vector<int> & Potato::getPlayerTrace() const {
    return playerTrace;
}

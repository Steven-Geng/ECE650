#include "potato.h"
#include <string.h>

// constructor
Potato::Potato(int hops_) : hops(hops_), currHop(0) {
    memset(playerTrace, 0, sizeof(playerTrace));
};

// decrease the number of hops by 1
void Potato::decreaseHops() {
    hops--;
}

// increase the currHop
void Potato::increaseCurrHop(){
    currHop++;
}

// add the current playerId to playerTrace
void Potato::appendPlayerId(int playerId) {
    playerTrace[currHop] = playerId;
}

// return the remaining number of hops
int Potato::getRemainingHop() {
    return hops;
}

// return the current number of hop
int Potato::getCurrHop(){
    return currHop;
}

// return the whole playerTrace
const int * Potato::getPlayerTrace() const {
    return playerTrace;
}

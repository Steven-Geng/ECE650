#ifndef POTATO_H
#define POTATO_H

#include <iostream>
#include <vector>

class Potato {
private:
    int hops;
    int currHop;
    int playerTrace[512];

public:
    Potato(int hops_);
    void decreaseHops();
    void increaseCurrHop();
    void appendPlayerId(int playerId);
    int getRemainingHop();
    int getCurrHop();
    const int * getPlayerTrace() const;
};

#endif
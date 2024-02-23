#ifndef POTATO_H
#define POTATO_H

#include <iostream>
#include <vector>

class Potato {
private:
    int hops;
    std::vector<int> playerTrace;

public:
    Potato(int hops_);
    void decreaseHops();
    void appendPlayerId(int playerId);
    int getCurrHop();
    const std::vector<int> & getPlayerTrace() const;
};

#endif
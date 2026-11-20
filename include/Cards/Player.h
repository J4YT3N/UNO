#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Cards/Deck.h"

struct Player {
    std::string name;
    std::vector<Card> hand;

    void handInit(Deck& deck);

    void printHand();

    void playerInit(Deck& deck);
};
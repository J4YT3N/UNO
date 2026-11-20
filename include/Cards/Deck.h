#pragma once

#include <vector>
#include <random>
#include <optional>

#include "Cards/Card.h"

class Deck {
    private:
        std::vector<Card> cards;
        std::mt19937 rng{std::random_device{}()}; // Random number generator for shuffling

    public:
        void deckInit();
        
        void shuffleDeck();

        std::vector<Card>& getCards();

        std::optional<Card> drawCard();

        void printDeck();
};
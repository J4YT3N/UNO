#include "Cards/Deck.h"

#include <iostream>
#include <algorithm>
#include <optional>

#include "Cards/CardData.h"

void Deck::deckInit() {
    cards.clear();
    for (CardColor color : colors) {
        cards.push_back(Card{color, Value::ZERO});
        for (Value value : values) {
            for (int i = 0; i < 2; i++) {
                cards.push_back(Card{color, value});
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        cards.push_back(Card{CardColor::WILD, Value::WILD});
        cards.push_back(Card{CardColor::WILD, Value::WILDDRAWFOUR});
    }
}
        
void Deck::shuffleDeck() {
    std::shuffle(cards.begin(), cards.end(), rng);
};

std::vector<Card>& Deck::getCards() {
    return cards;
}

std::optional<Card> Deck::drawCard() {
    // Draw card from deck
    if (!cards.empty()) {
        Card drawnCard = cards.back();
        cards.pop_back(); // Remove the drawn card from the deck
        return drawnCard;
    } else {
        return std::nullopt;
    }
};

void Deck::printDeck() {
    for (const Card& card : cards) {
        std::cout << "Card: Color = " << CardData::colorToString(card.color)
                  << ", Value = "     << CardData::valueToString(card.value) << '\n';
    }
    std::cout << "Total cards in deck: " << cards.size() << '\n';
};
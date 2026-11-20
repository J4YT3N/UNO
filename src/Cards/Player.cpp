#include "Cards/Player.h"

#include <optional>

#include "Cards/CardData.h"

void Player::handInit(Deck& deck) {
    hand.clear();

    for (int i = 0; i < 7; i++) {
        std::optional<Card> drawnCard = deck.drawCard();
        if (drawnCard.has_value()) {
            hand.push_back(drawnCard.value());
        }
    }
}

void Player::printHand() {
    std::cout << '\n' << name << "'s hand: \n";
    for (const Card& card : hand) {
        std::cout << "Card: Color = " << CardData::colorToString(card.color)
                  << ", Value = " << CardData::valueToString(card.value) << '\n';
    }
}

void Player::playerInit(Deck& deck) {
    if (name.empty()) {
        std::cout << "Enter player name: ";
        std::cin >> name;
        std::cout << "Would you like to draw a hand of 7 cards? (y/n): ";
        char choice;
        std::cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            handInit(deck);
            printHand();
        }
    }
    else if (name == "CPU") {
        handInit(deck);
        printHand();
    }
    else {
        std::cout << "something went wrong";
    }
}
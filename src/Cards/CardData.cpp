#include "Cards/CardData.h"

#include <string>

std::string CardData::colorToString(CardColor color) {
    switch (color) {
        case CardColor::REDR: return "Red";
        case CardColor::YELLOWY: return "Yellow";
        case CardColor::GREENG: return "Green";
        case CardColor::BLUEB: return "Blue";
        case CardColor::WILD: return "Wild";
    }
    return "Unknown";
}

std::string CardData::valueToString(Value value) {
    switch (value) {
        case Value::ZERO: return "0";
        case Value::ONE: return "1";
        case Value::TWO: return "2";
        case Value::THREE: return "3";
        case Value::FOUR: return "4";
        case Value::FIVE: return "5";
        case Value::SIX: return "6";
        case Value::SEVEN: return "7";
        case Value::EIGHT: return "8";
        case Value::NINE: return "9";

        case Value::SKIP: return "Skip";
        case Value::REVERSE: return "Reverse";
        case Value::DRAWTWO: return "Draw Two";

        case Value::WILD: return "Wild";
        case Value::WILDDRAWFOUR: return "Wild Draw Four";
    }
    return "Unknown";
}
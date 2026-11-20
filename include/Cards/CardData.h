#pragma once

#include <array>

#include "Cards/Card.h"

static constexpr float CARDWIDTH = 256.0f;
static constexpr float CARDHEIGHT = 387.0f;

inline constexpr std::array<CardColor, 4> colors = {
    CardColor::REDR,
    CardColor::YELLOWY,
    CardColor::GREENG,
    CardColor::BLUEB,
};

inline constexpr std::array<Value, 12> values = {
    Value::ONE,
    Value::TWO,
    Value::THREE,
    Value::FOUR,
    Value::FIVE,
    Value::SIX,
    Value::SEVEN,
    Value::EIGHT,
    Value::NINE,

    Value::SKIP,
    Value::REVERSE,
    Value::DRAWTWO,
};

namespace CardData {
    std::string colorToString(CardColor color);

    std::string valueToString(Value value);
    
}

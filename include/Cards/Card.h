#pragma once

enum class CardColor {
    REDR,
    YELLOWY,
    GREENG,
    BLUEB,
    WILD
};

enum class Value {
    ZERO,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,

    SKIP,
    REVERSE,
    DRAWTWO,

    WILD,
    WILDDRAWFOUR,
};

struct Card {
    CardColor color;
    Value value;
    bool isFlipped = false;
};
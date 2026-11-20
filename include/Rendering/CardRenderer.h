#pragma once

#include "raylib.h"
#include "Cards/Card.h"


class CardRenderer {
    private:

        static constexpr int COLUMNS = 4;

        int replacementColorLoc;
        int toleranceLoc;
        int featherLoc;

        Texture2D spritesheet;
        
        Shader shader;

    public:

        CardRenderer();

        ~CardRenderer();

        Texture2D getSpritesheet() const;

        Rectangle getSprite(Value value) const;

        Rectangle getCardBack() const;

        Color getCardColor(CardColor color) const;

        void drawCard(const Card& card, Rectangle destination, float rotation, float opacity);

};
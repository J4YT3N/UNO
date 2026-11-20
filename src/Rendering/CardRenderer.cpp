#include "Rendering/CardRenderer.h"

#include <iostream>

#include "Cards/CardData.h"


CardRenderer::CardRenderer() {
    spritesheet = LoadTexture("assets/cards_spritesheet.png");
    #ifdef __EMSCRIPTEN__
        shader = LoadShader(nullptr, "shaders/card_shader_web.fs");
    #else
        shader = LoadShader(nullptr, "shaders/card_shader.fs");
    #endif

    if (!IsShaderValid(shader)) {
        std::cout << "Shader failed to load... \n";
        return;
    }
            
    replacementColorLoc = GetShaderLocation(shader, "replacementColor");
    toleranceLoc = GetShaderLocation(shader, "tolerance");
    featherLoc = GetShaderLocation(shader, "feather");
}

CardRenderer::~CardRenderer() {
    UnloadTexture(spritesheet);
    UnloadShader(shader);
}

Texture2D CardRenderer::getSpritesheet() const{
    return spritesheet;
}

Rectangle CardRenderer::getSprite(Value value) const {
    int index = static_cast<int>(value);

    int column = index % COLUMNS;
    int row = index / COLUMNS;

    return Rectangle {
        column * CARDWIDTH,
        row * CARDHEIGHT,
        CARDWIDTH,
        CARDHEIGHT
    };
}

Rectangle CardRenderer::getCardBack() const {
    int index = 15;

    int column = index % COLUMNS;
    int row = index / COLUMNS;

    return Rectangle {
        column * CARDWIDTH,
        row * CARDHEIGHT,
        CARDWIDTH,
        CARDHEIGHT
    };
}

Color CardRenderer::getCardColor(CardColor color) const {
    switch (color) {
        case CardColor::REDR : return Color{215, 38 , 0, 255};
        case CardColor::YELLOWY : return Color{236, 212, 7, 255};
        case CardColor::GREENG : return Color{55, 151, 17, 255};
        case CardColor::BLUEB : return Color{9, 86, 191, 255};
        case CardColor::WILD : return WHITE;
    }
    return WHITE;
}

void CardRenderer::drawCard(const Card& card, Rectangle destination, float rotation, float opacity) {
    Rectangle source;

    if (card.isFlipped) {
        source = getCardBack();
    }
    else {
        source = getSprite(card.value);
    }

    Vector2 origin = {
        destination.width / 2.0f,
        destination.height / 2.0f
    };

    Vector4 normalizedColor = ColorNormalize(getCardColor(card.color));

    float color[3] = {
        normalizedColor.x,
        normalizedColor.y,
        normalizedColor.z
    };
            
    SetShaderValue(shader, replacementColorLoc, color, SHADER_UNIFORM_VEC3);

    float tolerance = 0.45f;
    float feather = 0.02f;

    SetShaderValue(shader, toleranceLoc, &tolerance, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, featherLoc, &feather, SHADER_UNIFORM_FLOAT);

    if (replacementColorLoc == -1 ||
        toleranceLoc == -1 ||
        featherLoc == -1) {
            std::cout << "One or more shader uniforms were not found!\n";
    }

    BeginShaderMode(shader);

    DrawTexturePro(
        spritesheet,
        source,
        destination,
        origin,
        rotation,
        Fade(WHITE, opacity)
    );

    EndShaderMode();
}
#include "Game/Game.h"

#include <algorithm>

#include "raylib.h"

#include "Rendering/CardRenderer.h"
#include "Cards/Deck.h"
#include "Cards/CardData.h"


void Game::gameInit() {
    deck.deckInit();
    deck.shuffleDeck();

    discardPile.push_back(deck.getCards().back());
    deck.getCards().pop_back();

    while (discardPile.back().value == Value::WILDDRAWFOUR) {
        deck.getCards().push_back(discardPile.back());
        discardPile.pop_back();

        deck.shuffleDeck();

        discardPile.push_back(deck.getCards().back());
        deck.getCards().pop_back();
    }

    activeColor = discardPile.back().color;

    if (discardPile.back().value == Value::WILD)
    {
        choosingWildColor = true;
    }

    player.handInit(deck);
    updatePlayerHandPositions();

    CPU.handInit(deck);

    menuBackground = LoadTexture("assets/menu_bg.png");
    menuRect = {
        0.0f,
        0.0f,
        960.0f,
        720.0f
    };

    startTexture = LoadTexture("assets/start_button.png");
    startButton = {
        368.1f,
        564.9f,
        244.5f,
        122.0f
    };

    musicStartTime = GetTime() + 6.0;
    music.looping = true;
}

void Game::update() {
    if (!musicStarted && GetTime() >= musicStartTime) {
        PlayMusicStream(music);
        musicStarted = true;
    }

    SetMusicVolume(music, 0.3f);

    if (musicStarted) {
        UpdateMusicStream(music);
    }

    mousePos = GetMousePosition();
    Vector2 wheel = GetMouseWheelMoveV();

    switch (currentScene) {
        case Scene::MENU:
            updateMenu();
            break;

        case Scene::PLAY:
            updatePlay(wheel);
            break;
        case Scene::GAMEOVER:
            break;
    }
}

void Game::updateMenu() {

    if (CheckCollisionPointRec(mousePos, startButton)
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

        currentScene = Scene::PLAY;
    }
    
}

void Game::updatePlay(Vector2 wheel) {

    if (cpuWildColorFlash
        && GetTime() - cpuWildColorFlashStartTime >= 1.0) {
            cpuWildColorFlash = false;
            hoveredWildColor = -1;
    }

    if (cpuWildColorFlash) {
        return;
    }

    if (choosingWildColor) {
        updateWild();
        return;
    }

    float deltaTime = GetFrameTime();

    float stepDeg = updatePlayerHandPositions();

    if (stepDeg > 0.0f) {
        float minOffset = -static_cast<float>(player.hand.size() - 1) * stepDeg;
        float maxOffset = 0.0f;

        if (CheckCollisionPointRec(mousePos, wheelBox)
            && (wheel.x != 0.0f || wheel.y != 0.0f)) {

            carouselVelocity += (wheel.x + wheel.y) * 10.0f;
            isSnapping = false;
            snapVelocity = 0.0f;
        }

        carouselOffset += carouselVelocity * deltaTime;
        carouselVelocity *= expf(-friction * deltaTime);

        if (carouselOffset <= minOffset) {
            carouselOffset = minOffset;

            if (carouselVelocity < 0.0f)
                carouselVelocity = 0.0f;
        }

        if (carouselOffset >= maxOffset) {
            carouselOffset = maxOffset;

            if (carouselVelocity > 0.0f)
                carouselVelocity = 0.0f;
        }

        if (!isSnapping && fabsf(carouselVelocity) < snapVelocityThreshold
            && fabsf(carouselVelocity) > 0.0f) {

            float nearestCard = roundf(carouselOffset / stepDeg) * stepDeg;

            snapTarget = std::clamp(nearestCard, minOffset, maxOffset);
            isSnapping = true;
            snapVelocity = carouselVelocity;
            carouselVelocity = 0.0f;
        }

        if (isSnapping) {
            float difference = snapTarget - carouselOffset;
            float direction = (difference > 0.0f) ? 1.0f : -1.0f;

            float stoppingSpeed = sqrtf(
                2.0f * snapAcceleration * fabsf(difference)
            );

            float targetSpeed = fminf(stoppingSpeed, snapMaxSpeed);

            snapVelocity += direction * snapAcceleration * deltaTime;
            snapVelocity = direction * fminf(fabsf(snapVelocity), targetSpeed);

            carouselOffset += snapVelocity * deltaTime;

            if (fabsf(difference) < 0.15f && fabsf(snapVelocity) < 1.0f) {
                carouselOffset = snapTarget;
                snapVelocity = 0.0f;
                isSnapping = false;
            }
        }
    }
    else {
        carouselOffset = 0.0f;
        carouselVelocity = 0.0f;
        snapVelocity = 0.0f;
        isSnapping = false;
    }

    updatePlayerHandPositions();

    if (!choosingWildColor) {
        updateCardHover();
    }

    if (currentTurn == Turn::PLAYER
        && hoveredCardIndex != -1
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        && (!hasDrawnCard || hoveredCardIndex == drawnCardIndex)) {

        selectedCard = player.hand.at(hoveredCardIndex);

        if (checker()) {
            discardPile.push_back(selectedCard);
            player.hand.erase(player.hand.begin() + hoveredCardIndex);

            checkWinner();

            if (gameOver) return;

            isTurnValid = false;

            if (selectedCard.color != CardColor::WILD) {
                activeColor = selectedCard.color;
            }

            specialCases(selectedCard);

            updatePlayerHandPositions();
        }

        updatePlayerHandPositions();       
    }

    if (currentTurn == Turn::PLAYER
        && !hasDrawnCard
        && CheckCollisionPointRec(mousePos, drawPileRect)
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

        drawnCard = deck.drawCard();

        if (drawnCard.has_value()) {
            int insertIndex = (centeredCardIndex != -1)
                ? centeredCardIndex
                : 0;

            player.hand.insert(
                player.hand.begin() + insertIndex,
                drawnCard.value()
            );

            drawnCardIndex = insertIndex;

            selectedCard = player.hand[drawnCardIndex];

            updatePlayerHandPositions();
            updateCardHover();

            if (checker()) {
                hasDrawnCard = true;
            }
            else {
                drawnCardIndex = -1;
                nextTurn();
            }
        }
    }

    if (currentTurn == Turn::CPU) {
        CPUTurn();
    }
}

void Game::insertCardAtFocusedPosition(Player& target, const Card& card) {
    int insertIndex = 0;

    if (&target == &player && centeredCardIndex != -1) {
        insertIndex = centeredCardIndex;
    }

    insertIndex = std::clamp(
        insertIndex,
        0,
        static_cast<int>(target.hand.size())
    );

    target.hand.insert(target.hand.begin() + insertIndex, card);
}

void Game::specialCases(const Card& card) {
    switch (card.value) {
        case Value::SKIP:
        case Value::REVERSE:
            nextTurn();
            nextTurn();
            break;

        case Value::DRAWTWO:
            for (int i = 0; i < 2; i++) {
                drawnCard = deck.drawCard();

                if (!drawnCard.has_value()) {
                    return;
                }

                if (currentTurn == Turn::PLAYER) {
                    insertCardAtFocusedPosition(CPU, drawnCard.value());
                } else {
                    insertCardAtFocusedPosition(player, drawnCard.value());
                }
            }

            nextTurn();
            nextTurn();
            break;
        
        case Value::WILD:
            if (currentTurn == Turn::CPU) {
                int colorCounts[4] = {0, 0, 0, 0};

                for (const Card& cpuCard : CPU.hand) {
                    switch (cpuCard.color) {
                        case CardColor::REDR:
                            colorCounts[0]++;
                            break;

                        case CardColor::BLUEB:
                            colorCounts[1]++;
                            break;

                        case CardColor::YELLOWY:
                            colorCounts[2]++;
                            break;

                        case CardColor::GREENG:
                            colorCounts[3]++;
                            break;

                        default:
                            break;
                    }
                }

                int chosenIndex = static_cast<int>(
                    std::max_element(
                        colorCounts,
                        colorCounts + 4
                    ) - colorCounts
                );

                switch (chosenIndex) {
                    case 0: activeColor = CardColor::REDR; break;
                    case 1: activeColor = CardColor::BLUEB; break;
                    case 2: activeColor = CardColor::YELLOWY; break;
                    case 3: activeColor = CardColor::GREENG; break;
                }

                hoveredWildColor = chosenIndex;
                cpuWildColorFlash = true;
                cpuWildColorFlashStartTime = GetTime();

                nextTurn();
            }
            else {
                choosingWildColor = true;
                pendingWildDrawFour = false;
            }

            return;

        case Value::WILDDRAWFOUR:
            for (int i = 0; i < 4; i++) {
                drawnCard = deck.drawCard();

                if (!drawnCard.has_value()) {
                    return;
                }

                if (currentTurn == Turn::PLAYER) {
                    insertCardAtFocusedPosition(CPU, drawnCard.value());
                } else {
                    insertCardAtFocusedPosition(player, drawnCard.value());
                }
            }

            if (currentTurn == Turn::CPU) {
                int colorCounts[4] = {0, 0, 0, 0};

                for (const Card& cpuCard : CPU.hand) {
                    switch (cpuCard.color) {
                        case CardColor::REDR:
                            colorCounts[0]++;
                            break;

                        case CardColor::BLUEB:
                            colorCounts[1]++;
                            break;

                        case CardColor::YELLOWY:
                            colorCounts[2]++;
                            break;

                        case CardColor::GREENG:
                            colorCounts[3]++;
                            break;

                        default:
                            break;
                    }
                }

                int chosenIndex = static_cast<int>(
                    std::max_element(
                        colorCounts,
                        colorCounts + 4
                    ) - colorCounts
                );

                switch (chosenIndex) {
                    case 0: activeColor = CardColor::REDR; break;
                    case 1: activeColor = CardColor::BLUEB; break;
                    case 2: activeColor = CardColor::YELLOWY; break;
                    case 3: activeColor = CardColor::GREENG; break;
                }

                hoveredWildColor = chosenIndex;
                cpuWildColorFlash = true;
                cpuWildColorFlashStartTime = GetTime();

                nextTurn();
                nextTurn();
                break;
            }
            else {
                choosingWildColor = true;
                pendingWildDrawFour = true;
                return;
            }
        
        default:
            nextTurn();
            break;
    }
}

void Game::nextTurn() {
    if (currentTurn == Turn::PLAYER) {
        currentTurn = Turn::CPU;
    }
    else {
        currentTurn = Turn::PLAYER;
    }

        hasDrawnCard = false;
        drawnCardIndex = -1;
        cpuTurnWaiting = false;
}

void Game::CPUTurn() {
    if (!cpuTurnWaiting) {
        cpuTurnStartTime = GetTime();
        cpuTurnWaiting = true;
        return;
    }

    if (GetTime() - cpuTurnStartTime < 2.0) {
        return;
    }

    cpuTurnWaiting = false;

    bool cardPlayed = false;
    
    Card playedCard;

    for (int i = 0; i < CPU.hand.size(); i++ ) {
        selectedCard = CPU.hand.at(i);

        if (checker()) {
            playedCard = selectedCard;
            discardPile.push_back(selectedCard);
            CPU.hand.erase(CPU.hand.begin() + i);

            checkWinner();
            if (gameOver) return;

            cardPlayed = true;

            break;
        }
    }

    if (!cardPlayed) {
        drawnCard = deck.drawCard();

        if (drawnCard.has_value()) {
            CPU.hand.push_back(drawnCard.value());
        }

        nextTurn();
        return;
    }

    if (cardPlayed) {
        isTurnValid = false;

        if (playedCard.color != CardColor::WILD) {
            activeColor = playedCard.color;
        }

        specialCases(playedCard);
    }
}

bool Game::checker() {
    if (selectedCard.value == discardPile.back().value
        || selectedCard.value == Value::WILD
        || selectedCard.color == CardColor::WILD
        || selectedCard.color == activeColor) {

        isTurnValid = true;
        return true;
    }
    else {
        isTurnValid = false;
        return false;
    }
}

void Game::draw() {
    switch (currentScene) {
        case Scene::MENU:
            drawMenu();
            break;

        case Scene::PLAY:
            drawPlay();
            break;
        
        case Scene::GAMEOVER:
            if (playerWon) {
                drawWin();
            }
            else {
                drawLoss();
            }
            break;
    }
}

void Game::drawMenu() {

    DrawTexture(
        menuBackground,
        menuRect.x,
        menuRect.y,
        WHITE
    );

    DrawTexture(
        startTexture,
        startButton.x,
        startButton.y,
        WHITE
    );

}

void Game::drawPlay() {

    drawDrawPile();

    if (!discardPile.empty()) {
        drawDiscardPile();
    }

    if (!player.hand.empty()) {
        drawPlayerHand();
    }

    if (currentTurn == Turn::PLAYER) {
        DrawTextEx(font, "Your Turn...", {410, 8}, 25.0f, 1.0f, BLACK);
    }
    else {
        DrawTextEx(font, "CPU's Turn...", {410, 8}, 25.0f, 1.0f, BLACK);
    }

    if (choosingWildColor || cpuWildColorFlash) {
        drawWild();
    }
}

void Game::drawDrawPile() {
    Rectangle source = cardRenderer.getCardBack();

    DrawTextureRec(
        cardRenderer.getSpritesheet(),
        source,
        {drawPileRect.x, drawPileRect.y},
        WHITE
    );
}

void Game::drawDiscardPile() {
    if (discardPile.empty()) return;

    cardRenderer.drawCard(
        discardPile.back(),
        discardPileRect,
        0.0f,
        1.0f
    );
}

void Game::updateWild() {
    hoveredWildColor = -1;

    Rectangle boxes[] = {
        redRect,
        blueRect,
        yellowRect,
        greenRect
    };

    for (int i = 0; i < 4; i++) {
        Rectangle hitbox = boxes[i];

        hitbox.x -= hitbox.width / 2.0f;
        hitbox.y -= hitbox.height / 2.0f;

        if (CheckCollisionPointRec(mousePos, hitbox)) {
            hoveredWildColor = i;
            break;
        }
    }

    if (hoveredWildColor != -1 &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

        switch (hoveredWildColor) {
            case 0: activeColor = CardColor::REDR; break;
            case 1: activeColor = CardColor::BLUEB; break;
            case 2: activeColor = CardColor::YELLOWY; break;
            case 3: activeColor = CardColor::GREENG; break;
        }

        choosingWildColor = false;
        if (pendingWildDrawFour) {
            pendingWildDrawFour = false;
            nextTurn();
            nextTurn();
        }
        else {
            nextTurn();
        }
    }
}

void Game::drawWild() {
    float scale = 0.825f;
    float opacity = 0.625f;

    DrawRectangle(
        0.0f,
        0.0f,
        960,
        720,
        Fade(GRAY, 0.9f)
    );

    Rectangle boxes[] = {
        redRect,
        blueRect,
        yellowRect,
        greenRect
    };

    Texture2D textures[] = {
        redBox,
        blueBox,
        yellowBox,
        greenBox
    };

    for (int i = 0; i < 4; i++) {
        Rectangle destination = boxes[i];

        if (i == hoveredWildColor) {
            scale = 1.0f;
            opacity = 1.0f;
        }
        else {
            scale = 0.825f;
            opacity = 0.625f;
        }

    destination.width *= scale;
    destination.height *= scale;

    destination.x -= destination.width / 2.0f;
    destination.y -= destination.height / 2.0f;

    DrawTexturePro(
            textures[i],
            {0, 0, (float)textures[i].width, (float)textures[i].height},
            destination,
            {0.0f, 0.0f},
            0.0f,
            Fade(WHITE, opacity)
        );
    }
}

bool Game::isMouseOverCard(Vector2 mousePos, Rectangle cardRect, float rotation) {
    Vector2 center = {
        cardRect.x,
        cardRect.y
    };

    float angle = -rotation * DEG2RAD;

    float dx = mousePos.x - center.x;
    float dy = mousePos.y - center.y;

    Vector2 rotatedMouse = {
        dx * cosf(angle) - dy * sinf(angle),
        dx * sinf(angle) + dy * cosf(angle)
    };

    return (
        rotatedMouse.x >= -cardRect.width / 2.0f &&
        rotatedMouse.x <= cardRect.width / 2.0f &&
        rotatedMouse.y >= -cardRect.height / 2.0f &&
        rotatedMouse.y <= cardRect.height / 2.0f
    );
}

void Game::updateCardHover() {
    float scale = 0.825f;

    hoveredCardIndex = -1;

    for (int i = static_cast<int>(playerCardRects.size()) - 1; i >= 0; i--) {
        Rectangle hitbox = playerCardRects[i];

        hitbox.width *= scale;
        hitbox.height *= scale;

        Rectangle raisedHitbox = hitbox;
        raisedHitbox.y -= 112.0f;

        if (
            isMouseOverCard(mousePos, hitbox, playerCardRotations[i]) ||
            isMouseOverCard(mousePos, raisedHitbox, playerCardRotations[i])
        ) {
            hoveredCardIndex = i;
            break;
        }
    }

    activeCardIndex = (hoveredCardIndex != -1)
        ? hoveredCardIndex
        : centeredCardIndex;
}

float Game::updatePlayerHandPositions() {
    playerCardRects.clear();
    playerCardRotations.clear();

    int cardCount = static_cast<int>(player.hand.size());

    if (cardCount == 0) return 0.0f;

    const float radius = 960.0f;

    const Vector2 arcCenter = {
        480.0f,
        1797.0f
    };

    const float startStepDeg = 15.0f;
    const float minStepDeg = 7.0f;
    const int crowdThreshold = 9;

    const float maxSpanDeg = startStepDeg * (crowdThreshold - 1);

    float stepDeg = startStepDeg;


    if (cardCount > crowdThreshold) {
        stepDeg = maxSpanDeg / (cardCount - 1);

        if (stepDeg < minStepDeg) {
            stepDeg = minStepDeg;
        }
    }

    centeredCardIndex = -1;

    int index = static_cast<int>(roundf(-carouselOffset / stepDeg));

    if (index >= 0 && index < cardCount) {
        centeredCardIndex = index;
    }

    float totalSpanDeg = (cardCount > 1)
        ? stepDeg * (cardCount - 1)
        : 0.0f;

    float startAngleDeg = -90.0f;

    for (int i = 0; i < cardCount; i++) {
        float angleDeg = startAngleDeg + (i * stepDeg) + carouselOffset;
        float angleRad = angleDeg * DEG2RAD;
        
        float cx = arcCenter.x + radius * cosf(angleRad);
        float cy = arcCenter.y + radius * sinf(angleRad);

        float cardCenterX = cx + cosf(angleRad) * (CARDHEIGHT / 2.0f);
        float cardCenterY = cy + sinf(angleRad) * (CARDHEIGHT / 2.0f);

        Rectangle destination = {
            cardCenterX,
            cardCenterY,
            CARDWIDTH,
            CARDHEIGHT
        };

        playerCardRects.push_back(destination);
        playerCardRotations.push_back(angleDeg + 90.0f);
    }

    return stepDeg;
}

void Game::drawPlayerHand() {
    float scale = 0.825f;
    float opacity = 1.0f;

    // Draw all inactive cards first
    for (int i = 0; i < static_cast<int>(player.hand.size()); i++) {
        if (i == activeCardIndex) continue;

        Rectangle scaledRect = playerCardRects[i];

        scaledRect.width *= scale;
        scaledRect.height *= scale;

        opacity = 0.625f;

        cardRenderer.drawCard(
            player.hand[i],
            scaledRect,
            playerCardRotations[i],
            opacity
        );
    }

    // Draw the active card last
    if (activeCardIndex != -1) {
        Rectangle activeRect = playerCardRects[activeCardIndex];

        // Only raise the card if it's hovered
        if (activeCardIndex == hoveredCardIndex) {
            activeRect.y -= 112.0f;
            opacity = 1.0f;
        }

        cardRenderer.drawCard(
            player.hand[activeCardIndex],
            activeRect,
            playerCardRotations[activeCardIndex],
            opacity
        );
    }
}

void Game::checkWinner() {
    if (player.hand.empty()) {
        playerWon = true;
        gameOver = true;
        currentScene = Scene::GAMEOVER;
    }
    else if (CPU.hand.empty()) {
        playerWon = false;
        gameOver = true;
        currentScene = Scene::GAMEOVER;
    }
}

void Game::updateGameover() {
    if (playerWon) {
        drawWin();
    }
    else {
        drawLoss();
    }
}

void Game::drawWin() {
    DrawRectangle(
        0,
        0,
        960,
        720,
        burgundy
    );

    DrawTextEx(
        font, "You Won!", 
        {152.43f, 306.49f},
        100.0f, 1.0f,
        BLACK);
}

void Game::drawLoss() {
    DrawRectangle(
        0,
        0,
        960,
        720,
        BLACK
    );

    DrawTextEx(
        font,
        "*n*",
        {122.16f, 190.27f},
        316.0f, 1.0f,
        burgundy);
}

Game::~Game() {
    UnloadTexture(menuBackground);
    UnloadTexture(startTexture);
    UnloadFont(font);
    UnloadMusicStream(music);
}

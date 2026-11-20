#pragma once

#include "Cards/Deck.h"
#include "Cards/Player.h"
#include "Rendering/CardRenderer.h"

#include <functional>

#include "raylib.h"

enum class Scene {
    MENU,
    PLAY,
    GAMEOVER
};

enum class Turn {
    PLAYER,
    CPU
};

class Game {

private:
    Deck deck;
    std::vector<Card> discardPile;
    Player player;
    Player CPU;

    CardRenderer cardRenderer;

    Scene currentScene = Scene::MENU;

    Texture2D menuBackground;
    Rectangle menuRect;

    Vector2 mousePos = GetMousePosition();

    Texture2D startTexture;
    Rectangle startButton;

    Font font = LoadFont("assets/BoldPixels.ttf");

    Music music = LoadMusicStream("assets/song.wav");
    double musicStartTime = 0.0f;
    bool musicStarted = false;

    Rectangle drawPileRect = {
        192.0f,
        39.0f,
        256.0f,
        387.0f
    };

    Rectangle discardPileRect = {
        640.0f,
        232.5f,
        256.0f,
        387.0f
    };

    Texture2D redBox = LoadTexture("assets/red.png");
    Texture2D blueBox = LoadTexture("assets/blue.png");
    Texture2D yellowBox = LoadTexture("assets/yellow.png");
    Texture2D greenBox = LoadTexture("assets/green.png");
    
    Rectangle redRect = {
        352.0f,
        232.0f,
        256.0f,
        256.0f
    };

    Rectangle blueRect = {
        608.0f,
        232.0f,
        256.0f,
        256.0f
    };

    Rectangle yellowRect = {
        352.0f,
        488.0f,
        256.0f,
        256.0f
    };

    Rectangle greenRect = {
        608.0f,
        488.0f,
        256.0f,
        256.0f
    };

    

    std::vector<Rectangle> playerCardRects;
    std::vector<float> playerCardRotations;

    float carouselOffset = 0.0f;
    float carouselVelocity = 0.0f;
    float friction = 5.0f;

    Rectangle wheelBox = {
        0.0f,
        450.0f,
        960.0f,
        270.0f
    };

    std::optional<Card> drawnCard;
    Card selectedCard;
    CardColor activeColor;

    float snapVelocityThreshold = 4.0f;
    float snapAcceleration = 100.0f;
    float snapMaxSpeed = 45.0f;
    float snapVelocity = 0.0f;
    float maxSnapDistance = 15.0f;

    bool isSnapping = false;
    float snapTarget = 0.0f;

    int hoveredCardIndex = -1;
    int centeredCardIndex = -1;
    int activeCardIndex = -1;
    int drawnCardIndex = -1;

    int hoveredWildColor = -1;

    Turn currentTurn = Turn::PLAYER;;
    bool hasDrawnCard = false;

    bool isTurnValid = false;

    bool choosingWildColor = false;
    bool cpuWildColorFlash = false;

    double cpuWildColorFlashStartTime = 0.0f;

    bool pendingWildDrawFour = false;

    double cpuTurnStartTime = 0.0f;
    bool cpuTurnWaiting = false;

    bool playerWon = false;
    bool gameOver = false;

    Color burgundy = {76, 26, 26, 255};

public:

    void gameInit();

    void update();

    void draw();

    ~Game();

private:

    void updateMenu();
    void drawMenu();

    void updatePlay(Vector2 wheel);
    void drawPlay();

    void insertCardAtFocusedPosition(Player& target, const Card& card);

    void specialCases(const Card& card);

    void nextTurn();

    void CPUTurn();

    bool checker();

    void drawDrawPile();

    void drawDiscardPile();

    void updateWild();
    void drawWild();

    bool isMouseOverCard(Vector2 mousePos, Rectangle cardRect, float rotation);

    void updateCardHover();

    float updatePlayerHandPositions();
    void drawPlayerHand();

    void checkWinner();

    void updateGameover();
    void drawWin();
    void drawLoss();
};
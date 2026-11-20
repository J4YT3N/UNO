#include <raylib.h>

#include "Game/Game.h"

int main() {
    // init app

    InitWindow(960, 720, "UNO");

    InitAudioDevice();

    ChangeDirectory(GetApplicationDirectory());

    {

        Game game;

        game.gameInit();

        // run app

        while (!WindowShouldClose()) {
            game.update();

            BeginDrawing();

                ClearBackground(SKYBLUE);
                game.draw();

            EndDrawing();

        }

    }

    // close app
    CloseAudioDevice();
    CloseWindow();

}
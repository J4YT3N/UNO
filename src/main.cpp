
#include <raylib.h>

#include "Game/Game.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

Game* gameInstance = nullptr;

void UpdateDrawFrame() {
    gameInstance->update();

    BeginDrawing();
        ClearBackground(SKYBLUE);
        gameInstance->draw();
    EndDrawing();
}
#endif

int main() {
    InitWindow(960, 720, "UNO");
    InitAudioDevice();

#ifdef __EMSCRIPTEN__

    static Game game;
    gameInstance = &game;

    game.gameInit();

    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);

#else

    ChangeDirectory(GetApplicationDirectory());

    {
        Game game;
        game.gameInit();

        while (!WindowShouldClose()) {
            game.update();

            BeginDrawing();
                ClearBackground(SKYBLUE);
                game.draw();
            EndDrawing();
        }
    }

    CloseAudioDevice();
    CloseWindow();

#endif
}

// main.c : This file contains the 'main' function. Program execution begins and ends there.
//

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

#define COLS 12
#define ROWS 12

const int TILE_WIDTH = SCREEN_WIDTH / COLS;
const int TILE_HEIGHT = SCREEN_HEIGHT / ROWS;

typedef struct {
    int x;
    int y;
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int nearbyMineCount;
} sTile;

sTile grid[COLS][ROWS];
int revealedTilesCount;
int minesPresentCount;

float timeGameStarted;
float timeGameEnded;

const char* labelGameWin = "YOU WIN";
const char* labelGameLose = "GAME OVER";
const char* labelEnter = "PRESS ENTER FOR MAIN MENU";

bool isSoundEnabled = true;
bool isMusicEnabled = true;

#define MAX_TEXTURES 1
typedef enum {
    TEXTURE_FLAG = 0
} texture_asset;

Texture2D textures[MAX_TEXTURES];

#define MAX_SOUNDS 3
typedef enum {
    SOUND_ONE = 0,
    SOUND_TWO,
    SOUND_THREE
} sound_asset;

Sound sounds[MAX_SOUNDS];

#define MAX_MUSIC 1
typedef enum {
    MUSIC_ONE = 0
} music_asset;

Music music[MAX_MUSIC];

typedef enum {
    STATE_MAIN_MENU = 0,
    STATE_OPTIONS_MENU,
    STATE_PLAYING,
    STATE_LOSE,
    STATE_WIN
} game_states;

game_states gameState;

void GameStartup();
void GameUpdate();
void GameShutdown();
void GameRender();
void GameReset();
void GamePlaySound(int sound);

void ResetTiles();
void RenderTiles();
void RenderTile(sTile);
int CountNearbyMines(int, int);
bool IsTileIndexValid(int, int);
void RevealTile(int, int);
void FlagTile(int, int);
void RevealTilesFrom(int, int);

void GamePlaySound(int sound) {
    if (isSoundEnabled) {
        PlaySound(sounds[sound]);
    }
}


bool IsTileIndexValid(int col, int row) {
    return col >= 0 && col < COLS && row > 0 && row < ROWS;
}

void RevealTilesFrom(int col, int row) {
    for (int colOffset = -1; colOffset <= 1; colOffset++)
    {
        for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
        {
            if (colOffset == 0 && rowOffset == 0) {
                continue;
            }

            if (!IsTileIndexValid(col + colOffset, row + rowOffset)) {
                continue;
            }

            RevealTile(col + colOffset, row + rowOffset);
        }
    }
}

void FlagTile(int col, int row) {
    if (grid[col][row].isFlagged) {
        return;
    }

    grid[col][row].isFlagged = !grid[col][row].isFlagged;
    GamePlaySound(SOUND_THREE);
}

void RevealTile(int col, int row) {
    if (grid[col][row].isFlagged ||
        grid[col][row].isRevealed) {
        return;
    }

    grid[col][row].isRevealed = true;

    if (grid[col][row].isMine) {
        gameState = STATE_LOSE;
        timeGameEnded = (float)GetTime();
        GamePlaySound(SOUND_TWO);
    }
    else {
        if (grid[col][row].nearbyMineCount == 0) {
            RevealTilesFrom(col, row);
        }

        revealedTilesCount++;

        if (revealedTilesCount >= (ROWS * COLS - minesPresentCount)) {
            gameState = STATE_WIN;
            timeGameEnded = (float)GetTime();
        }
    }
}

int CountNearbyMines(int col, int row) {
    int count = 0;
    for (int colOffset = -1; colOffset <= 1; colOffset++) {
        for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
            if (colOffset == 0 && rowOffset == 0) {
                continue;
            }

            if (grid[col + colOffset][row + rowOffset].isMine) {
                count++;
            }
        }
    }

    return count;
}

void ResetTiles() {
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            grid[i][j] = (sTile)
            {
                .x = i,
                .y = j,
                .isMine = false,
                .isRevealed = false,
                .isFlagged = false,
                .nearbyMineCount = -1
            };
        }
    }

    minesPresentCount = (int)(ROWS * COLS * 0.1f);
    int minesToPlace = minesPresentCount;
    while (minesToPlace > 0)
    {
        int col = GetRandomValue(0, COLS);
        int row = GetRandomValue(0, ROWS);

        if (!grid[col][row].isMine) {
            grid[col][row].isMine = true;
            minesToPlace--;
        }
    }

    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            if (!grid[i][j].isMine) {
                grid[i][j].nearbyMineCount = CountNearbyMines(i, j);
            }
        }
    }

}

void RenderTile(sTile tile) {

    if (tile.isRevealed) {
        if (tile.isMine) {
            DrawRectangle(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, RED);
        }
        else {
            DrawRectangle(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, LIGHTGRAY);
            if (tile.nearbyMineCount > 0) {
                DrawText(TextFormat("%d", tile.nearbyMineCount), tile.x * TILE_WIDTH + 13, tile.y * TILE_HEIGHT + 4, TILE_HEIGHT - 8, DARKGRAY);
            }
        }
    }
    else if (tile.isFlagged) {
        Rectangle source = { 0, 0, (float)textures[TEXTURE_FLAG].width, (float)textures[TEXTURE_FLAG].height };
        Rectangle dest = { (float)(tile.x * TILE_WIDTH), (float)(tile.y * TILE_HEIGHT), (float)TILE_WIDTH, (float)TILE_HEIGHT };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(textures[TEXTURE_FLAG], source, dest, origin, 0.0f, WHITE);
    }

    DrawRectangleLines(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, WHITE);
}

void RenderTiles() {
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            RenderTile(grid[i][j]);
        }
    }
}




void GameStartup() {
    InitAudioDevice();

    Image image1 = LoadImage("assets/flag.png");
    textures[TEXTURE_FLAG] = LoadTextureFromImage(image1);
    UnloadImage(image1);

    sounds[SOUND_ONE] = LoadSound("assets/click.wav");
    sounds[SOUND_TWO] = LoadSound("assets/explosion.wav");
    sounds[SOUND_THREE] = LoadSound("assets/pickupCoin.wav");

    music[MUSIC_ONE] = LoadMusicStream("assets/8-bit-game-158815.mp3");

    PlayMusicStream(music[MUSIC_ONE]);

    gameState = STATE_MAIN_MENU;
}

void GameUpdate() {

    UpdateMusicStream(music[MUSIC_ONE]);

    switch (gameState) {
    case STATE_MAIN_MENU:

        if (IsKeyPressed(KEY_N)) {
            GamePlaySound(SOUND_TWO);
            GameReset();
        }
        else if (IsKeyPressed(KEY_O)) {
            gameState = STATE_OPTIONS_MENU;
            GamePlaySound(SOUND_TWO);
        }

        break;
    case STATE_OPTIONS_MENU:
        if (IsKeyPressed(KEY_ENTER)) {
            gameState = STATE_MAIN_MENU;
            GamePlaySound(SOUND_TWO);
        }

        if (IsKeyPressed(KEY_S)) {
            isSoundEnabled = !isSoundEnabled;
            PlaySound(sounds[SOUND_ONE]);
        }

        if (IsKeyPressed(KEY_M)) {
            isMusicEnabled = !isMusicEnabled;
            PlaySound(sounds[SOUND_ONE]);
            if (isMusicEnabled) {
                StopMusicStream(music[MUSIC_ONE]);
                PlayMusicStream(music[MUSIC_ONE]);
            }
            else {
                StopMusicStream(music[MUSIC_ONE]);
            }
        }



        break;
    case STATE_PLAYING:

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            int col = (int)(mousePos.x / TILE_WIDTH);
            int row = (int)(mousePos.y / TILE_HEIGHT);

            if (IsTileIndexValid(col, row)) {
                RevealTile(col, row);
                GamePlaySound(SOUND_ONE);
            }
        }
        else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 mousePos = GetMousePosition();
            int col = (int)(mousePos.x / TILE_WIDTH);
            int row = (int)(mousePos.y / TILE_HEIGHT);
            if (IsTileIndexValid(col, row)) {
                FlagTile(col, row);
            }
        }

        break;
    case STATE_LOSE:

        if (IsKeyPressed(KEY_ENTER))
        {
            GamePlaySound(SOUND_TWO);
            gameState = STATE_MAIN_MENU;
        }

        break;
    case STATE_WIN:

        if (IsKeyPressed(KEY_ENTER))
        {
            GamePlaySound(SOUND_TWO);
            gameState = STATE_MAIN_MENU;
        }

        break;
    }



}

void GameRender() {

    int seconds = 0;

    switch (gameState) {
    case STATE_MAIN_MENU:
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKBLUE);
        DrawText("MINESWEEPER", 20, 20, 40, WHITE);
        DrawText("[N]ew Game", 120, 220, 20, WHITE);
        DrawText("[O]ptions", 120, 250, 20, WHITE);
        DrawText("ESC to QUIT", 120, 280, 20, WHITE);
        DrawText("Copyright 2024 @eyuzwa", 120, 500, 20, WHITE);
        break;
    case STATE_OPTIONS_MENU:

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKBLUE);
        DrawText("MINESWEEPER :: OPTIONS", 20, 20, 40, WHITE);

        DrawText("[S]ound ", 120, 220, 20, WHITE);
        if (isSoundEnabled) {
            DrawText("ON", 280, 220, 20, YELLOW);
            DrawText(" / ", 310, 220, 20, WHITE);
            DrawText("OFF", 350, 220, 20, WHITE);
        }
        else {
            DrawText("ON", 280, 220, 20, WHITE);
            DrawText(" / ", 310, 220, 20, WHITE);
            DrawText("OFF", 350, 220, 20, YELLOW);
        }

        DrawText("[M]usic", 120, 250, 20, WHITE);
        if (isMusicEnabled) {
            DrawText("ON", 280, 250, 20, YELLOW);
            DrawText(" / ", 310, 250, 20, WHITE);
            DrawText("OFF", 350, 250, 20, WHITE);
        }
        else {
            DrawText("ON", 280, 250, 20, WHITE);
            DrawText(" / ", 310, 250, 20, WHITE);
            DrawText("OFF", 350, 250, 20, YELLOW);
        }
        DrawText(labelEnter, 120, 400, 20, WHITE);

        break;
    case STATE_PLAYING:
        RenderTiles();
        break;
    case STATE_LOSE:
        RenderTiles();
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(WHITE, 0.8f));
        DrawText(labelGameLose, SCREEN_WIDTH / 2 - MeasureText(labelGameLose, 60) / 2, SCREEN_HEIGHT / 2 - 10, 60, DARKGRAY);
        DrawText(labelEnter, SCREEN_WIDTH / 2 - MeasureText(labelEnter, 34) / 2, (int)(SCREEN_HEIGHT * 0.75f) - 10, 34, DARKGRAY);
        seconds = (int)(timeGameEnded - timeGameStarted) % 60;
        DrawText(TextFormat("TIME PLAYED: %d s", seconds), 20, 40, 34, DARKGRAY);
        break;
    case STATE_WIN:
        RenderTiles();
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(WHITE, 0.8f));
        DrawText(labelGameWin, SCREEN_WIDTH / 2 - MeasureText(labelGameWin, 60) / 2, SCREEN_HEIGHT / 2 - 10, 60, DARKGRAY);
        DrawText(labelEnter, SCREEN_WIDTH / 2 - MeasureText(labelEnter, 34) / 2, (int)(SCREEN_HEIGHT * 0.75f) - 10, 34, DARKGRAY);
        seconds = (int)(timeGameEnded - timeGameStarted) % 60;
        DrawText(TextFormat("TIME PLAYED: %d s", seconds), 20, 40, 34, DARKGRAY);

        break;
    }


}

void GameShutdown() {

    for (int i = 0; i < MAX_TEXTURES; i++) {
        UnloadTexture(textures[i]);
    }

    for (int i = 0; i < MAX_SOUNDS; i++) {
        UnloadSound(sounds[i]);
    }

    StopMusicStream(music[MUSIC_ONE]);
    UnloadMusicStream(music[MUSIC_ONE]);

    CloseAudioDevice();
}

void GameReset() {
    ResetTiles();
    gameState = STATE_PLAYING;
    revealedTilesCount = 0;
    timeGameStarted = (float)GetTime();
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib:: Minesweeper");
    SetTargetFPS(60);
    GameStartup();

    while (!WindowShouldClose()) {
        GameUpdate();

        BeginDrawing();
        ClearBackground(DARKBLUE);

        GameRender();

        EndDrawing();
    }

    GameShutdown();
    CloseWindow();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

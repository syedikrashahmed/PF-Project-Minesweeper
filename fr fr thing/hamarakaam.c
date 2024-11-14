#include "raylib.h"

typedef struct Cell
{
	int x;
	int y;
	bool mine;
	bool revealed;
	bool flagged;
	int nearbyMines;
} Cell;

int const cols = 10;
int const rows = 10;

int const cellWidth = 50;
int const cellHeight = 50;
const int screenWidth = 500;
const int screenHeight = 500;


void GridInitialize();


int main(void)
{

    InitWindow(screenWidth, screenHeight, "MINESWEEPER");
  

    SetTargetFPS(60);               

    while (!WindowShouldClose())   
    {
        BeginDrawing();

            ClearBackground(MAROON);

        EndDrawing();
    }


    CloseWindow();  

    GridInitialize();
    
    return 0;
}

void GridInitialize()
{
    for (int x = 0; x < screenWidth; x += 50) {
            // Draw vertical lines
            DrawLine(x, 0, x, screenHeight, GRAY);
        }
        
        for (int y = 0; y < screenHeight; y += 50) {
            // Draw horizontal lines
            DrawLine(0, y, screenWidth, y, GRAY);
        }

    
}

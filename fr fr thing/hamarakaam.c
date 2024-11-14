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

int const cellSize = 50;
const int screenSize = 500;


void GridInitialize();


int main(void)
{
    InitWindow(screenSize, screenSize, "MINESWEEPER");
    SetTargetFPS(60);               

    while (!WindowShouldClose())   
    {
        BeginDrawing();

            ClearBackground(MAROON);
            GridInitialize();

        EndDrawing();
    }

    CloseWindow();      
    return 0;
}

void GridInitialize()
{
    for (int x = 0; x < cellSize; x ++) 
    {
        for (int y = 0; y < cellSize; y ++) 
        {          
            DrawRectangleLines(x*cellSize, y*cellSize, cellSize, cellSize, WHITE);
            //taake screen ke har pixel pe lines na banen, cell size ke gap se banen
        }
    }    
}

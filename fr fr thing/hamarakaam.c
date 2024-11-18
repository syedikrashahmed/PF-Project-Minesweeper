#include "raylib.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef struct Cell
{
	int x;
	int y;
	bool bomb;
	bool revealed;
	bool flagged;
	int nearbyBombs;
} cell;

cell grid[10][10];

int const cols = 10;
int const rows = 10;

int const cellSize = 50;
const int screenSize = 500;

int IndexIsValid(int i, int j);
void GridInitialize();


int main(void)
{

    InitWindow(screenSize, screenSize, "MINESWEEPER");
  

    SetTargetFPS(60);   
    srand(time(0));    

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

int IndexIsValid(int i, int j)
{
    if (i>=0 && i<=9 && j>=0 && j<=9)
        return 1;
    else
        return 0;
}

//x and y is array position(row and column)
void GridInitialize()
{
    for (int x = 0; x < 10; x ++) 
    {
        for (int y = 0; y < 10; y ++) 
        {
            
            DrawRectangleLines(x*cellSize, y*cellSize, cellSize, cellSize, WHITE);
            //taake screen ke har pixel pe lines na banen, cell size ke gap se banen
        }
    }
    
    for(int i = 0; i < 10; i ++)
    {
        for (int j = 0; j < 10; j ++) 
        {
            grid[i][j].x = i;
            grid[i][j].y = j;
            grid[i][j].bomb = false; 
            grid[i][j].revealed = false;
            grid[i][j].flagged = false;
            grid[i][j].nearbyBombs = -1;
        }
    }
    
    //bombs idhar lagayen ge 
    int bombsToLaga = 20;
    while(bombsToLaga > 0)
    {
        
        int i = rand() % 10;
        int j = rand() % 10;
        if (grid[i][j].bomb == false)
        {
            grid[i][j].bomb = true;
            bombsToLaga--;
        }
    }
}

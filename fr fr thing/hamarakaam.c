#include "raylib.h"
#include <stdio.h>
#include <time.h>

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


//x and y is array position(row and column)
void GridInitialize()
/*
1) lines draw horai
2)initialization of class
3)bombs lagao
4) nearby bombs count 
*/
{
    for (int x = 0; x < cellSize; x ++) 
    {
        for (int y = 0; y < cellSize; y ++) 
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
        srand(time(0));
        int i = rand() % 10;
        int j = rand() % 10;
        if (grid[i][j].bomb == false)
        {
            grid[i][j].bomb = true;
            bombsToLaga--;
        }
    }
    
    //count mines in nearby boxes (loop runs for all boxes in the grid)
    for(int i = 0; i<10; i++)
    {
        for (int j = 0; j<10; j++)
        {
            int count = 0; //grid ke har box ko check karte huey usska inital count will be 0
            if (grid[i][j].bomb == false)
            {
                //if bomb nai hai, count aaju baaju waale tiles mei bombs
                for (int iOff = -1; iOff <= 1; iOff++)
                {
                    for (int jOff = -1; jOff <= 1; jOff++)
                    {
                        if (iOff == 0 && jOff == 0) //jis box ke around check kar rae hain, usse count nai karen ge
                        {
                            continue; 
                        }

                        if (!IndexIsValid(i + iOff, j + jOff)) // function defined later to check ke index out of bounds tou nai hai
                        {
                            continue;
                        }

                        if (grid[i + iOff][j + jOff].bomb)
                        {
                            count++;
                        }
                    }
                }
            }
            grid[i][j].nearbyBombs = count;
        }
    }
}

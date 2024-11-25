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

const int cellSize = 50;
const int screenSize = 500;

int IndexIsValid(int i, int j);
void GridInitialize();
void revealCell(cell); 
void PrintFlag(int i, int j);

int main(void)
{
    
    InitWindow(screenSize, screenSize, "MINESWEEPER");
  
    int gamestate = 2; //2 =game chalra, 1 = win, 0 = lost
    int revealedCount = 0;
    srand(time(0));  
    GridInitialize();  
    InitAudioDevice();
    Sound mineSound = LoadSound("mine.mp3");
    Sound clickSound = LoadSound("click.mp3");
    Sound clickFlag = LoadSound("flag.wav");
    Sound winSound = LoadSound("win.mp3");
    float time1, time2;
    time1 = GetTime();

    while (!WindowShouldClose())   
    {
       if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))   
       {
           Vector2 mousePos = GetMousePosition();
           int mouseI = mousePos.x / 50;
           int mouseJ = mousePos.y / 50;  
           if(grid[mouseI][mouseJ].flagged == false && grid[mouseI][mouseJ].revealed == false)
           {
                grid[mouseI][mouseJ].revealed = true;
                PlaySound(clickSound);
                if(grid[mouseI][mouseJ].bomb == false)
                {
                    revealedCount++;
                    if(revealedCount == 1)
                    {
                        gamestate = 1;
                        time2 = GetTime();
                        PlaySound(winSound);
                    }
                }
                if(grid[mouseI][mouseJ].bomb == true)
                    {
                        gamestate = 0;
                        PlaySound(mineSound);
                    }
           }
       }                
       else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))    
       {
           Vector2 mousePos = GetMousePosition();
           int mouseI = mousePos.x / 50;
           int mouseJ = mousePos.y / 50; 
           if(grid[mouseI][mouseJ].revealed == false)          
            {
                PlaySound(clickFlag);
                grid[mouseI][mouseJ].flagged = !grid[mouseI][mouseJ].flagged;
            }
       }
       
       
        BeginDrawing();

        ClearBackground(LIGHTGRAY);
        
        if(gamestate == 0)
        {  
            ClearBackground(MAROON);
            DrawText("YOU LOSE =/", 100, 220, 50, WHITE); 
        }
        else if(gamestate == 1)
        {
            ClearBackground(DARKGREEN);
            DrawText("YOU WIN =D", 100, 220, 50, WHITE); 
            DrawText(TextFormat("TIME PLAYED: %.2f s", time2-time1), 115, 280, 25, WHITE); 
            DrawText(TextFormat("HIGH SCORE: %.2f s", time2-time1), 125, 315, 25, WHITE);           
        }
        else
        {
            for (int x = 0; x < 10; x ++) 
            {
                for (int y = 0; y < 10; y ++) 
                {
                    revealCell(grid[x][y]);
                }
            }
        }

        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();  

    
    return 0;
}

void revealCell(cell cell1)
{
    //taake screen ke har pixel pe lines na banen, cell size ke gap se banen
    if (cell1.revealed == true && cell1.flagged == false)
    {
        if(cell1.bomb == true)
        {
            DrawRectangle(cell1.x * 50, cell1.y * 50, 50, 50, MAROON);
            
        }
        else if(cell1.bomb == false)
        {
            if(cell1.nearbyBombs == 0)
            {
                DrawRectangle(cell1.x * 50, cell1.y * 50, 50, 50, WHITE);
            }
            else if (cell1.nearbyBombs != 0)
            {    
                DrawRectangle(cell1.x * 50, cell1.y * 50, 50, 50, WHITE);
                DrawText(TextFormat("%d", cell1.nearbyBombs), cell1.x*50 + 18, cell1.y*50 + 12, 35, BLACK);
            }
        }
    }
    else if(cell1.revealed == false && cell1.flagged == true)
    {
        PrintFlag(cell1.x, cell1.y);
    }
    DrawRectangleLines(cell1.x*50, cell1.y*50, 50, 50, BLACK);
    
}



int IndexIsValid(int i, int j)
{
    if (i>=0 && i<=9 && j>=0 && j<=9)
        return 1;
    else
        return 0;
}

void PrintFlag(int i, int j)
{
    Vector2 v1 =  {(i * 50) + 10, (j * 50) + 5};
    Vector2 v2 =  {(i * 50) + 10, (j * 50) + 30};
    Vector2 v3 =  {(i * 50) + 40, (j * 50) + 15};
    DrawTriangle(v1, v2, v3, RED); 
    DrawLine((i * 50) + 11, (j * 50) + 30, (i * 50) + 11, (j * 50) + 45, BLACK);
}

//x and y is array position(row and column)
void GridInitialize()
{
    
    
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

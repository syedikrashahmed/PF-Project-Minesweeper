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
void displayBomb(int, int);

int main(void)
{
    float highest;
    FILE *fptr = fopen("highscore.txt", "r");
    if (fptr != NULL)
    {
        fscanf(fptr,"%f", &highest);
    }
    fclose(fptr);

    InitWindow(screenSize, screenSize, "MINESWEEPER");
  
    int gamestate = 2;
    int revealedCount = 0;
    int bombX, bombY;
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
                    if(revealedCount == 80)
                    {
                        gamestate = 1;
                        time2 = GetTime();
                        PlaySound(winSound);
                    }
                }
                if(grid[mouseI][mouseJ].bomb == true)
                    {
                        gamestate = 0;
                        bombX = mouseI;
                        bombY = mouseJ;
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
        if (gamestate != 0)
        {
            ClearBackground(LIGHTGRAY);
        }
        
        if(gamestate == 0)
        {  
            displayBomb(bombX, bombY); 
        }
        else if(gamestate == 1)
        {
            ClearBackground(DARKGREEN);
            DrawText("YOU WIN =D", 100, 220, 50, WHITE); 
            DrawText(TextFormat("TIME PLAYED: %.2f s", time2-time1), 115, 280, 25, WHITE); 
            if((time2-time1) < highest)
            {
                highest = time2-time1;
                fptr=fopen("highscore.txt","w");
                fprintf(fptr, "%f", highest);
                fclose(fptr);
                DrawText(TextFormat("YOU MADE THE HIGHEST SCORE: %.2f s", highest), 115, 315, 25, WHITE);  
            }         
            else
            {
                DrawText(TextFormat("HIGHEST SCORE: %.2f s", highest), 105, 330, 25, WHITE); 

            }
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
    if (cell1.revealed == true && cell1.flagged == false)
    {
        if(cell1.bomb == false)
        {
            if(cell1.nearbyBombs == 0)
            {
                DrawRectangle(cell1.x * 50, cell1.y * 50, 50, 50, WHITE);
            }
            else if (cell1.nearbyBombs != 0)
            {    
                DrawRectangle(cell1.x * 50, cell1.y * 50, 50, 50, WHITE);
                if (cell1.nearbyBombs == 1)
                {
                    DrawText(TextFormat("%d", cell1.nearbyBombs), cell1.x*50 + 16, cell1.y*50 + 10, 38, DARKBLUE);
                }
                else if (cell1.nearbyBombs == 2)
                {
                    DrawText(TextFormat("%d", cell1.nearbyBombs), cell1.x*50 + 16, cell1.y*50 + 10, 38, DARKGREEN);
                }
                else if (cell1.nearbyBombs == 3)
                {
                    DrawText(TextFormat("%d", cell1.nearbyBombs), cell1.x*50 + 16, cell1.y*50 + 10, 38, RED);
                }
                else if (cell1.nearbyBombs == 4)
                {
                    DrawText(TextFormat("%d", cell1.nearbyBombs), cell1.x*50 + 16, cell1.y*50 + 10, 38, DARKPURPLE);
                }
                else
                {
                    DrawText(TextFormat("%d", cell1.nearbyBombs), cell1.x*50 + 16, cell1.y*50 + 10, 38, BLACK);
                }
                  
            }
        }
    }
    else if(cell1.revealed == false && cell1.flagged == true)
    {
        PrintFlag(cell1.x, cell1.y);
    }
    DrawRectangleLines(cell1.x*50, cell1.y*50, 50, 50, BLACK);
    
}

void displayBomb(int bombX, int bombY)
{
    DrawRectangle(bombX*50, bombY*50, 49, 49, MAROON); 
    for (int i=0; i<10; i++)
    {
        for (int j=0; j<10; j++)
        {
             if(grid[i][j].flagged == true && grid[i][j].bomb == false)
             {
                Vector2 lineStart1 = {i*50,j*50};
                Vector2 lineEnd1 = {i*50 + 50,j*50 + 50};
                DrawLineEx(lineStart1, lineEnd1, 2.0, BLACK);
                Vector2 lineStart2 = {i*50,j*50 + 50};
                Vector2 lineEnd2 = {i*50 + 50,j*50};
                DrawLineEx(lineStart2, lineEnd2, 2.0, BLACK);
             }
             if(grid[i][j].flagged == false && grid[i][j].bomb == true)
             {
                DrawCircle(i*50 + 25, j*50 + 25, 13, BLACK);
                DrawCircle(i*50 + 22, j*50 + 22, 3, WHITE);
                
                Vector2 StartTop = {i*50 + 25, j*50 + 12};
                Vector2 EndTop = {i*50 + 25, j*50 + 5};
                DrawLineEx(StartTop, EndTop, 2.0, BLACK);
                Vector2 StartBottom = {i*50 + 25, j*50 + 38};
                Vector2 EndBottom = {i*50 + 25, j*50 + 45};
                DrawLineEx(StartBottom, EndBottom, 2.0, BLACK);
                Vector2 StartLeft = {i*50 + 12, j*50 + 25};
                Vector2 EndLeft = {i*50 + 5, j*50 + 25};
                DrawLineEx(StartLeft, EndLeft, 2.0, BLACK);
                Vector2 StartRight = {i*50 + 38, j*50 + 25};
                Vector2 EndRight = {i*50 +45, j*50 + 25};
                DrawLineEx(StartRight, EndRight, 2.0, BLACK);
                
                Vector2 DStartTopRight = {i*50 + 34, j*50 + 15.5};
                Vector2 DEndTopRight = {i*50 + 37, j*50 + 11.5};
                DrawLineEx(DStartTopRight, DEndTopRight, 2.0, BLACK);
                Vector2 DStartBottomRight = {i*50 + 34, j*50 + 34.5};
                Vector2 DEndBottomRight = {i*50 + 37, j*50 + 38.5};
                DrawLineEx(DStartBottomRight, DEndBottomRight, 2.0, BLACK);
                Vector2 DStartTopLeft = {i*50 + 16, j*50 + 15.5};
                Vector2 DEndTopLeft = {i*50 + 13, j*50 + 11.5};
                DrawLineEx(DStartTopLeft, DEndTopLeft, 2.0, BLACK);
                Vector2 DStartBottomLeft = {i*50 + 16, j*50 + 34.5};
                Vector2 DEndBottomLeft = {i*50 + 13, j*50 + 38.5};
                DrawLineEx(DStartBottomLeft, DEndBottomLeft, 2.0, BLACK);
             }         
        }
    }
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
            int count = 0; 
            if (grid[i][j].bomb == false)
            {
                for (int iOff = -1; iOff <= 1; iOff++)
                {
                    for (int jOff = -1; jOff <= 1; jOff++)
                    {
                        if (iOff == 0 && jOff == 0) 
                        {
                            continue; 
                        }
                        if (!IndexIsValid(i + iOff, j + jOff)) 
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

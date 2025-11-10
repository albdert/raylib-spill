#include "../include/raylib.h"
#include "raylib.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define GRID_CELL_SIZE 20
#define GRID_CELLS_X   50
#define GRID_CELLS_Y   34

// variabler
static int width = 1080;
static int height = 800;

// types
typedef struct {
  int x;
  int y;
} Point;

typedef enum  {
  EMPTY,
  METAL,
  STONE,
  SAND,
  WATER,
  NUM_BLOCKS
} Block_type;

static Color colors[] = {RAYWHITE, LIGHTGRAY,DARKGRAY, GOLD, BLUE};

void init_grid(int grid[GRID_CELLS_X][GRID_CELLS_Y]) {
  for (int i=0;i<GRID_CELLS_X;i++) {
    for (int j=0;j<GRID_CELLS_Y;j++)
      grid[i][j] = EMPTY;
  }
}
void init_buttons(Rectangle buttonrecs[], size_t len, Vector2 gridOffset) {
  for (int i=0; i<len; i++) {
    int x0 = (gridOffset.x+GRID_CELL_SIZE) + (4*i*GRID_CELL_SIZE);
    int y0 = (gridOffset.y+(GRID_CELL_SIZE*GRID_CELLS_Y)) + GRID_CELL_SIZE;
    int x1 = (2*GRID_CELL_SIZE);
    int y1 = (2*GRID_CELL_SIZE);

    Rectangle block = (Rectangle){x0,y0,x1,y1};
    buttonrecs[i] = block;
  }
}

void get_neighbours(int grid[GRID_CELLS_X][GRID_CELLS_Y], Point cell, int* neighbours) {
  // under og diagonal
  if (cell.y==GRID_CELLS_Y-1) {
    for (int i=0;i<3;i++) {neighbours[i]=1;}}
  else {
    neighbours[0] = grid[cell.x][cell.y+1];

    if (cell.x>=1) {
      neighbours[1] = grid[cell.x-1][cell.y+1];}
    if (cell.x<GRID_CELLS_X-1) {
      neighbours[2] = grid[cell.x+1][cell.y+1];}
  }

  // sider
  if (cell.x>=1) {
    neighbours[3] = grid[cell.x-1][cell.y];}
  if (cell.x<GRID_CELLS_X-1) {
    neighbours[4] = grid[cell.x+1][cell.y];}
}
bool check_under(int grid[GRID_CELLS_X][GRID_CELLS_Y], int newgrid[GRID_CELLS_X][GRID_CELLS_Y], int neighbours[], Point cell) {
  if (neighbours[0]==EMPTY) {
    newgrid[cell.x][cell.y+1] = grid[cell.x][cell.y];
    return true;}
  if (neighbours[0]==WATER && neighbours[0]>grid[cell.x][cell.y]) {
      newgrid[cell.x][cell.y] = neighbours[0];
      newgrid[cell.x][cell.y+1] = grid[cell.x][cell.y];
      grid[cell.x][cell.y+1] = grid[cell.x][cell.y];
      return true;}
  return false;
}
bool check_diagonals(int grid[GRID_CELLS_X][GRID_CELLS_Y], int newgrid[GRID_CELLS_X][GRID_CELLS_Y], int neighbours[], Point cell) {
  bool diff = false;
  if (neighbours[1]==EMPTY && newgrid[cell.x-1][cell.y+1]==EMPTY) {
    newgrid[cell.x-1][cell.y+1] = grid[cell.x][cell.y];
    return true;}
  if (neighbours[2]==EMPTY && newgrid[cell.x+1][cell.y+1]==EMPTY) {
    newgrid[cell.x+1][cell.y+1] = grid[cell.x][cell.y];
    return true;}
  return false;
}
bool check_sides(int grid[GRID_CELLS_X][GRID_CELLS_Y], int newgrid[GRID_CELLS_X][GRID_CELLS_Y], int neighbours[], Point cell) {
  bool diff = false;
  if (neighbours[3]==EMPTY) {
    if (newgrid[cell.x-1][cell.y]==EMPTY) {
      newgrid[cell.x-1][cell.y] = grid[cell.x][cell.y];
      return true;}
  }
  if (neighbours[4]==EMPTY) {
    if (newgrid[cell.x+1][cell.y]==EMPTY) {
      newgrid[cell.x+1][cell.y] = grid[cell.x][cell.y];
      return true;}
  }
  return false;
}

void update_grid(int grid[GRID_CELLS_X][GRID_CELLS_Y], int newgrid[GRID_CELLS_X][GRID_CELLS_Y], size_t gridsize) {
  int neighbours[5];
  bool diff = false;

  for (int x=0; x<GRID_CELLS_X; x++) {
    for (int y=0; y<GRID_CELLS_Y; y++) {
      if (grid[x][y]==EMPTY) { continue; }
      get_neighbours(grid, (Point){x,y}, neighbours);
      diff = false;
      if ( (grid[x][y]>1) && !diff) {
        diff = check_under(grid, newgrid, neighbours, (Point){x,y});}
      if ( (grid[x][y]>2) && !diff) {
        diff = check_diagonals(grid, newgrid, neighbours, (Point){x,y});}
      if ( (grid[x][y]>3) && !diff) {
      diff = check_sides(grid, newgrid, neighbours, (Point){x,y});}
      if (!diff) {
        newgrid[x][y] = grid[x][y];}
    }
  }
  memcpy(grid, newgrid, gridsize);
}

void draw_grid(Vector2 gridOffset) {
  // draw game grid
  for (int y = 0; y <= GRID_CELLS_Y; y++) {
    DrawLine((int)gridOffset.x, (int)gridOffset.y + y*GRID_CELL_SIZE,
        (int)gridOffset.x + GRID_CELLS_X*GRID_CELL_SIZE, (int)gridOffset.y + y*GRID_CELL_SIZE, GRAY);}
  for (int x = 0; x <= GRID_CELLS_X; x++) {
    DrawLine((int)gridOffset.x + x*GRID_CELL_SIZE, (int)gridOffset.y,
        (int)gridOffset.x + x*GRID_CELL_SIZE, (int)gridOffset.y + GRID_CELLS_Y*GRID_CELL_SIZE, GRAY);}
}
void draw_grid_blocks(int grid[GRID_CELLS_X][GRID_CELLS_Y], Vector2 gridOffset) {
  // draw blocks
  for (int x = 0; x < GRID_CELLS_X; x++) {
    for (int y = 0; y < GRID_CELLS_Y; y++) {
      if ((grid[x][y])==0) {continue;}
      DrawRectangle((int)gridOffset.x+x*GRID_CELL_SIZE,(int)gridOffset.y+y*GRID_CELL_SIZE,GRID_CELL_SIZE,
        GRID_CELL_SIZE, colors[grid[x][y]]);}
  }
}
void draw_buttons(Rectangle buttonrecs[], size_t len, Block_type selected) {
  for (int i=0; i<len; i++) {
    DrawRectangleRec(buttonrecs[i], colors[i]);
    if (i==selected) {
      DrawRectangleLinesEx(buttonrecs[i], 2, BLACK);}
    else {
      DrawRectangleLinesEx(buttonrecs[i], 1, GRAY);}
  }
}

void place_block(int grid[GRID_CELLS_X][GRID_CELLS_Y], Vector2 gridOffset, Vector2 mousepos, Block_type type) {
  int x = (int)(mousepos.x-gridOffset.x)/GRID_CELL_SIZE;
  int y = (int)(mousepos.y-gridOffset.y)/GRID_CELL_SIZE;

  // out of bounds
  if (x<0 || x>GRID_CELLS_X-1) {return;}
  if (y<0 || y>GRID_CELLS_Y-1) {return;}

  if (grid[x][y]!=EMPTY){return;}
  grid[x][y] = type;
}
void check_button_click(Vector2 mousepos, Rectangle buttonrecs[], size_t len, Block_type* selected) {
  for (int i=0; i<len; i++) {
    if (CheckCollisionPointRec(mousepos, buttonrecs[i])) {
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *selected = i;}
    }
  }
}
void check_block_placement(Vector2 mousepos, int newgrid[GRID_CELLS_X][GRID_CELLS_Y], Vector2 gridOffset, Block_type blocktype) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)
    ||IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
    place_block(newgrid, gridOffset, mousepos, blocktype);}
}

int main (void)
{
  InitWindow(width, height, "Powder simulation");

  // drawn grid offset
  Vector2 gridOffset = { 40, 20 };

  // grid og tmp grid for utregninger
  int grid[GRID_CELLS_X][GRID_CELLS_Y];
  int newgrid[GRID_CELLS_X][GRID_CELLS_Y];
  size_t gridsize = sizeof(grid);
  init_grid(grid);
  init_grid(newgrid);

  // button bounds
  Rectangle buttonrecs[NUM_BLOCKS];
  size_t buttoncnt = sizeof(buttonrecs)/sizeof(Rectangle);
  init_buttons(buttonrecs, buttoncnt, gridOffset);

  // selected block
  Block_type blocktype;
  // mouse position
  Vector2 mousepos;

  SetTargetFPS(20);
  while (!WindowShouldClose()) {

    /*--intput----------------------------------------------*/
     mousepos = GetMousePosition();
     check_button_click(mousepos, buttonrecs, buttoncnt, &blocktype);
     check_block_placement(mousepos, newgrid, gridOffset, blocktype);
    /*-----------------------------------------------------*/

    /*--update----------------------------------------------*/
    update_grid(grid, newgrid, gridsize);
    init_grid(newgrid);
    /*-----------------------------------------------------*/

    /*--drawing--------------------------------------------*/
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_grid_blocks(grid, gridOffset);
    draw_grid(gridOffset);
    draw_buttons(buttonrecs, buttoncnt, blocktype);
    /*------------------------------------------------------*/
    EndDrawing();
  }
  return 0;
}

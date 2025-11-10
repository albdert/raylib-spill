#include "../include/raylib.h"
#include "raylib.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#define CELL_SIZE 20
#define CELLS_X   52
#define CELLS_Y   38

// window
static int width = 1080;
static int height = 800;

// types
typedef struct {
  int x;
  int y;
} Point;

typedef enum  {
  DEAD,
  ALIVE,
} Cell_state;

static Color colors[] = {WHITE, BLACK};

void init_grid(int grid[][CELLS_Y]) {
  for (int i=0;i<CELLS_X;i++) {
    for (int j=0;j<CELLS_Y;j++)
      grid[i][j] = DEAD;
  }
}

int count_neighbours(int grid[][CELLS_Y], Point cell) {
  int cnt = 0;
  for (int y=-1; y<2; y++) {
    if (cell.y+y < 0) {continue;}
    for (int x=-1; x<2; x++) {
      if (cell.x+x < 0) {continue;}
      if (grid[cell.x+x][cell.y+y]) {
        cnt++;}
    }
  }
  return cnt;
}
void update_grid(int grid0[][CELLS_Y], int grid1[][CELLS_Y], size_t gridsize) {
  int n = 0;
  for (int x=0; x<CELLS_X; x++) {
  for (int y=0; y<CELLS_Y; y++) {
    n = count_neighbours(grid0, (Point){x,y});

    if (grid0[x][y]==DEAD) {
      if (n==3) {
        grid1[x][y] = ALIVE;}
      else {
        grid1[x][y] = DEAD;}
    }

    if (grid0[x][y]==ALIVE) {
      if ((n-1)<2) {
        grid1[x][y] = DEAD;}
      if ((n-1)==2 || (n-1)==3) {
        grid1[x][y] = ALIVE;}
      if ((n-1)>3) {
        grid1[x][y] = DEAD;}
    }
  }}
  memcpy(grid0, grid1, gridsize);
}

void draw_grid(Vector2 gridOffset) {
  // draw game grid
  for (int y = 0; y <= CELLS_Y; y++) {
    DrawLine((int)gridOffset.x, (int)gridOffset.y + y*CELL_SIZE,
        (int)gridOffset.x + CELLS_X*CELL_SIZE, (int)gridOffset.y + y*CELL_SIZE, GRAY);}
  for (int x = 0; x <= CELLS_X; x++) {
    DrawLine((int)gridOffset.x + x*CELL_SIZE, (int)gridOffset.y,
        (int)gridOffset.x + x*CELL_SIZE, (int)gridOffset.y + CELLS_Y*CELL_SIZE, GRAY);}
}
void draw_grid_cells(int grid[CELLS_X][CELLS_Y], Vector2 gridOffset) {
  // draw blocks
  for (int x = 0; x < CELLS_X; x++) {
    for (int y = 0; y < CELLS_Y; y++) {
      DrawRectangle((int)gridOffset.x+x*CELL_SIZE,(int)gridOffset.y+y*CELL_SIZE,CELL_SIZE,
        CELL_SIZE, colors[grid[x][y]]);}
  }
}


void toggle_cell(int grid[][CELLS_Y], Cell_state state, Vector2 gridOffset, Vector2 mousepos) {
  int x = (int)((mousepos.x-gridOffset.x)/CELL_SIZE);
  int y = (int)((mousepos.y-gridOffset.y)/CELL_SIZE);

  // out of bounds
  if ((mousepos.x-gridOffset.x)/CELL_SIZE < 0) {return;}
  if ((mousepos.y-gridOffset.y)/CELL_SIZE < 0) {return;}
  if (x<0 || x>CELLS_X-1) {return;}
  if (y<0 || y>CELLS_Y-1) {return;}

  if (grid[x][y]!=state){
    grid[x][y] = state;}
}
void check_cell_click(Vector2 mousepos, int grid[][CELLS_Y], Vector2 gridOffset) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)
    ||IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
    toggle_cell(grid, ALIVE, gridOffset, mousepos);}
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)
    ||IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
    toggle_cell(grid, DEAD, gridOffset, mousepos);}
}
void check_cell_clear(int grid[][CELLS_Y]) {
  if (IsKeyPressed(KEY_BACKSPACE)) {
    for (int x = 0; x < CELLS_X; x++) {
    for (int y = 0; y < CELLS_Y; y++) {
        grid[x][y] = DEAD;
    }}
  }
}
void check_start(bool* start) {
  if (IsKeyPressed(KEY_SPACE) && *start) {
      *start = false;
      return;
  }
  if (IsKeyPressed(KEY_SPACE)) {
    *start = true;
    return;
  }
}
void check_speed(int* speed) {
  if (IsKeyPressed(KEY_UP)) {
    if ((*speed)>40) {return;}
    *speed+= 1;
    return;
  }
  if (IsKeyPressed(KEY_DOWN)) {
    if (!(*speed)) {return;}
    *speed-= 1;
    return;
  }
}

int main (void)
{
  InitWindow(width, height, "Game of life");

  // drawn grid offset
  Vector2 gridOffset = { 20, 20 };

  // grid og tmp grid for utregninger
  int grid0[CELLS_X][CELLS_Y];
  int grid1[CELLS_X][CELLS_Y];
  size_t gridsize = sizeof(grid0);
  init_grid(grid0);
  init_grid(grid1);

  // mouse position
  Vector2 mousepos;
  // start simulation
  bool start;
  // frame counter
  int frames = 0;
  int speed  = 4;

  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    frames++;
    /*--intput----------------------------------------------*/
    mousepos = GetMousePosition();
    check_start(&start);
    //check_speed(&speed); WIP
    if (!start) {
      check_cell_click(mousepos, grid0, gridOffset);}
      check_cell_clear(grid0);
    /*-----------------------------------------------------*/

    /*--update----------------------------------------------*/
    if (start && (!(frames%speed))) {
      update_grid(grid0, grid1, gridsize);}
    init_grid(grid1);
    /*-----------------------------------------------------*/

    /*--drawing--------------------------------------------*/
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_grid_cells(grid0, gridOffset);
    draw_grid(gridOffset);
    //draw_buttons(buttonrecs, buttoncnt, blocktype);
    /*------------------------------------------------------*/
    EndDrawing();

    // reset framecounter
    if (frames>60) {frames=0;}
  }
  return 0;
}

#ifndef MAZE_H
#define MAZE_H
#include <stdbool.h>

typedef struct {
  bool visited;
  bool tip;
  bool lower;
  bool right;
} cell;

extern int maze_width;
extern int maze_height;

void initMaze(cell maze[maze_height][maze_width]);
void genMaze(cell maze[maze_height][maze_width]);
void printMaze(cell maze[maze_height][maze_width]);

#endif

#ifndef MAZE_H
#define MAZE_H
#include <stdbool.h>

typedef struct {
  bool visited;
  bool lower;
  bool right;
} cell;
struct node {
  int col;
  int row;
  struct node *prev;
};

extern int maze_width;
extern int maze_height;

void initMaze(cell maze[maze_height][maze_width]);
void genMaze(cell maze[maze_height][maze_width]);
void printMaze(cell maze[maze_height][maze_width]);
int countNeighbours(cell maze[maze_height][maze_width], struct node *current);
bool isDone(cell maze[maze_height][maze_width]);

#endif

#include "headers/maze.h"

int main() {
  maze_height = 20;
  maze_width = 50;
  cell m[maze_height][maze_width];
  initMaze(m);
  genMaze(m);
  printMaze(m);
  return 0;
}
